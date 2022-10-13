#include "csapp.h"
#include <time.h>

sem_t filemutex;
int total_bytes = 0;
FILE* fp;

typedef struct MultipleArg
{
	char *host;
	char *port;
	int num_access
}MultipleArg;

int main(int argc, char **argv)
{

	int runprocess = 0, status, i;

	int clientfd, num_client, num_access;
	char *host, *port, buf[MAXLINE], *file;
	rio_t rio;
	pthread_t tid;
	
	if (argc != 5)
	{
		fprintf(stderr, "usage: %s <host> <port> <client#> <number of accesss> <name of file>\n", argv[0]);
		exit(0);
	}
	// ip 주소, 포트번호, 클라이언트 개수, 접근 횟수
	host = argv[1];
	port = argv[2];
	num_client = atoi(argv[3]);
	num_access = atoi(argv[4]);
	file = argv[5];
	// 파일 열기
	fp = fopen(file, 'r');
	
	MultipleArg* args = (MultipleArg*) malloc(sizeof(MultipleArg));
	args->host = host;
	args->port = port;
	args->num_access = num_access;

	for (int i = 0; i < num_client; i++) /* Create worker threads */
		Pthread_create(&tid, NULL, thread, args);

	/*	fork for each client process	*/

	// wait(&state);
	
	
	fclose(fp);

	return 0;
}

void *thread(void *vargp)
{
	MultipleArg * args = (MultipleArg*) vargp;

	char* port = args->port;
	char* host = args->host;
	int num_access  = args->num_access;
	rio_t rio;
	char buf[MAXLINE];
	int clientfd = Open_clientfd(host, port); 
	Rio_readinitb(&rio, clientfd);
	
	for (int i = 0; i < num_access; i++)
	{
		sem_wait(&filemutex); // 연결 끝나면 정산 time
		// command line 읽어오기
		// clientNumber++;
		// buf 에 입력 받아오기
		fscanf(fp ,"%s", buf);
		sem_post(&filemutex);
		fprintf(stdout, "%s", buf);
		Rio_writen(clientfd, buf, strlen(buf));
		// Rio_readlineb(&rio, buf, MAXLINE);
		Rio_readnb(&rio, buf, MAXLINE);
		Fputs(buf, stdout);
		
	}

	Close(clientfd);
}