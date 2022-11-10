#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *FILE_NAME = "test.ppm";
const char *REVERSE_FILE_NAME = "reverse.ppm";
const char *GRAY_FILE_NAME = "gray.ppm";
const char *SMOOTH_FILE_NAME = "smooth.ppm";
const char *TEST_FILE_NAME = "twin.ppm";

char p;
char number;

unsigned int row;
unsigned int col;
unsigned int max;

typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} pixel;

pixel *original_image; // 이미지 배열
pixel *reverse_image;  // 리버스 배열
pixel *gray_image;     // 회색 배열
pixel *smooth_image;   // 스무스 배열

int alloc_space(pixel **target)
{
    *target = (pixel *)malloc(sizeof(pixel) * row * col);
}

int copy_ppm(pixel *dest, pixel *src)
{
    for (int i = 0; i < row; i++)
        for (int j = 0; j < col; j++)
            dest[i * row + j] = src[i * row + j];
}

int read_ppm()
{

    FILE *fp;

    fp = fopen(FILE_NAME, "rb");

    fscanf(fp, "%c%c", &p, &number); // 헤더를 읽는 곳
    fscanf(fp, "%u %u", &row, &col);
    fscanf(fp, "%u", &max);
    fseek(fp, 1, SEEK_CUR); /* skip one byte, should be whitespace */
    fprintf(stdout, "%d %d\n", row, col);
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

int reverse_ppm()
{
    // 좌우반전
    // 공간 할당
    memcpy(reverse_image, original_image, sizeof(pixel) * row * col);

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

    //공간 할당 완료
}

int gray_ppm()
{
    // rgb 값을 모두 평균으로 만들어준다.
    memcpy(gray_image, original_image, sizeof(pixel) * row * col);

    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            unsigned char temp_r = gray_image[i * row + j].r;
            unsigned char temp_g = gray_image[i * row + j].g;
            unsigned char temp_b = gray_image[i * row + j].b;
            float mod = 0.299 * (float)temp_r + 0.587 * (float)temp_g + 0.114 * (float)temp_b;
            // float mod = ((float)temp_r +  (float)temp_g + (float)temp_b) / 3.0;
            // fprintf(stdout,"%f %d\n", mod, (unsigned char)mod);
            gray_image[i * row + j].r = (unsigned char)mod;
            gray_image[i * row + j].g = (unsigned char)mod;
            gray_image[i * row + j].b = (unsigned char)mod;
        }
    }
}


int smooth_ppm()
{
    // 주위 8개
    memcpy(smooth_image, original_image, sizeof(pixel) * row * col);

    for (int i = 1; i < row - 1; i++)
    {
        for (int j = 1; j < col - 1; j++)
        {

            float temp_r = (float)(smooth_image[(i - 1) * row + (j - 1)].r + smooth_image[(i - 1) * row + j].r + smooth_image[(i - 1) * row + (j + 1)].r + smooth_image[i * row + (j - 1)].r + smooth_image[i * row + j].r + smooth_image[i * row + (j + 1)].r + smooth_image[(i + 1) * row + (j - 1)].r + smooth_image[(i + 1) * row + j].r + smooth_image[(i + 1) * row + (j + 1)].r) / 9.0;
            float temp_g = (float)(smooth_image[(i - 1) * row + (j - 1)].g + smooth_image[(i - 1) * row + j].g + smooth_image[(i - 1) * row + (j + 1)].b + smooth_image[i * row + (j - 1)].g + smooth_image[i * row + j].g + smooth_image[i * row + (j + 1)].g + smooth_image[(i + 1) * row + (j - 1)].g + smooth_image[(i + 1) * row + j].g + smooth_image[(i + 1) * row + (j + 1)].g) / 9.0;
            float temp_b = (float)(smooth_image[(i - 1) * row + (j - 1)].b + smooth_image[(i - 1) * row + j].b + smooth_image[(i - 1) * row + (j + 1)].g + smooth_image[i * row + (j - 1)].b + smooth_image[i * row + j].b + smooth_image[i * row + (j + 1)].b + smooth_image[(i + 1) * row + (j - 1)].b + smooth_image[(i + 1) * row + j].b + smooth_image[(i + 1) * row + (j + 1)].b) / 9.0;
            // fprintf(stdout, "%f %f %f\n", temp_r, temp_g, temp_b);
            // fprintf(stdout, "%d %d %d\n", (char)temp_r, (char)temp_g, (char)temp_b);
            smooth_image[i * row + j].r = (unsigned char)temp_r;
            smooth_image[i * row + j].g = (unsigned char)temp_g;
            smooth_image[i * row + j].b = (unsigned char)temp_b;
        }
    }
}


int main()
{

    // 공간 할당

    //공간 할당 완료

    read_ppm();
    alloc_space(&reverse_image);
    alloc_space(&gray_image);
    alloc_space(&smooth_image);

    write_ppm(original_image, TEST_FILE_NAME);
    fprintf(stdout, "file twin complete!\n");

    reverse_ppm();
    write_ppm(reverse_image, REVERSE_FILE_NAME);
    fprintf(stdout, "file reverse complete!\n");

    gray_ppm();
    write_ppm(gray_image, GRAY_FILE_NAME);
    fprintf(stdout, "file gray complete!\n");
    
    smooth_ppm();
    write_ppm(smooth_image, SMOOTH_FILE_NAME);
    fprintf(stdout, "file smooth complete!\n");
    
    // write 함수를 모듈화 하자

    free(original_image);
    free(reverse_image);
    free(gray_image);
    free(smooth_image);

    return 0;
}