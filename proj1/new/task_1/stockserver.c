/*
 * echoservere.c - An iterative echo server
 */
/* $begin echoserverimain */

#include "stockserver.h"

#define SBUFSIZE 1024
#define NTHREADS 5

int lines = 0; /*파일 라인 수(주식의 개수)*/
int loop = 1;  /*루프 지속할지 말지 결정하는 변수*/
int clientNumber = 0;
int total_bytes = 0;

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
    pthread_t* tid;
    
    // int status = 0; // 주식 현재 상태

    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }

    listenfd = Open_listenfd(argv[1]);

    tid = (pthread_t*)malloc(sizeof(pthread_t) * NTHREADS);

    for (int i = 0; i < NTHREADS; i++) /* Create worker threads */
        Pthread_create(&tid, NULL, thread, NULL);
    

    for (int i = 0; i < NTHREADS; i++) /* Create worker threads */
		Pthread_join(tid[i], NULL);

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
    
    // epoll 하고 해당 connfd 를 echo_cnt로 건내준다.
    int event_cnt;

    while (true) {
        if ((event_cnt =
                 epoll_wait(ep_fd, ep_events, MAX_POOL, ep_timeout)) == -1) {
            unix_error("epoll error!\n");
        }

        for (int i = 0; i < ep_event_cnt; i++) {
            if (ep_events[i].data.fd == sock_fd) {
                if ((con_fd = accept(sock_fd, (struct sockaddr*) &c_address,
                                     (socklen_t*) &c_len)) == -1) {
                    error("Failed to accept socket connection\n");
                    exit(1);
                }

                echo_cnt(con_fd);
            }
        }
    }


}
