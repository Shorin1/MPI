#include<stdio.h>
#include<mpi.h>
#include<string>
#include<fstream>
#include<iostream>

#define NUM_DIMS 1

using namespace std;

int main(int argc, char **argv) {
	int rank, size;
	int n = 4, m = 6, l = -1;
	MPI_Status status;

	int **matrixA = new int*[4];
	int **matrixB = new int*[6];
	int *vectorA = new int[6*4];
	int *vectorB = new int[6*4];

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {
		fstream streamA("A.txt");
		streamA >> n >> m;

		for (int i = 1; i < size; i++) {
			MPI_Send(&n, 1, MPI_INT, i, 1, MPI_COMM_WORLD); // отправляем длину строк
			cout << "0 process send n to " << i << endl;
			MPI_Send(&m, 1, MPI_INT, i, 1, MPI_COMM_WORLD); // отправляем длину строк
			cout << "0 process send m to " << i << endl;
		}

		for (int i = 0; i < n; i++) {
			matrixA[i] = new int[m];

			for (int j = 0; j < m; j++) {
				streamA >> matrixA[i][j];
			}

		}

		streamA.close();

		fstream streamB("B.txt");

		if (streamB.is_open()) {

			for (int i = 0; i < n * m; i++) {
				streamB >> vectorB[i];
			}

		}

		streamB.close();

		if (n <= size - 1) {
			l = 1;

			for (int i = 1; i <= n; i++) {
				MPI_Send(&l, 1, MPI_INT, i, 1, MPI_COMM_WORLD); //рассылка количества строк
				cout << "0 process send l to " << i << endl;
				MPI_Send(matrixA[i], m, MPI_INT, i, 1, MPI_COMM_WORLD); // рассылаем самих строки
				cout << "0 process send matrix to " << i << endl;
			}

		}
		else {
			l = n / (size - 2);

			for (int i = 1; i < size - 1; i++) {
				MPI_Send(&l, 1, MPI_INT, i, 1, MPI_COMM_WORLD); //рассылка количества строк
				cout << "0 process send l to " << i << endl;

				for (int j = 0; j < l; j++) {
					MPI_Send(matrixA[j], m, MPI_INT, i, 1, MPI_COMM_WORLD); // рассылаем самих строки
				}

				cout << "0 process send matrix to " << i << endl;
			}

			l = n % (size - 2);
			MPI_Send(&l, 1, MPI_INT, size - 1, 1, MPI_COMM_WORLD); // отправляем количество строк последнему процессу
			cout << "0 process send l to " << size - 1 << endl;

			for (int j = 0; j < l; j++) {
				MPI_Send(matrixA[j], m, MPI_INT, size - 1, 1, MPI_COMM_WORLD); // рассылаем самих строки
			}

			cout << "0 process send matrix to " << size - 1 << endl;
		}

		MPI_Send(vectorB, 6 * 4, MPI_INT, 1, 1, MPI_COMM_WORLD);
		cout << "Send vectorB from " << rank << " process to " << 1 << endl;

	}
	else { //Если процесс не нулевой
		MPI_Recv(&n, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status); // получаем длину строки
		cout << rank << " process recv n from 0 process" << endl;
		MPI_Recv(&m, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status); // получаем длину строки
		cout << rank << " process recv m from 0 process" << endl;
		MPI_Recv(&l, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status); // получаем количество строк
		cout << rank << " process recv l from 0 process" << endl;

		for (int i = 0; i < l; i++) {
			matrixA[i] = new int[m];
			MPI_Recv(matrixA[i], m, MPI_INT, 0, 1, MPI_COMM_WORLD, &status); // получаем строки
		}

		cout << rank << " process recv matrix from 0 process" << endl;
		
		MPI_Recv(vectorB, 6 * 4, MPI_INT, rank - 1, 1, MPI_COMM_WORLD, &status);
		cout << rank << " recv vectorB from " << rank - 1 << endl;
		

		for (int i = 0, z = 0; i < m; i++) {
			matrixB[i] = new int[n];

			for (int j = 0; j < n; j++) {
				matrixB[i][j] = vectorB[z++];
			}

		}

		for (int z = 0; z < l; z++) {

			for (int i = 0, z = 0; i < n; i++) {
				int result = 0;

				for (int j = 0; j < m; j++) {
					result += matrixA[l][j] * matrixB[j][i];
				}

				cout << result << " ";
			}

			cout << endl;
		}

		if (rank != size - 1) {
			cout << rank << " send vectorB to" << rank + 1 << endl;
			MPI_Send(vectorB, n*m, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
		}
	}

	//MPI_Barrier(MPI_COMM_WORLD);

	//if (rank == 0) {
	//	
	//}
	//else {
	//	cout << "Recv vectorB from " << rank - 1;
	//	MPI_Recv(vectorB, n * m, MPI_INT, rank - 1, 5, MPI_COMM_WORLD, &status);

	//	for (int i = 0, z = 0; i < m; i++) {
	//		matrixB[i] = new int[n];

	//		for (int j = 0; j < n; j++) {
	//			matrixB[i][j] = vectorB[z++];
	//		}

	//	}

	//	for (int z = 0; z < l; z++) {

	//		for (int i = 0, z = 0; i < n; i++) {
	//			int result = 0;

	//			for (int j = 0; j < m; j++) {
	//				result += matrixA[l][j] * matrixB[j][i];
	//			}

	//			cout << result << " ";
	//		}

	//		cout << endl;
	//	}

	//	if (rank != size) {
	//		MPI_Send(vectorB, n*m, MPI_INT, rank + 1, 5, MPI_COMM_WORLD);
	//	}
	//}

	return 0;
}