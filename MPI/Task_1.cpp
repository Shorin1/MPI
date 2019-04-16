#include "mpi.h"
#include <iostream>

using namespace std;

int main1(int * argc, char ***argv)
{
	int rank, size, resultlen;
	char name[MPI_MAX_PROCESSOR_NAME];
	MPI_Init(argc, argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Get_processor_name(name, &resultlen);
	cout << "Hello world from rank = " << rank << " size = " << size << " name = " << name;
	MPI_Finalize();
	return 0;
}