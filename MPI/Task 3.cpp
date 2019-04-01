#define _CRT_SECURE_NO_WARNINGS
#include "mpi.h"
#include <Windows.h>
#include <iostream>
#include <math.h>

int main(int argc, char *argv[])
{
	int n, myid, numprocs, i; 
	double PI25DT = 3.141592653589793238462643; 
	double mypi, pi, h, sum, x, starttime; 

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	while (1)
	{
		starttime = MPI_Wtime();
		if (myid == 0) {
			printf("Enter the number of intervals: (0 quits) ");
			fflush(stdout);
			scanf("%d", &n);
		}

		MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

		if (n != 0) {
			h = 1.0 / (double)n; 
			sum = 0.0;

			for (i = myid + 1; i <= n; i += numprocs) {
				x = h * ((double)i - 0.5);
				sum += (4.0 / (1.0 + x * x));
			}

			mypi = h * sum; 
			MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); 

			if (myid == 0) {
				printf("Result: %.16f. Error: %.16f. Work time: %f\n", pi, fabs(pi - PI25DT), MPI_Wtime() - starttime);
				fflush(stdout);
			}
		}
	}
	MPI_Finalize(); 
	return 0;
}