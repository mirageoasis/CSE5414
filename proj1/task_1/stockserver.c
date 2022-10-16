/*
 * echoservere.c - An iterative echo server
 */
/* $begin echoserverimain */

#include "stockserver.h"

#define SBUFSIZE 1024
#define NTHREADS 10000

int lines = 0; /*파일 라인 수(주식의 개수)*/
int loop = 1;  /*루프 지속할지 말지 결정하는 변수*/
int clientNumber = 0;
int total_bytes = 0;
struct epoll_event ep_events[SBUFSIZE];

void echo(int connfd);
void command(char *BUF2, char *buf, char *argv[], char *clientBuf);
void sigint_handler();
void *thread(void *vargp);

typedef struct args
{
    int listenfd;
    int epfd;
} args;

int main(int argc, char **argv)
{
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr; /* Enough space for any address */ // line:netp:echoserveri:sockaddrstorage
    sigset_t signal_pool;
    pthread_t *tid;
    int epfd;
    struct epoll_event ep_event;

    // int status = 0; // 주식 현재 상태

    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    // tid 열어두기
    tid = (pthread_t *)malloc(sizeof(pthread_t) * NTHREADS);

    // listen할 port를 열어두는 것
    listenfd = Open_listenfd(argv[1]);

    // args
    args *arg = (args *)malloc(sizeof(args));

    if ((epfd = epoll_create(SBUFSIZE)) == -1)
    {
        unix_error("Failed to create epoll\n");
        exit(1);
    }

    ep_event.events = EPOLLIN | EPOLLET;
    ep_event.data.fd = listenfd;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ep_event) == -1)
    {
        unix_error("Failed to register to epoll\n");
        close(epfd);
        exit(1);
    }

    arg->epfd = epfd;
    arg->listenfd = listenfd;

    for (int i = 0; i < NTHREADS; i++) /* Create worker threads */
        Pthread_create(&tid, NULL, thread, arg);

    for (int i = 0; i < NTHREADS; i++) /* Create worker threads */
        Pthread_join(tid[i], NULL);

    free(arg);
    return 0;
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
        // printf("server received %d bytes\n", n);
        // printf("input is %s :\n", buf);
        //줄바꿈 문자까지 받는다.
        input(buf, clientBuf);
        total_bytes += strlen(clientBuf);
        // printf("%s\n", clientBuf);
        Rio_writen(connfd, clientBuf, MAXLINE);
        // fprintf(stdout, "command: %s", buf);
    }
}

void *thread(void *vargp)
{

    int epfd = ((args *)(vargp))->epfd;
    int listenfd = ((args *)(vargp))->listenfd;

    // epoll 하고 해당 connfd 를 echo_cnt로 건내준다.
    int event_cnt;
    int connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;

    while (true)
    {
        if ((event_cnt = epoll_wait(epfd, ep_events, SBUFSIZE, -1)) == -1)
        {
            unix_error("epoll error!\n");
        }
        //fprintf(stdout, "event cnt : %d %d\n", epfd, listenfd);

        for (int i = 0; i < event_cnt; i++)
        {
            if (ep_events[i].data.fd == listenfd)
            {
                connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
                echo_cnt(connfd);
                Close(connfd);
            }
        }
    }
}