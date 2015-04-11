#include <stdio.h>
#include "mpi.h"
 
main(int argc, char** argv)
{
int my_rank;      // Identificador deste processo
int proc_n;         // Numero de processos disparados pelo usuário na linha de comando (np)
int message;     // Buffer para as mensagens           
int tag = 50; /* Tag para as mensagens */    
MPI_Status status; /* Status de retorno */    


MPI_Init(&argc , & argv); // funcao que inicializa o MPI, todo o código paralelo esta abaixo

//my_rank = MPI_Comm_rank();  // pega o numero do processo atual (rank)
MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
//proc_n    = MPI_Comm_size();  // pega informação do numero de processos (quantidade total)
MPI_Comm_size(MPI_COMM_WORLD, &proc_n);


// receber da esquerda
if ( my_rank == 0 ){    // sou o primeiro?
     message = 0;      // sim, sou o primeiro, crio a mensagem sem receber
	printf("proc_n:%d",proc_n);
}
else{
     //MPI_Recv(message, my_rank-1); // não sou o primeiro, recebo da esquerda
     MPI_Recv(&message, 8, MPI_INT, my_rank-1, tag, MPI_COMM_WORLD, &status);
	printf("my_rank:%d",my_rank);
}

// processo mensagem
message +=1; // incremento um na mensagem recebida

// enviar para a direita
if ( my_rank == proc_n-1 ) // sou o último?
     printf("Mensagem: %d", message); // sim sou o último, apenas mostro mensagem na tela
else
     //MPI_Send(message, my_rank+1); // não sou o último, envio mensagem para a direita
     MPI_Send(&message, 8, MPI_INT,my_rank+1, tag, MPI_COMM_WORLD);

MPI_Finalize();
}
