#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>
#define NUM_ARRAYS 100000
#define ARRAYS_SIZE 10

int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}

void printArray(int * a){
    int i;
    for(i=0;i<ARRAYS_SIZE;i++){
        printf("a[%d]=%d\n",i,a[i]);
    }
}


main(int argc, char** argv)
{

    int i,j;
    //*
    //int saco[NUM_ARRAYS][ARRAYS_SIZE];
    int **saco;
	saco = malloc(NUM_ARRAYS * sizeof(int *));
	if(saco == NULL)
	{
		printf("out of memory\n");
	}else{
        printf("lines created\n");
        printf("array size will be %lu\n",ARRAYS_SIZE * sizeof(int));
        for(i = 0; i < NUM_ARRAYS; i++)
        {
            saco[i] = malloc(ARRAYS_SIZE * sizeof(int));
            if(saco[i] == NULL)
            {
                printf("out of memory row %d\n",i);
            }else{
                for(j = 0; j< ARRAYS_SIZE; j++)
                {
                    saco[i][j] = ARRAYS_SIZE - j;
                }
            }
        }
    }
    //*/

    printf("All created.last item saco");
    printf("[%d][%d]=%d\n",i-1,j-1,saco[i-1][j-1]);

    printArray(saco[0]);
    qsort (saco[0], ARRAYS_SIZE, sizeof(int), compare);
    printArray(saco[0]);

    return 0;
}
