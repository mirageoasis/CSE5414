/*
 * echoservere.c - An iterative echo server
 */
/* $begin echoserverimain */

#include "stockserver.h"

typedef struct
{ /* Represents a pool of connected descriptors */ // line:conc:echoservers:beginpool
    int maxfd;                                     /* Largest descriptor in read_set */
    fd_set read_set;                               /* Set of all active descriptors */
    fd_set ready_set;                              /* Subset of descriptors ready for reading  */
    int nready;                                    /* Number of ready descriptors from select */
    int maxi;                                      /* Highwater index into client array */
    int clientfd[FD_SETSIZE];                      /* Set of active descriptors */
    rio_t clientrio[FD_SETSIZE];                   /* Set of active read buffers */
} pool;                                            // line:conc:echoservers:endpool

int lines = 0; /*파일 라인 수(주식의 개수)*/
int loop = 1;  /*루프 지속할지 말지 결정하는 변수*/
int clientNumber = 0;

void echo(int connfd);
void init_pool(int listenfd, pool *p);
void add_client(int connfd, pool *p);
int check_clients(pool *p);
void command(char *BUF2, char *buf, char *argv[], char *clientBuf);

int main(int argc, char **argv)
{
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr; /* Enough space for any address */ // line:netp:echoserveri:sockaddrstorage
    static pool pool;
    sigset_t signal_pool;
    // char client_hostname[MAXLINE], client_port[MAXLINE];
    // int status = 0; // 주식 현재 상태

    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }

    read_file();

    // fprintf(stdout, "read file done!\n");

    // building binary tree
    binary_tree_init(lines); /*이진 트리 빌드하기(qsort로 배열 정렬하고 head 설정)*/

    // fprintf(stdout, "building tree done!\n");
    assert(tree_head != NULL);
    // fprintf(stdout, "head %d %d %d\n", tree_head->ID, tree_head->left_stock, tree_head->price);

    // levelOrder(tree_head, lines); // test code

    listenfd = Open_listenfd(argv[1]);
    init_pool(listenfd, &pool); // line:conc:echoservers:initpool

    while (loop)
    {
        pool.ready_set = pool.read_set;
        // fprintf(stdout, "before select!\n");
        pool.nready = Select(pool.maxfd + 1, &pool.ready_set, NULL, NULL, NULL);
        // fprintf(stdout, "after select!\n");
        /* If listening descriptor ready, add new client to pool */
        if (FD_ISSET(listenfd, &pool.ready_set))
        { // line:conc:echoservers:listenfdready
            clientlen = sizeof(struct sockaddr_storage);
            connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); // line:conc:echoservers:accept
            add_client(connfd, &pool);                                // line:conc:echoservers:addclient
        }
        /* Echo a text line from each ready connected descriptor */
        check_clients(&pool); // line:conc:echoservers:checkclients
        // save_binary_tree(tree_head); // 파일에 트리를 밀어넣는다
    }

    exit(0);
}

/* $begin init_pool */
void init_pool(int listenfd, pool *p)
{
    /* Initially, there are no connected descriptors */
    int i;
    p->maxi = -1; // line:conc:echoservers:beginempty
    for (i = 0; i < FD_SETSIZE; i++)
        p->clientfd[i] = -1; // line:conc:echoservers:endempty

    /* Initially, listenfd is only member of select read set */
    p->maxfd = listenfd; // line:conc:echoservers:begininit
    FD_ZERO(&p->read_set);
    FD_SET(listenfd, &p->read_set); // line:conc:echoservers:endinit
}
/* $end init_pool */

/* $begin add_client */
void add_client(int connfd, pool *p)
{
    int i;
    p->nready--;
    for (i = 0; i < FD_SETSIZE; i++) /* Find an available slot */
        if (p->clientfd[i] < 0)
        {
            clientNumber++;
            // fprintf(stdout, "clientNumber is %d\n", clientNumber);
            /* Add connected descriptor to the pool */
            p->clientfd[i] = connfd;                 // line:conc:echoservers:beginaddclient
            Rio_readinitb(&p->clientrio[i], connfd); // line:conc:echoservers:endaddclient

            /* Add the descriptor to descriptor set */
            FD_SET(connfd, &p->read_set); // line:conc:echoservers:addconnfd

            /* Update max descriptor and pool highwater mark */
            if (connfd > p->maxfd) // line:conc:echoservers:beginmaxfd
                p->maxfd = connfd; // line:conc:echoservers:endmaxfd
            if (i > p->maxi)       // line:conc:echoservers:beginmaxi
                p->maxi = i;       // line:conc:echoservers:endmaxi
            break;
        }
    if (i == FD_SETSIZE) /* Couldn't find an empty slot */
        app_error("add_client error: Too many clients");
}
/* $end add_client */

/* $begin check_clients */
int check_clients(pool *p)
{
    int i, connfd, n;
    char buf[MAXLINE];       // 기존 입력
    char BUF2[MAXLINE];      // 복사본
    char clientBuf[MAXLINE]; // client 한테 보낼 입력
    char *argv[MAXLINE];
    rio_t rio;

    for (i = 0; (i <= p->maxi) && (p->nready > 0); i++)
    {
        connfd = p->clientfd[i];
        rio = p->clientrio[i];

        /* If the descriptor is ready, echo a text line from it */
        if ((connfd > 0) && (FD_ISSET(connfd, &p->ready_set)))
        {
            p->nready--;
            if ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0)
            {
                // fprintf(stdout, "%s", buf);
                command(BUF2, buf, argv, clientBuf);
                // fprintf(stdout, "%s", clientBuf);
                Rio_writen(connfd, clientBuf, MAXLINE); // line:conc:echoservers:endecho
            }
            else /* EOF detected, remove descriptor from pool */
            {
                clientNumber--;
                // fprintf(stdout, "clientNumber is %d\n", clientNumber);
                assert(clientNumber >= 0);
                Close(connfd);                   // line:conc:echoservers:closeconnfd
                FD_CLR(connfd, &p->read_set);    // line:conc:echoservers:beginremove
                p->clientfd[i] = -1;             // line:conc:echoservers:endremove
                // fprintf(stdout, "connection ended! event based server\n");
            }
        }
    }
    return 0;
}
/* $end check_clients */

void command(char *BUF2, char *buf, char *argv[], char *clientBuf)
{
    
}
