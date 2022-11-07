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

int row;
int col;
int max;

unsigned char **original_image; // 이미지 배열
unsigned char **reverse_image;  // 리버스 배열
unsigned char **gray_image;  // 회색 배열
unsigned char **smooth_image;  // 스무스 배열

int copy_ppm(unsigned char **dest, unsigned char **src)
{
    for (int i = 0; i < row; i++)
        for (int j = 0; j < col * 3; j++)
            dest[i][j] = src[i][j];
}

int read_ppm()
{

    FILE *fp;

    fp = fopen(FILE_NAME, "rb");

    fscanf(fp, "%c%c", &p, &number); // 헤더를 읽는 곳
    fscanf(fp, "%d %d", &row, &col);
    fscanf(fp, "%d", &max);

    // fprintf(stdout, "%d %d\n", row, col);
    // fprintf(stdout, "%d\n", max);
    //  row 개수 만큼 할당

    // col * 3 만큼 할당 -> 이유는 rgb 3개의 값을 사용하기 때문

    // 공간 할당
    original_image = (unsigned char **)malloc(sizeof(unsigned char *) * row);

    for (int i = 0; i < row; i++)
        original_image[i] = (unsigned char *)malloc(sizeof(unsigned char) * col * 3);

    //공간 할당 완료
    // fprintf(stdout, "ready for read!\n");
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col * 3; j++)
        {
            fread(&original_image[i][j], sizeof(unsigned char), 1, fp);
        }
    }

    fclose(fp);
}

int write_ppm(unsigned char **image, const char *name)
{
    FILE *fp;

    fp = fopen(name, "w");

    fprintf(fp, "%c%c\n", p, number); // 헤더를 읽는 곳
    fprintf(fp, "%d %d\n", row, col);
    fprintf(fp, "%d", max);

    // col * 3 만큼 할당 -> 이유는 rgb 3개의 값을 사용하기 때문

    // 공간 할당

    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col * 3; j += 3)
        {
            fprintf(fp, "%c", image[i][j]);
            fprintf(fp, "%c", image[i][j + 1]);
            fprintf(fp, "%c", image[i][j + 2]);
        }
    }

    fclose(fp);
}

int reverse_ppm()
{
    // 좌우반전
    // 공간 할당

    reverse_image = (unsigned char **)malloc(sizeof(unsigned char *) * row);

    for (int i = 0; i < row; i++)
        reverse_image[i] = (unsigned char *)malloc(sizeof(unsigned char) * col * 3);
    
    copy_ppm(reverse_image, original_image);
    
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col / 2; j++)
        {
            unsigned char temp_r = reverse_image[i][j*3];
            unsigned char temp_g = reverse_image[i][j*3+1];
            unsigned char temp_b = reverse_image[i][j*3+2];

            reverse_image[i][j*3] = reverse_image[i][(col - j - 1) * 3];
            reverse_image[i][j*3+1] = reverse_image[i][(col - j - 1) * 3 + 1];
            reverse_image[i][j*3+2] = reverse_image[i][(col - j - 1) * 3 + 2];

            reverse_image[i][(col - j - 1) * 3] = temp_r;
            reverse_image[i][(col - j - 1) * 3 + 1] = temp_g;
            reverse_image[i][(col - j - 1) * 3 + 2] = temp_b;
        }
    }


    //공간 할당 완료
}

int gray_ppm()
{
    // rgb 값을 모두 평균으로 만들어준다.
    gray_image = (unsigned char **)malloc(sizeof(unsigned char *) * row);

    for (int i = 0; i < row; i++)
        gray_image[i] = (unsigned char *)calloc(col * 3 ,sizeof(unsigned char));

    copy_ppm(gray_image, original_image);

     for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            unsigned char temp_r = gray_image[i][j*3];
            unsigned char temp_g = gray_image[i][j*3+1];
            unsigned char temp_b = gray_image[i][j*3+2];
            float mod = 0.299 * (float)temp_r + 0.587 * (float)temp_g + 0.114 * (float)temp_b;
            //float mod = ((float)temp_r +  (float)temp_g + (float)temp_b) / 3.0;
            //fprintf(stdout,"%f %d\n", mod, (unsigned char)mod);
            gray_image[i][j*3] = (unsigned char)mod;
            gray_image[i][j*3+1] = (unsigned char)mod;
            gray_image[i][j*3+2] = (unsigned char)mod;
        }
    }
    
}

int smooth_ppm()
{
    // 주위 8개
    smooth_image = (unsigned char **)malloc(sizeof(unsigned char *) * row);

    for (int i = 0; i < row; i++)
        smooth_image[i] = (unsigned char *)calloc(col * 3 ,sizeof(unsigned char));

    copy_ppm(smooth_image, original_image);

    for(int i = 1; i < row - 1; i++){
        for(int j = 1; j < col - 1; j++){

            float temp_r = (float)(
                                    smooth_image[i-1][(j-1)*3] + smooth_image[i-1][j*3] + smooth_image[i-1][(j+1)*3]
                                    + smooth_image[i][(j-1)*3] + smooth_image[i][j*3] + smooth_image[i][(j+1)*3]
                                    + smooth_image[i+1][(j-1)*3] + smooth_image[i+1][j*3] + smooth_image[i+1][(j+1)*3]
                                    ) / 9.0;
            float temp_g = (float)(
                                    smooth_image[i-1][(j-1)*3+1] + smooth_image[i-1][j*3+1] + smooth_image[i-1][(j+1)*3+1]
                                    + smooth_image[i][(j-1)*3+1] + smooth_image[i][j*3+1] + smooth_image[i][(j+1)*3+1]
                                    + smooth_image[i+1][(j-1)*3+1] + smooth_image[i+1][j*3+1] + smooth_image[i+1][(j+1)*3+1]
                                    ) / 9.0;
            float temp_b = (float)(
                                    smooth_image[i-1][(j-1)*3+2] + smooth_image[i-1][j*3+2] + smooth_image[i-1][(j+1)*3+2]
                                    + smooth_image[i][(j-1)*3+2] + smooth_image[i][j*3+2] + smooth_image[i][(j+1)*3+2]
                                    + smooth_image[i+1][(j-1)*3+2] + smooth_image[i+1][j*3+2] + smooth_image[i+1][(j+1)*3+2]
                                    ) / 9.0;
            //fprintf(stdout, "%f %f %f\n", temp_r, temp_g, temp_b);
            //fprintf(stdout, "%d %d %d\n", (char)temp_r, (char)temp_g, (char)temp_b);
            smooth_image[i][j*3] = (unsigned char)temp_r;
            smooth_image[i][j*3+1] = (unsigned char)temp_g;
            smooth_image[i][j*3+2] = (unsigned char)temp_b;
        }
    }


}

int main()
{

    read_ppm();
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

    return 0;
}