#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "mpi.h"
#define TAREFAS 5
#define GET_WORK 0
#define WORK_DONE 1
#define WORK 2
#define SUICIDE 3
int compare (const void * a, const void * b)
{
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

const char * curTime(){
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime ( &rawtime );
    return asctime (timeinfo);
}

main(int argc, char** argv)
{
    int my_rank;       // Identificador deste processo
    int proc_n;        // Numero de processos disparados pelo usuÃ¡rio na linha de comando (np)
    int message=0;       // Buffer para as mensagens
    int saco[TAREFAS];
	int i;

	srand(time(NULL));
	int r = rand();


	for(i=0;i < TAREFAS;i++){
		saco[i] = (rand() % TAREFAS) + 1;
	}

    MPI_Status status; /* Status de retorno */

    MPI_Init(&argc , & argv); // funcao que inicializa o MPI, todo o cÃ³digo paralelo esta abaixo

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);  // pega pega o numero do processo atual (rank)
    MPI_Comm_size(MPI_COMM_WORLD, &proc_n);  // pega informaÃ§Ã£o do numero de processos (quantidade total)
	int dones = 0;
	int toDo = TAREFAS;

    printf("[%s]@process %d starting...\n",curTime(),my_rank);
    if ( my_rank == 0 ) // qual o meu papel: sou o mestre ou um dos escravos?
    {
        // papel do mestre
		printf("[%s]@Wait for 5 seconds to start.\n",curTime());
		sleep(5);
		while(dones<toDo){
            printf("[%s]@waiting.tarefas=%d;dones=%d;toDo=%d\n",curTime(),TAREFAS,dones,toDo);
			MPI_Recv(&message, 8, MPI_INT,MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);  // recebo por ordem de chegada com any_source
			if(status.MPI_TAG == WORK_DONE){
                printf("[%s]@receiving %d from %d\n",curTime(),message,status.MPI_SOURCE);
				dones++;
			}else{
                printf("[%s]@receiving %s from %d\n",curTime(),printTag(GET_WORK),status.MPI_SOURCE);
            }

            int val = saco[dones];
			if(dones==TAREFAS){
                val=0;
                printf("[%s]@killing %d\n",curTime(),status.MPI_SOURCE);
				MPI_Send(&val, 8, MPI_INT,status.MPI_SOURCE, SUICIDE, MPI_COMM_WORLD);
			}else{
				printf("[%s]@sending %d to %d\n",curTime(),val,status.MPI_SOURCE);
				MPI_Send(&val, 8, MPI_INT,status.MPI_SOURCE, WORK, MPI_COMM_WORLD);
			}
		}
        printf("[%s]@master leaving...\n",curTime());
     }
     else
     {
         // papel do escravo
		//MPI_Send (message, strlen(message)+1, MPI_CHAR,dest, tag, MPI_COMM_WORLD);
		//message = (my_rank + 1) * 2;
		int tag = WORK;
		while(tag != SUICIDE){
            printf("[%s]@[%d]waiting\n",curTime(),my_rank);
			MPI_Send(&tag,  8, MPI_INT,0, GET_WORK, MPI_COMM_WORLD);    // retorno resultado para o mestre
			printf("[%s]@[%d]sended tag: %s\n",curTime(),my_rank,printTag(GET_WORK));
			MPI_Recv(&message, 8, MPI_INT,0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			printf("[%s]@[%d]receiving work %d from master with tag %s\n",curTime(),my_rank,message,printTag(status.MPI_TAG));
            tag = status.MPI_TAG;
			if(tag == WORK){
				message = message + 1;
				MPI_Send(&message,  8, MPI_INT,0, WORK_DONE, MPI_COMM_WORLD);
			}
		}
        printf("[%s]@slave[%d] leaving...\n",curTime(),my_rank);
     }

    MPI_Finalize();

}
