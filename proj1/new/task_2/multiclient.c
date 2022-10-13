#include "csapp.h"
#include <time.h>

sem_t filemutex;
sem_t countmutex;
int total_bytes = 0;
FILE* fp;

typedef struct MultipleArg
{
	char *host;
	char *port;
	int num_access;
}MultipleArg;

void *thread(void *vargp);

int main(int argc, char **argv)
{

	int runprocess = 0, status, i;

	int clientfd, num_client, num_access;
	char *host, *port, buf[MAXLINE], *file;
	rio_t rio;
	pthread_t* tid;
	
	if (argc != 6)
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
	fp = fopen(file, "r");
	// tid 배열 만들기
	tid = (pthread_t*)malloc(sizeof(pthread_t) * num_client);
	// semaphore 초기화
	Sem_init(&filemutex, 0, 1);
	Sem_init(&countmutex, 0, 1);


	MultipleArg* args = (MultipleArg*) malloc(sizeof(MultipleArg));
	args->host = host;
	args->port = port;
	args->num_access = num_access;

	for (int i = 0; i < num_client; i++) /* Create worker threads */
		Pthread_create(&tid[i], NULL, thread, args);

	/*	fork for each client process	*/
	
	// wait for all threads
	for (int i = 0; i < num_client; i++) /* Create worker threads */
		Pthread_join(tid[i], NULL);

	free(tid);
	fclose(fp);
	fprintf(stdout, "total %d bytes received from server!\n", total_bytes);
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
		// buf 에 입력 받아오기 스레드 safe 하지 않아도 문제 없을꺼 같음
		// 파일 접근시에는 무조건 lock을 건다.
		fprintf(stdout, "reading files!\n");
		fgets(buf , MAXLINE, fp);
		fprintf(stdout, "%s\n", buf);
		fprintf(stdout, "reading files!\n");
		sem_post(&filemutex);
		
		Rio_writen(clientfd, buf, strlen(buf));
		// Rio_readlineb(&rio, buf, MAXLINE);
		Rio_readnb(&rio, buf, MAXLINE);
		Fputs(buf, stdout);
		//Fputs('\n', stdout);
		
		sem_wait(&countmutex);
		total_bytes += strlen(buf);
		sem_post(&countmutex);
		//sleep(1);
	}

	Close(clientfd);
}