#define _CRT_SECURE_NO_WARNINGS
#include "mpi.h"
#include <Windows.h>
#include <iostream>
#include <math.h>

using namespace std;

int main(int argc, char *argv[])
{
	int n, rank, size, i, resultlen; 
	double PI25DT = 3.141592653589793238462643; 
	double mypi, pi, h, sum, x, starttime; 
	char *name = new char[MPI_MAX_PORT_NAME];

	MPI_Init(&argc, &argv);
	
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(name, &resultlen);
	cout << "Process number " << rank << " started on computer '" << name << "'" << endl;
	MPI_Barrier(MPI_COMM_WORLD);
	fflush(stdout);

	do
	{
		starttime = MPI_Wtime();
		if (rank == 0) {
			cout << "Enter the number of intervals (0 for exit): ";
			fflush(stdout);
			cin >> n;
		}

		MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

		if (n != 0) {
			sum = 0.0;
			h = 2 * n;

			for (i = rank + 1; i <= n; i += size) {
				x = (2 * i - 1) / h;
				sum += (4.0 / (1.0 + x * x));
			}

			sum = 1.0 / n * sum;
			fflush(stdout);
			MPI_Reduce(&sum, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); 

			if (rank == 0) {
				cout << "Result: " << pi << " Error: " << fabs(pi - PI25DT) << " Running time: " << MPI_Wtime() - starttime << endl;
				fflush(stdout);
			}
		}
	}while (n != 0);

	MPI_Finalize(); 
	return 0;
}