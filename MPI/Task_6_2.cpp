#include<mpi.h>
#include<stdio.h>
#define NUM_DIMS 1

int main(int argc, char **argv)
{
	int rank, size, i, A, dims[NUM_DIMS];
	int periods[NUM_DIMS], source, dest;
	int reorder = 0;
	MPI_Comm comm_cart;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	{
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		MPI_Comm_size(MPI_COMM_WORLD, &size);

		/* �������� ������ dims � ��������� ������ periods ��� ��������� "������" */
		for (i = 0; i < NUM_DIMS; i++) {
			dims[i] = 0;
			periods[i] = 1;
		}
		/* ��������� ������ dims, � ������� ����������� ������� ������� */
		MPI_Dims_create(size, NUM_DIMS, dims);
		/* ������� ��������� "������" � communicator-�� comm_cart */
		MPI_Cart_create(MPI_COMM_WORLD, NUM_DIMS, dims, periods, reorder, &comm_cart);
		/* ������ ����� ������� ����� ������� ����� ������, � ����������� ������� �������� ������ */
		MPI_Cart_shift(comm_cart, 0, 1, &source, &dest);
		/* ������ ����� �������� ���� ������ (�������� ���������� A) �������� �����
		� ������� ������ � ��������� ������ � B �� �������� ����� � ������� ������.
		����� � ������ size-1 �������� ���� ������ ����� � ������ 0, � ����� 0 ���������
		������ �� ����� size-1. */

		if (rank == 0) {

			int sendto = rank + 1;
			int recivefrom = size - 1; //last process
			A = 5;
			MPI_Send(&A, 1, MPI_INT, sendto, 0, comm_cart);
			MPI_Recv(&A, 1, MPI_INT, recivefrom, 0, comm_cart, MPI_STATUS_IGNORE);

		}
		else {

			int recivefrom = rank - 1;
			int sendto;
			if (rank == size - 1) sendto = 0;
			else sendto = rank + 1;

			MPI_Recv(&A, 1, MPI_INT, recivefrom, 0, comm_cart, MPI_STATUS_IGNORE);
			MPI_Send(&A, 1, MPI_INT, sendto, 0, comm_cart);

		}

		printf("rank=%d A=%d\n", rank, A);
		fflush(stdout);
		/* ������ ����� �������� ���� ���� (�� �� � ��� ������ �������� ����� � ������� ������ � ��������
		���������� B (���� �������� ����� � ������� ������). */
		/* ��� ����� ��������� ��������� ��������, ��������� � ���������� comm_cart � ���������
		���������� ��������� */
		MPI_Comm_free(&comm_cart);
	}
	MPI_Finalize();
	return 0;
}
