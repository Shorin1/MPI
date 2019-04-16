#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <iostream>
#include <windows.h>

#define FILE_NAME "matrix.txt"
#define N 12
#define PROCESS_COUNT 4
#define ROOT_PROCESS 0

using namespace std;

int main(int argc, char **argv) {
	int rank, iterations_count;
	int first_row, last_row;
	MPI_Status status;
	double diffnorm, gdiffnorm;
	double matrix_local[(N / 4) + 2][N];
	double new_matrix[(N / 3) + 2][N];
	double matrix[N][N];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == ROOT_PROCESS) { //Считываем файл
		FILE *file;
		file = fopen(FILE_NAME, "r");
		for (int i = N - 1; i >= 0; i--) {
			for (int j = 0; j < N; j++) {
				fscanf(file, "%lf", &matrix[i][j]);
			}
		}
		fclose(file);
	}

	MPI_Scatter(matrix[0], N * (N / PROCESS_COUNT), MPI_DOUBLE, matrix_local[1], N * (N / PROCESS_COUNT), MPI_DOUBLE, 0, MPI_COMM_WORLD);

	first_row = 1;
	last_row = N / PROCESS_COUNT;

	if (rank == ROOT_PROCESS) {
		first_row++;
	}

	if (rank == PROCESS_COUNT - 1) {
		last_row--;
	}

	iterations_count = 0;
	do {

		if (rank < PROCESS_COUNT - 1) {
			MPI_Send(matrix_local[N / PROCESS_COUNT], N, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
		}

		if (rank > 0)
		{
			MPI_Recv(matrix_local[0], N, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &status);
			MPI_Send(matrix_local[1], N, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD);
		}

		if (rank < PROCESS_COUNT - 1) {
			MPI_Recv(matrix_local[N / PROCESS_COUNT + 1], N, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, &status);
		}

		iterations_count++;
		diffnorm = 0.0;
		for (int i = first_row; i <= last_row; i++) { // Сложение матриц и запись результат в новую матрицу
			for (int j = 1; j < N - 1; j++) {
				new_matrix[i][j] = (matrix_local[i][j + 1] + matrix_local[i][j - 1] + matrix_local[i + 1][j] + matrix_local[i - 1][j]) / 4.0;
				diffnorm += (new_matrix[i][j] - matrix_local[i][j]) * (new_matrix[i][j] - matrix_local[i][j]);
			}
			matrix_local[i][j] = new_matrix[i][j];
		}
	}
}

for (int i = first_row; i <= last_row; i++) { // Перезапись матриц из новой матрицы в локальрной
	for (int j = 1; j < N - 1; j++) {
		gdiffn
			gdiffnorm = sqrt(gdiffnorm);

		if (rank == ROOT_PROCESS) { // Вывод результата
			cout << "At iteration " << iterations_count << ", diff is " << gdiffnorm << endl;
		}

	} while (gdiffnorm > 1.0E-2 && iterations_count < 100);


	MPI_Gather(matrix_local[1], N * (N / PROCESS_COUNT), MPI_DOUBLE, matrix, N * (N / PROCESS_COUNT), MPI_DOUBLE, 0, MPI_COMM_WORLD); // Собирает единичные сообщения


	if (rank == ROOT_PROCESS) { // Вывод финального результата
		cout << "Final solution is" << endl;
		for (int i = N - 1; i >= 0; i--) {
			for (int j = 0; j < N; j++) {
				cout << matrix[i][j] << " ";
			}
			cout << endl;
		}
	}

	MPI_Finalize();
	return 0;
}
