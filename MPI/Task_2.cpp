#define _CRT_SECURE_NO_WARNINGS
#include "mpi.h"
#include <Windows.h>
#include <iostream>

using namespace std;

int main2(int * argc, char ***argv){
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	int rank, resultlen;
	int* buffer = (int*) malloc(sizeof(int));
	char name[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(argc, argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(name, &resultlen);
	 do {
		 
		if (rank == 0) {
			printf("Enter number: ");
			fflush(stdout);
			scanf("%d", buffer);
		}

		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Bcast(buffer, 1, MPI_INT, 0, MPI_COMM_WORLD);
		printf("Process: %d; buffer: %d\n", rank, *buffer);
		fflush(stdout);
		MPI_Barrier(MPI_COMM_WORLD);
	 } while (*buffer > 0);

	MPI_Finalize();
	return 0;
}