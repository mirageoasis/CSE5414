#include "global.h"

extern const char *FILE_NAME;
const char *REVERSE_FILE_NAME = "reverse.ppm";

char p;
char number;

unsigned int row;
unsigned int col;
unsigned int max;

pixel *original_image; // 이미지 배열
pixel *reverse_image;  // 리버스 배열

int alloc_space(pixel **target)
{
    *target = (pixel *)malloc(sizeof(pixel) * row * col);
}

int read_ppm()
{

    FILE *fp;

    fp = fopen(FILE_NAME, "rb");

    //fscanf(fp, "%c%c", &p, &number); // 헤더를 읽는 곳
    p = fgetc(fp);
    number = fgetc(fp);
    fgetc(fp);
    fscanf(fp, "%u %u", &row, &col);
    fscanf(fp, "%u", &max);
    fgetc(fp);
    //fseek(fp, 1, SEEK_CUR); /* skip one byte, should be whitespace */
    fprintf(stdout, "%d %d\n", row, col);
    fprintf(stdout, "%d\n", max);
    // fprintf(stdout, "%d\n", max);
    //  row 개수 만큼 할당

    // col * 3 만큼 할당 -> 이유는 rgb 3개의 값을 사용하기 때문
    original_image = (pixel *)malloc(sizeof(pixel) * row * col);

    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            // fprintf(stdout, "i %d j %d i * row + j %d\n", i, j, i * row + j);
            fread(&(original_image[i * row + j].r), sizeof(unsigned char), 1, fp);
            fread(&(original_image[i * row + j].g), sizeof(unsigned char), 1, fp);
            fread(&(original_image[i * row + j].b), sizeof(unsigned char), 1, fp);
        }
    }

    fclose(fp);
}

int write_ppm(pixel *image, const char *name)
{
    FILE *fp;

    fp = fopen(name, "w");

    fprintf(fp, "%c%c\n", p, number); // 헤더를 읽는 곳
    fprintf(fp, "%d %d\n", row, col);
    fprintf(fp, "%d\n", max);

    // col * 3 만큼 할당 -> 이유는 rgb 3개의 값을 사용하기 때문

    // 공간 할당

    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            pixel temp = image[i * row + j];
            fprintf(fp, "%c", temp.r);
            fprintf(fp, "%c", temp.g);
            fprintf(fp, "%c", temp.b);
        }
    }

    fclose(fp);
}


int main(int argc, char* argv[])
{
    read_ppm();
    alloc_space(&reverse_image);
    
    memcpy(reverse_image, original_image, sizeof(pixel) * row * col);
    struct timeval startTime, endTime;
    double diffTime;

    gettimeofday(&startTime, NULL);
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col / 2; j++)
        {
            unsigned char temp_r = reverse_image[i * row + j].r;
            unsigned char temp_g = reverse_image[i * row + j].g;
            unsigned char temp_b = reverse_image[i * row + j].b;

            reverse_image[i * row + j].r = reverse_image[i * row + (col - j - 1)].r;
            reverse_image[i * row + j].g = reverse_image[i * row + (col - j - 1)].g;
            reverse_image[i * row + j].b = reverse_image[i * row + (col - j - 1)].b;

            reverse_image[i * row + (col - j - 1)].r = temp_r;
            reverse_image[i * row + (col - j - 1)].g = temp_g;
            reverse_image[i * row + (col - j - 1)].b = temp_b;
        }
    }
    gettimeofday(&endTime, NULL);
    diffTime = ( endTime.tv_sec - startTime.tv_sec ) + (( endTime.tv_usec - startTime.tv_usec ) / 1000000.0);
    write_ppm(reverse_image, REVERSE_FILE_NAME);
    fprintf(stdout, "file reverse complete in %f seconds!\n", diffTime);


    //free
    free(original_image);
    free(reverse_image);

    //free
    return 0;
}