#include<mpi.h>
#include<stdio.h>
#include<iostream>
#define NUM_DIMS 1

using namespace std;

int main7(int argc, char **argv)
{
	int rank, size, i, A, B, dims[NUM_DIMS];
	int periods[NUM_DIMS], new_coords[NUM_DIMS];
	int sourceb, destb, sourcem, destm;
	int reorder = 0;
	MPI_Comm comm_cart;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	/* �������� ������ dims � ��������� ������ periods ��� ��������� "�������" */
	dims[0] = 0; 
	periods[0] = 0; 
	/* ��������� ������ dims, ��� ����������� ������� (����������) ������� */
	MPI_Dims_create(size, NUM_DIMS, dims);
	/* ������� ��������� "�������" � communicator-�� comm_cart */
	MPI_Cart_create(MPI_COMM_WORLD, NUM_DIMS, dims, periods, reorder, &comm_cart);
	/* ���������� ����� � ���������� � ������� �� */
	MPI_Cart_coords(comm_cart, rank, NUM_DIMS, new_coords);
	A = rank;
	B = -1;
	/* ������ ����� ������� ����� ������� ����� �������, � ����������� ������� �������� ������� �����������
	� � ����������� ������� �������� �������. ����� � ������� new_coords[0]==0 �� ����� �������
	� ������� �������, ������� � ����� ����������� ��� ����� ��������� ������ �� ��������������
	������, �.�. �� ������ sourcem=MPI_PROC_NULL, �, ��������������, �������� ������ � ���� �����������
	������ destm=MPI_PROC_NULL. ���������� ������������ ��������� ��� ������ � ������� new_coords[0]==dims[0]-1. */
	if (rank == 0) // ���� 0 �������
	{
		sourcem = destm = -1;
	}
	else
	{
		sourcem = destm = rank - 1;
	}

	if (rank == size - 1)
	{
		destb = sourceb = -1;
	}
	else
	{
		destb = sourceb = rank + 1;
	}
	/* ������ ����� �������� ���� ������ (�������� ���������� �) ����� �������� ����� � �������
	������� � ��������� ������ � B �� �������� ����� � ������� �������. ���� ����� � �����, ��������
	� B ��������� �� ������. */
	MPI_Sendrecv(&A, 1, MPI_INT, destb, 12, &B, 1, MPI_INT, sourcem, 12, comm_cart, &status);
	printf("new_coords[0]=%d B=%d\n", new_coords[0], B);
	/* ����� ������ � ��������������� ������� � ����� ��������������� ������ */
	MPI_Sendrecv(&A, 1, MPI_INT, destm, 12, &B, 1, MPI_INT, sourceb, 12, comm_cart, &status);
	printf("new_coords[0]=%d B=%d\n", new_coords[0], B);
	/* ��� ����� ��������� ��������� ��������, ��������� � ��������� comm_cart � ��������� ��������� */
	MPI_Comm_free(&comm_cart);
	MPI_Finalize();
	return 0;
}
