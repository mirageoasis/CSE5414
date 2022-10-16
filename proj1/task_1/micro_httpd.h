#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SERVER_NAME "micro_httpd"
#define SERVER_URL "http://www.acme.com/software/micro_httpd/"
#define PROTOCOL "HTTP/1.0"
#define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT"

/* Forwards. */
static void file_details(char *dir, char *name);
static void send_error(int status, char *title, char *extra_header, char *text, char* clientBuf);
static int send_headers(int status, char *title, char *extra_header, char *mime_type, off_t length, time_t mod, char* clientBuf);
static char *get_mime_type(char *name);
static void strdecode(char *to, char *from);
static int hexit(char c);
static void strencode(char *to, size_t tosize, const char *from);
int input(char* buf, char* clientBuf);