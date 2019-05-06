#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <iostream>
#include <windows.h>

#define FILE_NAME "matrix.txt"
#define MAX_N 12
#define PROCESS_COUNT 4
#define ROOT_PROCESS 0

int main5(int argc, char **argv) {
	int rank, iterations_count;
	int first_row, last_row;
	MPI_Status status;
	double diffnorm, gdiffnorm;
	double xlocal[(MAX_N / 4) + 2][MAX_N];
	double xnew[(MAX_N / 3) + 2][MAX_N];
	double x[MAX_N][MAX_N];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == ROOT_PROCESS) {
		std::cout << "Current directory is ";
		system("echo %cd%");
		std::cout << std::endl;
		fflush(stdout);
	}

	if (rank == 0) {
		FILE *file_pointer;
		file_pointer = fopen(FILE_NAME, "r");

		for (int i = MAX_N - 1; i >= 0; i--) {

			for (int j = 0; j < MAX_N; j++) {
				fscanf(file_pointer, "%lf", &x[i][j]);
			}

			fscanf(file_pointer, "\n");
		}

		fclose(file_pointer);
	}

	MPI_Scatter(x[0], MAX_N * (MAX_N / PROCESS_COUNT), MPI_DOUBLE, xlocal[1], MAX_N * (MAX_N / PROCESS_COUNT), MPI_DOUBLE, 0, MPI_COMM_WORLD);

	first_row = 1;
	last_row = MAX_N / PROCESS_COUNT;

	if (rank == ROOT_PROCESS) {
		first_row++;
	}

	if (rank == PROCESS_COUNT - 1) {
		last_row--;
	}

	iterations_count = 0;
	do {

		if (rank < PROCESS_COUNT - 1) {
			MPI_Send(xlocal[MAX_N / PROCESS_COUNT], MAX_N, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
		}

		if (rank > 0) {
			MPI_Recv(xlocal[0], MAX_N, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &status);
			MPI_Send(xlocal[1], MAX_N, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD);
		}

		if (rank < PROCESS_COUNT - 1) {
			MPI_Recv(xlocal[MAX_N / PROCESS_COUNT + 1], MAX_N, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, &status);
		}

		iterations_count++;
		diffnorm = 0.0;

		for (int row = first_row; row <= last_row; row++) {

			for (int column = 1; column < MAX_N - 1; column++) {
				xnew[row][column] = (xlocal[row][column + 1] + xlocal[row][column - 1] + xlocal[row + 1][column] + xlocal[row - 1][column]) / 4.0;
				diffnorm += (xnew[row][column] - xlocal[row][column]) * (xnew[row][column] - xlocal[row][column]);
			}
		}

		for (int row = first_row; row <= last_row; row++) {

			for (int column = 1; column < MAX_N - 1; column++) {
				xlocal[row][column] = xnew[row][column];
			}
		}

		MPI_Allreduce(&diffnorm, &gdiffnorm, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

		gdiffnorm = sqrt(gdiffnorm);

		if (rank == ROOT_PROCESS) {
			printf("At iteration %d, diff is %f\n", iterations_count, gdiffnorm);
			fflush(stdout);
		}

	} while (gdiffnorm > 1.0E-2 && iterations_count < 100);


	MPI_Gather(xlocal[1], MAX_N * (MAX_N / PROCESS_COUNT), MPI_DOUBLE, x, MAX_N * (MAX_N / PROCESS_COUNT), MPI_DOUBLE, 0, MPI_COMM_WORLD);


	if (rank == ROOT_PROCESS) {

		printf("Final solution is\n");

		for (int i = MAX_N - 1; i >= 0; i--) {

			for (int j = 0; j < MAX_N; j++) {
				printf("%e ", x[i][j]);
				fflush(stdout);
			}

			printf("\n");
			fflush(stdout);
		}
	}

	MPI_Finalize();
	return 0;
}