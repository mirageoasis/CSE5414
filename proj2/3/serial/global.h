#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} pixel;

const char *FILE_NAME = "test.ppm";