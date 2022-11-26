/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "cal.h"
#define MAXSIZE 256

void caleprog_1(char *host)
{
	CLIENT *clnt;
	int *result_1;
	int add_1_first = 4;
	int add_1_second = 5;
	int *result_2;
	int sub_1_first;
	int sub_1_second;
	int *result_3;
	int mul_1_first;
	int mul_1_second;
	int *result_4;
	int div_1_first = 4;
	int div_1_second = 4;
	int *result_5;
	int pow_1_first;
	int pow_1_second;

#ifndef DEBUG
	clnt = clnt_create(host, CALEPROG, CALMESSAGEVERS, "udp");
	if (clnt == NULL)
	{
		clnt_pcreateerror(host);
		exit(1);
	}
#endif /* DEBUG */
	add_1(0, 0, clnt);
	result_1 = add_1(add_1_first, add_1_second, clnt);
	fprintf(stdout, "%d\n", *result_1);
	if (result_1 == (int *)NULL)
	{
		clnt_perror(clnt, "call failed");
	}
	result_2 = sub_1(sub_1_first, sub_1_second, clnt);
	if (result_2 == (int *)NULL)
	{
		clnt_perror(clnt, "call failed");
	}
	result_3 = mul_1(mul_1_first, mul_1_second, clnt);
	if (result_3 == (int *)NULL)
	{
		clnt_perror(clnt, "call failed");
	}
	result_4 = div_1(div_1_first, div_1_second, clnt);
	if (result_4 == (int *)NULL)
	{
		clnt_perror(clnt, "call failed");
	}
	result_5 = pow_1(pow_1_first, pow_1_second, clnt);
	if (result_5 == (int *)NULL)
	{
		clnt_perror(clnt, "call failed");
	}
#ifndef DEBUG
	clnt_destroy(clnt);
#endif /* DEBUG */
}

typedef struct Node
{
	int data;
	// -1 plus
	// -2 sub
	// -3 div
	// -4 mul
	// -5 pow
	struct Node *next;
} Node;

typedef struct
{
	Node *top; // top pointer 선언
} Stack;

Node *buffer_list_head = NULL; // 처음에 buffer 에서 연산을 받아서 리스트로 만들어준다.
Node *buffer_list_tail = NULL;

void buffer_list_add(int value)
{

	Node *pnew = (Node *)malloc(sizeof(Node));
	pnew->data = value;
	pnew->next = NULL;
	// 노드 값 설정

	if (buffer_list_head == NULL)
	{
		// 아무것도 없는 경우
		buffer_list_head = pnew;
		buffer_list_tail = pnew;
	}
	else
	{
		// 일반적인 경우
		buffer_list_tail->next = pnew;
		buffer_list_tail = pnew;
	}
}

int main(int argc, char *argv[])
{
	char *host;

	if (argc < 2)
	{
		printf("usage: %s server_host\n", argv[0]);
		exit(1);
	}

	// host 받아오기
	host = argv[1];
	// caleprog_1 (host);

	char buffer[MAXSIZE + 1];
	// 연산 먼저하기
	fgets(buffer, MAXSIZE, stdin);
	// printf("%s\n", buffer);
	//  argument list 만들기

	int mul_count = 0;
	int num_count = 0; // 숫자인지 아닌지 판별
	int num_stack = 0; // 숫자 남았는지 계산

	for (int i = 0; i < strlen(buffer) - 1; i++)
	{
		char c = buffer[i];
		// printf("%c ", buffer[i]);
		// printf("%d ", i);
		//  문자열 읽기
		switch (c)
		{
		case '+':
			buffer_list_add(num_stack);
			//printf("number\n");
			buffer_list_add(-1);
			mul_count = 0;
			num_count = 0;
			num_stack = 0;
			//printf("plus\n");
			break;
		case '-':
			buffer_list_add(num_stack);
			//printf("number\n");
			buffer_list_add(-2);
			mul_count = 0;
			num_count = 0;
			num_stack = 0;
			//printf("sub\n");
			break;
		case '/':
			buffer_list_add(num_stack);
			//printf("number\n");
			buffer_list_add(-3);
			mul_count = 0;
			num_count = 0;
			num_stack = 0;
			//printf("div\n");
			break;
		case '*':
			if (mul_count)
			{
				// ** 연산
				buffer_list_add(-5);
				//printf("pow\n");
			}else{
				buffer_list_add(num_stack);
				//printf("number\n");
			}
			mul_count ^= 1;
			num_count = 0;
			num_stack = 0;
			break;
		default:
			if (mul_count)
			{ // 전에 나온 문자가 곱셈이다.
				// 곱셈 연산
				buffer_list_add(-4);
				//printf("mul\n");
			}
			num_stack *= 10;
			num_stack += buffer[i] - '0';

			// printf("number\n");
			mul_count = 0;
			break;
		}
	}

	buffer_list_add(num_stack);
	//printf("number\n");

	Node * cur = buffer_list_head;

	while(cur != NULL){
		if (cur->data > -1){
			fprintf(stdout, "%d", cur->data);
		}else{
			switch(cur->data){
				case -1:
					fprintf(stdout, "+");
					break;
				case -2:
					fprintf(stdout, "-");
					break;
				case -3:
					fprintf(stdout, "/");
					break;
				case -4:
					fprintf(stdout, "*");
					break;
				case -5:
					fprintf(stdout, "**");
					break;
			}
		}
		cur = cur->next;
	}
	fprintf(stdout, "\n");
	// 완료


	// 연산 우선 순위 +, - / *, / / **
	exit(0);
	CLIENT *clnt;
	clnt = clnt_create(host, CALEPROG, CALMESSAGEVERS, "udp");
	if (clnt == NULL)
	{
		clnt_pcreateerror(host);
		exit(1);
	}

	// 연산 문자열 받아주기

	clnt_destroy(clnt);
	exit(0);
}