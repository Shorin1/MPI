#include<mpi.h>
#include<stdio.h>
#define NUM_DIMS 1

int main(int argc, char **argv)
{
	int rank, size, i, A, B, dims[NUM_DIMS];
	int periods[NUM_DIMS], source, dest;
	int reorder = 0;
	MPI_Comm comm_cart;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	A = rank;	
	B = -1;
	/* �������� ������ dims � ��������� ������ periods ��� ��������� "������" */
	for (i = 0; i < NUM_DIMS; i++) {
		dims[i] = 0; periods[i] = 1; 
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
	MPI_Sendrecv(&A, 1, MPI_INT, dest, 12, &B, 1, MPI_INT, source, 12, comm_cart, &status);
	/* ������ ����� �������� ���� ���� (�� �� � ��� ������ �������� ����� � ������� ������ � ��������
	���������� B (���� �������� ����� � ������� ������). */
	printf("rank=%d B=%d\n", rank, B);
	/* ��� ����� ��������� ��������� ��������, ��������� � ���������� comm_cart � ���������
	���������� ��������� */
	MPI_Comm_free(&comm_cart);
	MPI_Finalize();
	return 0;
}
