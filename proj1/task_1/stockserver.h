#include "csapp.h"
#include "struct.h"
#include "file_manage.h"
#include "argv.h"
#include "micro_httpd.h"
#include <sys/epoll.h>


#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"

STOCK_NODE *temp_array; /*처음에 파일 읽을 때 임시 저장하는 array*/
STOCK_NODE *tree_head;  /*이진 트리의 root node*/