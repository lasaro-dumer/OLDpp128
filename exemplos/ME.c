#include <stdio.h>
#include "mpi.h"
#define TAREFAS 7 // Numero de tarefas no saco de trabalho para np = 8, processo 0 Ã© o mestre

main(int argc, char** argv)
{
    int my_rank;       // Identificador deste processo
    int proc_n;        // Numero de processos disparados pelo usuÃ¡rio na linha de comando (np)
    int message;       // Buffer para as mensagens
    int saco[TAREFAS]; // saco de trabalho
    MPI_Status status; /* Status de retorno */


    MPI_Init(&argc , & argv); // funcao que inicializa o MPI, todo o cÃ³digo paralelo esta abaixo

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);  // pega pega o numero do processo atual (rank)
    MPI_Comm_size(MPI_COMM_WORLD, &proc_n);  // pega informaÃ§Ã£o do numero de processos (quantidade total)

    if ( my_rank == 0 ) // qual o meu papel: sou o mestre ou um dos escravos?
    {
        // papel do mestre
        int i=0;
        for ( i=0 ; i < TAREFAS ; i++) // mando o trabalho para os escravos fazerem
        {
           message = saco[i];
           printf("sending message(%d) to  %d",message,i+1);
           MPI_Send(&message, 8, MPI_INT,i+1, i, MPI_COMM_WORLD); // envio trabalho saco[i] para escravo com id = i+1;
        }

        // recebo o resultado

        for ( i=0 ; i < TAREFAS ; i++)
        {
            // recebo mensagens de qualquer emissor e com qualquer etiqueta (TAG)

            MPI_Recv(&message, 8, MPI_INT,MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);  // recebo por ordem de chegada com any_source
            printf("receiving from %d",status.MPI_SOURCE-1);
            saco[status.MPI_SOURCE-1] = message;   // coloco mensagem no saco na posiÃ§Ã£o do escravo emissor
        }
     }
     else
     {
         // papel do escravo

         MPI_Recv(&message, 8, MPI_INT,0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);    // recebo do mestre
         printf("[%d]receiving work: %d",my_rank,message);
         message = message+1;      // icremento conteÃºdo da mensagem
         printf("[%d]sending work: %d",my_rank,message);
         MPI_Send(&message,  8, MPI_INT,0, my_rank-1, MPI_COMM_WORLD);    // retorno resultado para o mestre
     }

    MPI_Finalize();

}
