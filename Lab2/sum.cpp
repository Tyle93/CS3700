#include "mpi.h"
#include <cstdio>
#include <cmath>
#include <cstdlib>

#define ARRAY_SIZE 1000000

int main (int argc,  char *argv[]) {

   int myid, numprocs;
   int namelen;
   int* numbers = new int[ARRAY_SIZE];
   char processor_name[MPI_MAX_PROCESSOR_NAME];

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
   MPI_Comm_rank(MPI_COMM_WORLD, &myid);
   MPI_Get_processor_name(processor_name, &namelen);
 
   printf("Process %d on %s\n", myid, processor_name);
 
   for (int i=0; i<ARRAY_SIZE; i++)
      numbers[i] = i;  //could be randomly generated

   int s = (int)floor(ARRAY_SIZE/numprocs);
   int s0 = s + ARRAY_SIZE%numprocs;

   int startIndex = s0 + (myid-1)*s;
   int endIndex = startIndex + s;

   double startwtime;
   if (myid == 0) {
      startwtime = MPI_Wtime();
   }

   int i;
   int part_sum = 0;
   
   if (myid == 0) {
      // master worker - comput the master's numbers
      for (i=0; i<s0; i++) {
         part_sum += numbers[i];
      }
      printf("Process %d - startIndex 0 endIndex %d; part_sum %ld\n",
             myid, s0-1, part_sum);
   } else {
      //slave's work
      for (i= startIndex; i<endIndex; i++) {
         part_sum += numbers[i];
      }
      printf ("Process %d - startIndex %d endIndex %d; part_sum %ld\n",
              myid, startIndex, endIndex-1, part_sum);
   }

   int sum = 0;
   MPI_Reduce(&part_sum, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

   if (myid == 0) {
      double runTime = MPI_Wtime() - startwtime;
      printf("Execution time (sec) = %f sum = %ld \n",
             runTime, sum);
   }

   delete[] numbers;

   MPI_Finalize();
}