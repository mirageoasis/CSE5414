/*
 * echoservert.c - An iterative echo server
 */
/* $begin echoserverimain */

#include "stockserver.h"
#define SBUFSIZE 1024
#define NTHREADS 1024

int lines = 0; /*파일 라인 수(주식의 개수)*/
int loop = 1;  /*루프 지속할지 말지 결정하는 변수*/
sem_t filemutex;
int clientNumber = 0;
int total_bytes = 0;

typedef struct
{
    int *buf;    /* Buffer array */
    int n;       /* Maximum number of slots */
    int front;   /* buf[(front+1)%n] is first item */
    int rear;    /* buf[rear%n] is last item */
    sem_t mutex; /* Protects accesses to buf */
    sem_t slots; /* Counts available slots */
    sem_t items; /* Counts available items */
} sbuf_t;

sbuf_t sbuf;
void sbuf_init(sbuf_t *sp, int n);
void sbuf_deinit(sbuf_t *sp);
void sbuf_insert(sbuf_t *sp, int item);
int sbuf_remove(sbuf_t *sp);

void echo(int connfd);
void command(char *BUF2, char *buf, char *argv[], char *clientBuf);
void sigint_handler();
void *thread(void *vargp);

int main(int argc, char **argv)
{
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr; /* Enough space for any address */ // line:netp:echoserveri:sockaddrstorage
    sigset_t signal_pool;
    pthread_t tid;
    sem_init(&filemutex, 0, 1);
    // int status = 0; // 주식 현재 상태

    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }

    listenfd = Open_listenfd(argv[1]);

    sbuf_init(&sbuf, SBUFSIZE);
    for (int i = 0; i < NTHREADS; i++) /* Create worker threads */
        Pthread_create(&tid, NULL, thread, NULL);
    while (1)
    {
        clientlen = sizeof(struct sockaddr_storage);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        sbuf_insert(&sbuf, connfd); /* Insert connfd in buffer */
    }

    exit(0);
}

void echo_cnt(int connfd)
{
    // 이 함수가 client 로 부터 입력을 받는 함수다.
    int n;
    char buf[MAXLINE];       // 원본
    char clientBuf[MAXLINE]; // client 한테 보낼 입력
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0)
    {
        //printf("server received %d bytes\n", n);
        //printf("input is %s :\n", buf);
        //줄바꿈 문자까지 받는다.
        input(buf, clientBuf);
        total_bytes += strlen(clientBuf);
        //printf("%s\n", clientBuf);
        Rio_writen(connfd, clientBuf, MAXLINE);
        //fprintf(stdout, "command: %s", buf);
    }
}

void *thread(void *vargp)
{
    Pthread_detach(pthread_self());
    while (1)
    {
        int connfd = sbuf_remove(&sbuf); /* Remove connfd from buf */
        sem_wait(&filemutex);            // 연결 끝나면 정산 time
        clientNumber++;
        sem_post(&filemutex);
        // 여기가 이제 mico_httpd 입력 들어올 곳이다. echo_cnt 함수
        echo_cnt(connfd); /* Service client */

        sem_wait(&filemutex); // 연결 끝나면 정산 time
        clientNumber--;
        assert(clientNumber >= 0);
        sem_post(&filemutex);
        
        Close(connfd);
    }
}

/* Create an empty, bounded, shared FIFO buffer with n slots */
void sbuf_init(sbuf_t *sp, int n)
{
    sp->buf = Calloc(n, sizeof(int));
    sp->n = n;                  /* Buffer holds max of n items */
    sp->front = sp->rear = 0;   /* Empty buffer iff front == rear */
    Sem_init(&sp->mutex, 0, 1); /* Binary semaphore for locking */
    Sem_init(&sp->slots, 0, n); /* Initially, buf has n empty slots */
    Sem_init(&sp->items, 0, 0); /* Initially, buf has 0 items */
}

void sbuf_deinit(sbuf_t *sp)
{
    Free(sp->buf);
}

void sbuf_insert(sbuf_t *sp, int item)
{
    P(&sp->slots);                          /* Wait for available slot */
    P(&sp->mutex);                          /* Lock the buffer */
    sp->buf[(++sp->rear) % (sp->n)] = item; /* Insert the item */
    V(&sp->mutex);                          /* Unlock the buffer */
    V(&sp->items);                          /* Announce available item */
}

/* Remove and return the first item from buffer sp */
int sbuf_remove(sbuf_t *sp)
{
    int item;
    P(&sp->items);                           /* Wait for available item */
    P(&sp->mutex);                           /* Lock the buffer */
    item = sp->buf[(++sp->front) % (sp->n)]; /* Remove the item */
    V(&sp->mutex);                           /* Unlock the buffer */
    V(&sp->slots);                           /* Announce available slot */
    return item;
}