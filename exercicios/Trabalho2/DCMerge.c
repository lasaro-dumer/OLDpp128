#include "mpi.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>

#define ARRAY_SIZE 100000

void BubbleSort(int len, int * array){
	int c=0, d, temp, done =1;

	while (c < (len-1) & done )
	{
		done = 0;
		for (d = 0 ; d < len - c - 1; d++)
			if (array[d] > array[d+1])
			{
				temp      	= array[d];
				array[d]   	= array[d+1];
				array[d+1] 	= temp;
				done = 1;
			}
		c++;
	}
}

int *interleaving(int array[], int len){
	int *array_aux;
	int i1, i2, i_aux;

	array_aux = (int *)malloc(sizeof(int) * len);

	i1 = 0;
	i2 = len / 2;

	for (i_aux = 0; i_aux < len; i_aux++)
	{
		if (((array[i1] <= array[i2]) && (i1 < (len / 2)))
				|| (i2 == len))
			array_aux[i_aux] = array[i1++];
		else
			array_aux[i_aux] = array[i2++];
	}

	return array_aux;
}

const double curMilis(){
	struct timeval  tv;
	gettimeofday(&tv, NULL);

	return ((tv.tv_sec) * 1000 + (tv.tv_usec) / 1000.0) +0.5; // convert tv_sec & tv_usec to millisecond
}

main(int argc, char** argv){
	int my_rank;  /* Identificador do processo */
	int proc_n;   /* Número de processos */
	int source;   /* Identificador do proc.origem */
	int dest;     /* Identificador do proc. destino */
	int tam_vetor = 0;
    int *vetor_aux;         /* ponteiro para o vetor resultantes que sera alocado dentro da rotina */

    //time measure
    double t_before, t_after;

	int message[ARRAY_SIZE]; /* Buffer para as mensagens */
	MPI_Status status; /* Status de retorno */

	MPI_Init (&argc , & argv);

    t_before = MPI_Wtime();

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &proc_n);
    int delta = ARRAY_SIZE/((proc_n+1)/2);

	int vetor[ARRAY_SIZE];

    if ( my_rank != 0 ){
        MPI_Recv(&vetor[0],ARRAY_SIZE,MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &tam_vetor);

		printf("[%f]@slave[%d] Array len=%d\n",curMilis(),my_rank,tam_vetor);
    }
    else
    {
        tam_vetor = ARRAY_SIZE;
        // sou a raiz e portanto gero o vetor - ordem reversa
        int i;
        for (i=0 ; i<ARRAY_SIZE; i++)              /* init array with worst case for sorting */
            vetor[i] = ARRAY_SIZE-i;

		printf("[%f]@master[%d] Initial array len=%d\n",curMilis(),my_rank,tam_vetor);
    }

    //root
    if(tam_vetor <= delta)
    {
        //conquista (bubble sort)
        BubbleSort(tam_vetor, vetor);    // conquisto
        if (my_rank == 0)
        {
            t_after = MPI_Wtime();
			printf("[%f]@master[%d]:Time measured=%1.3fs ;Array len=%d\n",curMilis(),my_rank, t_after - t_before,tam_vetor);
        }
		printf("[%f]@slave[%d] Array len=%d\n",curMilis(),my_rank,tam_vetor);
    }
    else
    {
        //dividir e mandar para os filhos
        //2 * my_rank + 1 e 2 * my_rank + 2
        MPI_Send(&vetor[0], tam_vetor/2, MPI_INT, 2 * my_rank + 1, 1, MPI_COMM_WORLD);
        MPI_Send(&vetor[tam_vetor/2], tam_vetor/2, MPI_INT, 2 * my_rank + 2, 1, MPI_COMM_WORLD);

        //recebe dos filhos
        MPI_Recv(&vetor[0],tam_vetor,MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(&vetor[tam_vetor/2],tam_vetor,MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        // intercalo vetor inteiro

        vetor_aux = interleaving(vetor, tam_vetor);

        int i;
        for (i=0 ; i<tam_vetor; i++){
            vetor[i] = vetor_aux[i];
        }
        if (my_rank == 0){
            t_after = MPI_Wtime();
			printf("[%f]@master[%d]:Time measured=%1.3fs ;Array len=%d\n",curMilis(),my_rank, t_after - t_before,tam_vetor);
        }
    }

    // mando para o pai
    if ( my_rank !=0 ){
        MPI_Send(&vetor,tam_vetor,MPI_INT, my_rank/2 - 1 + my_rank%2, 1, MPI_COMM_WORLD);
		printf("[%f]@slave[%d] leaving...\n",curMilis(),my_rank);
    }else{
		printf("[%f]@master leaving...\n",curMilis());
		/*
		sleep(2);
		printf("\nVetor %d: ", my_rank);
		int i;
		for (i=0 ; i<tam_vetor; i++)
			printf("[%03d] ", vetor[i]);
		printf("\n");
		//*/
	}

    MPI_Finalize();
}
