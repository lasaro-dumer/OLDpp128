#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mpi.h"
#define GET_WORK 0
#define WORK_DONE 1
#define WORK 2
#define SUICIDE 3
#define NUM_ARRAYS 2
#define ARRAYS_SIZE 5

int compare (const void * a, const void * b){
  return ( *(int*)a - *(int*)b );
}

const char * printTag(int tag){
    if(tag== GET_WORK){
        return "GET_WORK";
    }else if(tag==WORK_DONE){
        return "WORK_DONE";
    }else if(tag==WORK){
        return "WORK";
    }else if(tag==SUICIDE){
        return "SUICIDE";
    }else{
        return "Invalid Tag";
    }
}

const double curMilis(){
    struct timeval  tv;
    gettimeofday(&tv, NULL);

    return ((tv.tv_sec) * 1000 + (tv.tv_usec) / 1000.0) +0.5; // convert tv_sec & tv_usec to millisecond
}

main(int argc, char** argv){
    int my_rank;       // Identificador deste processo
    int proc_n;        // Numero de processos disparados pelo usuÃ¡rio na linha de comando (np)
    int **saco;
    int * toOrder;
    int **ordered;
    int i,val;

	srand(time(NULL));
	int r = rand();

    if ( my_rank == 0 ){
        saco = malloc(NUM_ARRAYS * sizeof(int *));
        if(saco == NULL)
        {
            printf("out of memory\n");
            return -1;
        }else{
            for(i = 0; i < NUM_ARRAYS; i++)
            {
                saco[i] = malloc(ARRAYS_SIZE * sizeof(int));
                if(saco[i] == NULL)
                {
                    printf("out of memory row %d\n",i);
                    return -1;
                }else{
                    for(j = 0; j< ARRAYS_SIZE; j++)
                    {
                        saco[i][j] = ARRAYS_SIZE - j;
                    }
                }
            }
        }

        ordered = malloc(NUM_ARRAYS * sizeof(int *));
        if(ordered == NULL)
        {
            printf("out of memory\n");
            return -1;
        }else{
            for(i = 0; i < NUM_ARRAYS; i++)
            {
                ordered[i] = malloc(ARRAYS_SIZE * sizeof(int));
                if(ordered[i] == NULL)
                {
                    printf("out of memory row %d\n",i);
                    return -1;
                }else{
                    /*
                    for(j = 0; j< ARRAYS_SIZE; j++)
                    {
                        ordered[i][j] = 0;
                    }//*/
                }
            }
        }
    }

    MPI_Status status; /* Status de retorno */

    MPI_Init(&argc , & argv); // funcao que inicializa o MPI, todo o cÃ³digo paralelo esta abaixo

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_n);
	int dones = 0;
    int slavesAlive = proc_n-1;

    if ( my_rank == 0 ){
        int next = 0;
        for(s=1;s<slavesAlive;s++){
            if(next>=NUM_ARRAYS){
                val=0;
                MPI_Send(&val, 8, MPI_INT,s, SUICIDE, MPI_COMM_WORLD);
                slavesAlive--;
            }else {
                MPI_Send(saco[next], 8, MPI_INT,s, WORK, MPI_COMM_WORLD);
                next++;
            }
        }

		while(slavesAlive > 0){
            MPI_Recv(toOrder, 8, MPI_INT,MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);  // recebo por ordem de chegada com any_source
			if(status.MPI_TAG == WORK_DONE){
                ordered[dones]=toOrder;
                dones++;
			}else if(status.MPI_TAG == GET_WORK){
                if(next>=NUM_ARRAYS){
                    val=0;
                    MPI_Send(&val, 8, MPI_INT,status.MPI_SOURCE, SUICIDE, MPI_COMM_WORLD);
                    slavesAlive--;
    			}else {
                    val = saco[next];
    				MPI_Send(saco[next], 8, MPI_INT,status.MPI_SOURCE, WORK, MPI_COMM_WORLD);
                    next++;
    			}
            }
		}
        printf("[%f]@master leaving...\n",curMilis());
     }
     else
     {
		int tag = WORK;
		do{
            MPI_Recv(toOrder, 8, MPI_INT,0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			tag = status.MPI_TAG;
			if(tag == WORK){
                qsort (toOrder, ARRAYS_SIZE, sizeof(int), compare);
				MPI_Send(toOrder,  8, MPI_INT,0, WORK_DONE, MPI_COMM_WORLD);
			}
            if(tag != SUICIDE){
                MPI_Send(&tag,  8, MPI_INT,0, GET_WORK, MPI_COMM_WORLD);
            }
		}while(tag != SUICIDE)
        printf("[%f]@slave[%d] leaving...\n",curMilis(),my_rank);
     }

    MPI_Finalize();

}
