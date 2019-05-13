#include<stdio.h>
#include<mpi.h>
#include<string>
#include<fstream>
#include<iostream>

using namespace std;

int main8(int argc, char **argv)
{
	int dims[1], periods[1], new_coords[1];
	int source, dest;
	int rank, size;
	int n = 4, m = 6;
	MPI_Comm comm_cart;
	MPI_Status status;


	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int *(*matrixA) = new int*[n];
	int *(*matrixB) = new int*[n];

	int *vectorA = (int*)malloc(m * sizeof(int));
	int *vectorB = (int*)malloc(m * sizeof(int));

	if (rank == 0) { 
		fstream streamA("a.txt");

		for (int i = 0; i < n; i++) {
			matrixA[i] = new int[m];
			for (int j = 0; j < m; j++) {
				streamA >> matrixA[i][j];

			}
		}

		streamA.close();

		fstream streamB("b.txt");

		if (streamB.is_open()) {
			for (int i = 0; i < n * m; i++) {
				streamB >> vectorB[i];
			}
		}

		streamB.close();
	}

	if (rank == 0){ //Отправка второй матрицы и приобразование его в двухмерный массив

		for (int i = 1; i < size; i++) {
			MPI_Send(vectorB, n * m, MPI_INT, i, 1, MPI_COMM_WORLD);
		}

	}
	else {
		MPI_Recv(vectorB, n * m, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

		for (int i = 0, z = 0; i < m; i++) {
				matrixB[i] = new int[n];
			for (int j = 0; j < n; j++) {
				matrixB[i][j] = vectorB[z++];
			}
		}
	}

	int sendTo = 1;

	for (int i = 0; i < n; i++) { //Умножение матриц

		if (rank == 0) {

			if (sendTo == size) {
				sendTo = 1;
			}

			MPI_Send(matrixA[i], m, MPI_INT, sendTo++, 2, MPI_COMM_WORLD);
		}
		else {
			MPI_Recv(vectorA, m, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
			cout << "rank: " << rank << endl;
			for (int i = 0, z = 0; i < n; i++) {
				int result = 0;
				for (int j = 0; j < m; j++) {
					result += vectorA[j] * matrixB[j][i];
				}
				cout << result << " ";
			}
			cout << endl;
		}
	}

	cout << "Finalize" << endl;
	MPI_Finalize();		

	return 0;
}