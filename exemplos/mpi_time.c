#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "mpi.h"

int main( int argc, char *argv[] )
{
    double t1, t2;
    int rank, size;

    MPI_Init(&argc , &argv); // funcao que inicializa o MPI, todo o codigo paralelo estah abaixo

    t1 = MPI_Wtime();        // contagem de tempo inicia neste ponto

    MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
    MPI_Comm_size (MPI_COMM_WORLD, &size);        /* get number of processes */
    printf( "Hello world from process %d of %d\n", rank, size );

    sleep(10);               // espera aproximadamente 30 segundos

    t2 = MPI_Wtime();        // contagem de tempo termina neste ponto

    printf("MPI_Wtime measured a 10 second sleep to be: %1.2f\n", t2-t1);
    fflush(stdout);

    MPI_Finalize( );

    return 0;
}
