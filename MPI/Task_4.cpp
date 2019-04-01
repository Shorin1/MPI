#include "mpi.h"
#include <iostream>

using namespace std;

int main(int argc, char **argv) {
	MPI_Init(&argc, &argv);

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	char *sendArr = new char[30];
	char *recArr = new char[30];

	if (rank == 0) {
		double starttime = MPI_Wtime();
		cout << "Enter values in buffer: ";
		fflush(stdout);
		cin >> sendArr;
		cout << "Rank = " << rank << " size = " << size << endl;
		fflush(stdout);
		MPI_Send(sendArr, 30, MPI_CHAR,  rank + 1, rank + 1, MPI_COMM_WORLD);
		cout << "Sending data to loop. Sending time: " << MPI_Wtime() - starttime << endl;
		fflush(stdout);
		MPI_Recv(recArr, 30, MPI_CHAR, size - 1, rank, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
		cout << "Process nummber: " << rank << " receive data: '" << recArr << "' from process: " << size - 1
			<< ". Total running time: " << MPI_Wtime() - starttime << " sek" << endl;
		fflush(stdout);
	}
	else {
		double starttime = MPI_Wtime();

		MPI_Recv(recArr, 30, MPI_CHAR, rank - 1, rank, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

		cout << "Process nummber: " << rank << " receive data: '" << recArr << "' from process: " << rank - 1 << endl;
		fflush(stdout);
		if (rank == size - 1) {
			MPI_Send(recArr, 30, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
			cout << "Sending data to: 0 process from: " << rank << " process. Sending time: " << MPI_Wtime() - starttime << " sek" << endl;
			fflush(stdout);
		}
		else {
			MPI_Send(recArr, 30, MPI_CHAR, rank + 1, rank + 1, MPI_COMM_WORLD);
			cout << "Sending data to: " << rank + 1 << " process from: " << rank << " process. Sending time: " << MPI_Wtime() - starttime << " sek" << endl;
			fflush(stdout);
		}
	}

	MPI_Finalize();
}