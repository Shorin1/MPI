#include<mpi.h>
#include<fstream>
#include<iostream>

using namespace std;

void multMatrix(int** arrA, int** arrB, int n, int m, int l) {
	int result;

	for (int i = 0; i < l; i++) {
		for (int j = 0; j < n; j++) {

			result = 0;

			for (int z = 0; z < m; z++) {
				result += arrA[i][z] * arrB[z][j];
			}

			cout << result << " ";
		}
		cout << endl;
	}
}

void main8_2(int argc, char **argv) {
	int n = 0, m = 0, l = 0; // n - количство строк, m - длина строки, l - количество строк в матрице
	int rank, size;
	int **arrA = NULL;
	int **arrB = NULL;
	int countProcess;

	MPI_Status status;
	MPI_Comm comm_cart;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {
		fstream fa("a.txt");
		fa >> n >> m;

		arrA = new int* [n];

		for (int i = 0; i < n; i++) { // read matrix a
			arrA[i] = new int[m];

			for (int j = 0; j < m; j++) {
				fa >> arrA[i][j];
			}

		}

		fa.close();

		fstream fb("b.txt");
		arrB = new int* [m];

		for (int i = 0; i < m; i++) { // read matrix b
			arrB[i] = new int[n];

			for (int j = 0; j < n; j++) {
				fb >> arrB[i][j];
			}

		}

		fb.close();

		if (n <= size) {
			countProcess = n;
		}
		else {
			countProcess = size;
		}

		for (int i = 1; i < size; i++) {
			MPI_Send(&n, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			MPI_Send(&m, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			MPI_Send(&countProcess, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
		}
	}
	else {
		MPI_Recv(&n, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status); // Прием m
		MPI_Recv(&m, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status); // Прием m
		MPI_Recv(&countProcess, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status); // Прием m
	}



	if (rank == 0) {
		if (size == 1) {
			multMatrix(arrA, arrB, n, m, n);
		} 
		else if (n <= size) { // Если количество строк меньше чем процессов
			l = 1;

			for (int i = 1; i < n; i++) {
				MPI_Send(&l, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			}

			int result;

			multMatrix(arrA, arrB, n, m, 1);

			for (int i = 1; i < n; i++) {

				for (int j = 0; j < m; j++) {
					MPI_Send(&arrA[i][j], 1, MPI_INT, i, 1, MPI_COMM_WORLD);
				}

			}

		}
		else { // Если количество строк больше чем процессов
			
			l = n / (size - 1);

			for (int i = 1; i < size; i++) { // Отправка l, m всем матрицам
				MPI_Send(&l, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			}

			int c = n % (size - 1);
			int result;

			multMatrix(arrA, arrB, n, m, c);

			for (int i = 1; i < size; i++) {

				for (int z = 0; c < n && z < l; z++, c++) {

					for (int j = 0; j < m; j++) {
						MPI_Send(&arrA[c][j], 1, MPI_INT, i, 1, MPI_COMM_WORLD);
					}

				}

			}
		}

		if (size != 1) {

			for (int i = 0; i < m; i++) {

				for (int j = 0; j < n; j++) {
					MPI_Send(&arrB[i][j], 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
				}

			}

		}

	}
	else if (rank < countProcess) {

		MPI_Recv(&l, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

		arrA = new int* [l];

		for (int i = 0; i < l; i++) {
			arrA[i] = new int[m];

			for (int j = 0; j < m; j++) {
				MPI_Recv(&arrA[i][j], 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
			}
		}

		arrB = new int* [m];

		for (int i = 0; i < m; i++) {
			arrB[i] = new int[n];

			for (int j = 0; j < n; j++) {
				MPI_Recv(&arrB[i][j], 1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD, &status);
			}
		}

		multMatrix(arrA, arrB, n, m, l);

		if (rank < countProcess - 1) {

			for (int i = 0; i < m; i++) {

				for (int j = 0; j < n; j++) {
					MPI_Send(&arrB[i][j], 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
				}

			}

		}
	}


	MPI_Finalize();
}
