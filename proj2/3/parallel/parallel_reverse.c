#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

const char *FILE_NAME = "test.ppm";
const char *REVERSE_FILE_NAME = "reverse.ppm";

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

int alloc_space(pixel **target)
{
    *target = (pixel *)malloc(sizeof(pixel) * row * col);
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


int main(int argc, char* argv[])
{

    int n_elements_recieved;
    int row_recieved;
    int col_recieved;
    int num_of_processes, rank;
    MPI_Status status;
    MPI_Request request[1000000];

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_of_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    pixel* buffer; // slave에서 받을 버퍼

    if (rank == 0)
    {      

        printf("\nRANK 0 is working\n\n");

        read_ppm();

        
        int elements_size = ( (sizeof(pixel) * row * col) / num_of_processes);
        for(int i = 1; i < num_of_processes; i++)
        {
            //size를 보냄
            printf("file_size = %d \n elements_size = %d\n\n",(sizeof(pixel) * row * col),elements_size);
            MPI_Send(&elements_size,1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&row,1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&col,1, MPI_INT, i, 0, MPI_COMM_WORLD);

            MPI_Send((unsigned char *)original_image + (elements_size * i), elements_size , MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD);
        }

        alloc_space(&reverse_image);

        //memcpy(reverse_image, original_image, elements_size);
        for (int i = 0; i < (row / num_of_processes); i++)
        {
            for (int j = 0; j < col / 2; j++)
            {
                unsigned char temp_r = original_image[i * col + j].r;
                unsigned char temp_g = original_image[i * col + j].g;
                unsigned char temp_b = original_image[i * col + j].b;

                reverse_image[i * col + j].r = original_image[i * col + (col - j - 1)].r;
                reverse_image[i * col + j].g = original_image[i * col + (col - j - 1)].g;
                reverse_image[i * col + j].b = original_image[i * col + (col - j - 1)].b;

                reverse_image[i * col + (col - j - 1)].r = temp_r;
                reverse_image[i * col + (col - j - 1)].g = temp_g;
                reverse_image[i * col + (col - j - 1)].b = temp_b;
            }
        }

        //나머지것들 보낸 work 값 기다리고 조각 받아서 붙인다 비동기
        for(int i = 1; i < num_of_processes; i++)
        {
            MPI_Irecv((unsigned char *)reverse_image + (elements_size * i),elements_size,
                 MPI_UNSIGNED_CHAR, i, 0,
                 MPI_COMM_WORLD,
                 &request[i]);
        }
        for(int i = 1; i < num_of_processes; i++)
        {
            MPI_Wait(&request[i], &status);
        }


        write_ppm(reverse_image, REVERSE_FILE_NAME);
        fprintf(stdout, "file reverse complete!\n");
        //memcpy(reverse_image, original_image, sizeof(pixel) * row * col);


        free(original_image);
        free(reverse_image);
        

        
        // write_ppm(reverse_image, REVERSE_FILE_NAME);
        // fprintf(stdout, "file reverse complete!\n");

        //free
    }
    else
    {

        MPI_Recv(&n_elements_recieved,
                 1, MPI_INT, 0, 0,
                 MPI_COMM_WORLD,
                 &status);
        MPI_Recv(&row_recieved,
                 1, MPI_INT, 0, 0,
                 MPI_COMM_WORLD,
                 &status);
        MPI_Recv(&col_recieved,
                 1, MPI_INT, 0, 0,
                 MPI_COMM_WORLD,
                 &status);
  
        // stores the received array segment
        buffer = (pixel*)malloc(sizeof(pixel) * n_elements_recieved);
        MPI_Recv(buffer, n_elements_recieved,
                 MPI_UNSIGNED_CHAR, 0, 0,
                 MPI_COMM_WORLD,
                 &status);

        printf("\nRANK %d\n n_elements_recieved =  %d\n\n row_recieved =  %d\n\n col_recieved =  %d\n\n",rank,n_elements_recieved,row_recieved,col_recieved);

        row_recieved = row_recieved / num_of_processes;
        for (int i = 0; i < row_recieved; i++)
        {
            for (int j = 0; j < col_recieved / 2; j++)
            {
                unsigned char temp_r = buffer[i * col_recieved + j].r;
                unsigned char temp_g = buffer[i * col_recieved + j].g;
                unsigned char temp_b = buffer[i * col_recieved + j].b;

                buffer[i * col_recieved + j].r = buffer[i * col_recieved + (col_recieved - j - 1)].r;
                buffer[i * col_recieved + j].g = buffer[i * col_recieved + (col_recieved - j - 1)].g;
                buffer[i * col_recieved + j].b = buffer[i * col_recieved + (col_recieved - j - 1)].b;

                buffer[i * col_recieved + (col_recieved - j - 1)].r = temp_r;
                buffer[i * col_recieved + (col_recieved - j - 1)].g = temp_g;
                buffer[i * col_recieved + (col_recieved - j - 1)].b = temp_b;
            }
        }

        MPI_Send((unsigned char *)buffer, n_elements_recieved , MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD);
        printf("\nRANK %d is get data sent \n\n",rank);
    }

    MPI_Barrier(MPI_COMM_WORLD); // block 하기
    MPI_Finalize();
    //     free(original_image);
    //     free(reverse_image);
    //free
    return 0;
}