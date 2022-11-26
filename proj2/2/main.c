#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "mpi.h"

int root(int num){

}

int main(int argc,char *argv[])
{
    int rank, num_of_processes;
    int i;

    MPI_Comm comm = MPI_COMM_WORLD;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(comm, &num_of_processes);
    MPI_Comm_rank(comm, &rank);

    int localsum = 0;
    int globalsum = 0;
    int expectedsum = 0;
    
    if(rank == 0) {
        printf("Checking mpi_scan(sum)... (if you see no output then you are good)\n");
    }

    localsum = do_something(rank, 2);
    globalsum = 0;
    MPI_Scan(&localsum,&globalsum,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);

    expectedsum = 0;
    // count upto my rank and verify that that was the return from scan
    
    for()


        
    if (globalsum != expectedsum) {
        printf("ERROR: Expected %d got %d [rank:%d]\n", expectedsum, globalsum, rank);
    }
        
    MPI_Finalize();
}