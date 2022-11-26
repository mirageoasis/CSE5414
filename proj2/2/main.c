#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "mpi.h"

<<<<<<< HEAD
int root(int num){

=======
#define ARRAY_SIZE 100

long long int original[ARRAY_SIZE];
long long int copy_mpi[ARRAY_SIZE];
long long int copy_seq[ARRAY_SIZE];

int log_two(int number)
{
    int ret = 0;
    int compare = 1;

    while (compare <= number)
    {
        compare *= 2;
        ret += 1;
    }
    return ret - 1;
>>>>>>> 4914d872b97cfeb939f683ad5b85cfc0688d68eb
}

long long int pow_two(int number)
{
    int count = 1;
    long long int ret = 1;
    while (count <= number)
    {
        ret *= 2;
        count += 1;
    }
    return ret;
}

int main(int argc, char *argv[])
{
    // 맨 앞에서 모든 원소를 생성
    srand((unsigned int)time(NULL));
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        original[i] = rand() % 128 + 1;
    }
    memcpy(copy_mpi, original, sizeof(long long int) * ARRAY_SIZE);
    memcpy(copy_seq, original, sizeof(long long int) * ARRAY_SIZE);

    // 사전 작업 해주기
    struct timeval startTime, endTime;
    double diffTime;
    gettimeofday(&startTime, NULL);

    for (int i = 1; i < ARRAY_SIZE; i++)
        copy_seq[i] += copy_seq[i - 1];

    gettimeofday(&endTime, NULL);
    diffTime = (endTime.tv_sec - startTime.tv_sec) + ((endTime.tv_usec - startTime.tv_usec) / 1000000.0);
    // fprintf(stdout, "file reverse complete in %f seconds!\n", diffTime);

    // printf("%d\n", log_two(ARRAY_SIZE));

    MPI_Init(&argc, &argv);
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Request request[10000];
    MPI_Status status;

    int rank, num_of_processes;
    MPI_Comm_size(comm, &num_of_processes);
    MPI_Comm_rank(comm, &rank);

    fprintf(stdout, "number of process %d rank %d\n", num_of_processes, rank);

    int per_rank = ARRAY_SIZE / num_of_processes;
    int leftover = ARRAY_SIZE % num_of_processes;


    // 마지막 꼭다리는 process 당 할당량 + 꼭다리 해준다.
    if (!leftover)
        leftover = per_rank;
    else
        leftover += per_rank;
    
    int step = 0;
    for (; step <= log_two(ARRAY_SIZE); step++)
    {
        // 한번의 step 마다 계속 send 와 recevie 를 반복
        if (rank == 0)
        {
            //long long int *puppet = (long long int *)malloc(sizeof(long long int) * per_rank);
            long long int puppet[ARRAY_SIZE];
            // 자기일 처리하고 남들 한테 메시지 보내기
            // puppet은 자기 복제본
            memcpy(puppet, copy_mpi, sizeof(long long int) * ARRAY_SIZE);
            
            for (int i = 1; i < num_of_processes; i++)
            {
                int number_to_receive;
                if (i != num_of_processes - 1)
                    number_to_receive = per_rank;
                else
                    number_to_receive = leftover;


                // 다른 rank 에 메시지를 보내면서 받는 것을 비동기로 해준다.
                MPI_Send((long long int *)puppet + (per_rank * i), number_to_receive,
                          MPI_LONG_LONG_INT, i, 0,
                          MPI_COMM_WORLD);
            }
            
            for (int i = 0; i < per_rank; i++)
            {
                // 보내 줄 곳 rank 계산하기
                // 만약에 해당 index 존재 안하면 안보낸다.
                int now_index = rank * per_rank + i; // 현재 rank 곱하기 할당량 + i
                int index_to_receive = now_index - pow_two(step);
                // 받아올 index가 없으면 계산 무효~
                if (index_to_receive < 0)
                    continue;
                puppet[i] = puppet[i] + copy_mpi[index_to_receive];
            }
            
            for (int i = 1; i < num_of_processes; i++)
            {
                int number_to_receive;
                if (i != num_of_processes - 1)
                    number_to_receive = per_rank;
                else
                    number_to_receive = leftover;


                // 다른 rank 에 메시지를 보내면서 받는 것을 비동기로 해준다.
                MPI_Irecv((long long int *)puppet + (per_rank * i), number_to_receive,
                          MPI_LONG_LONG_INT, i, 0,
                          MPI_COMM_WORLD,
                          &request[i]);
            }

            for (int i = 1; i < num_of_processes; i++)
            {
                MPI_Wait(&request[i], &status);
            }
            memcpy(copy_mpi, puppet, sizeof(long long int) * ARRAY_SIZE);
            //free(puppet);
        }
        else
        {
            int number_to_receive;

            if(rank != num_of_processes - 1)
                number_to_receive = per_rank;
            else
                number_to_receive = leftover;
            
            long long int *buffer = (long long int *)
                                    malloc(sizeof(long long int) * number_to_receive);
            // 0번 rank 에서 오는 신호를 받아준다.
            MPI_Recv(buffer, number_to_receive,
                MPI_LONG_LONG_INT, 
                0, 0, MPI_COMM_WORLD, &status
            );
            // 받아온 개수를 토대로
            for (int i = 0; i < number_to_receive; i++)
            {
                // index 계산하기
                // 보내 줄 곳 rank 계산하기
                // 만약에 해당 index 존재 안하면 안보낸다.
                int now_index = rank * per_rank + i; // 현재 rank 곱하기 할당량 + i
                int index_to_receive = now_index - pow_two(step);
                // 받아올 index가 없으면 계산 무효~
                if (index_to_receive < 0)
                    continue;
                buffer[i] = buffer[i] + copy_mpi[index_to_receive];
            }
            MPI_Send(buffer, number_to_receive,
                MPI_LONG_LONG_INT, 
                0, 0, MPI_COMM_WORLD
            );
            fprintf(stdout, "rank %d received %d elements!\n", rank, number_to_receive);
            //free(buffer);
        }

    }
    
    MPI_Finalize();
    
    // 둘이 일치하는지 디버깅
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        fprintf(stdout, "%lld prefix sum seq : %lld mpi : %lld! pos : %d\n", original[i],copy_seq[i], copy_mpi[i], i);
        if (copy_seq[i] != copy_mpi[i])
        {
            fprintf(stdout, "prefix sum differ on postion %d!\n", i);
            return 1;
        }
    }
    fprintf(stdout, "prefix sum equals\n");
}