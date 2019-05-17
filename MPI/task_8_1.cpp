#include<stdio.h>
#include<mpi.h>
#include<string>
#include<fstream>
#include<iostream>
#include<time.h>

using namespace std;

#define NUM_DIMS 1
const int N1 = 100, M1 = 100;
static double A[N1][M1], B[N1][M1], C[N1][N1];
int main8_1(int argc, char **argv)
{
	int rank, size, i, j, k, i1, j1, d, sour, dest;
	int N, M;
	int dims[NUM_DIMS], periods[NUM_DIMS], new_coords[NUM_DIMS];
	int reorder = 0;
	MPI_Comm comm_cart;
	MPI_Status st;
	double time1, time2;
	double *Avector, *Bvector, *Cvector;
	int dt1;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (rank == 0)
	{
		/*������ ������� A �� ����� A.txt, �������������� � ����� � ����������� ������ �������� ����������
		N � M ����������� ��� ��*/
		fstream fa("a.txt");
		fa >> N >> M;
		/* �������� N � M ���� ���������� */
		for (i = 1; i < size; i++)
		{
			MPI_Send(&N, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			MPI_Send(&M, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
		}
		for (i = 0; i < N; i++)
		{
			for (j = 0; j < M; j++)
			{
				fa >> A[i][j];
			}
		}
		fa.close();
		/*������� B ������ �� ����� B.txt, �������������� ��� ��*/
		fstream fb("b.txt");
		for (i = 0; i < M; i++)
		{
			for (j = 0; j < N; j++)
			{
				fb >> B[j][i];//������� B ������������� ��� ��������
			}
		}
		fb.close();
	}
	if (rank != 0)
	{
		//������ ������� �������� �� �������� N � M
		MPI_Recv(&N, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &st);
		MPI_Recv(&M, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &st);
	}
	if (N%size != 0)
	{
		/*���� ������ ������� A ������ ������� �������� �� ��������� � ������, ������� ������� ��������� �� ������*/
		if (rank == 0)
		{
			printf("������! �����: %d, �����������: %d.\n�����, ����� ������ ������� A � ������� ������� B ������� �������� �� �����������!!!\n", N, size);
			fflush(stdout);
		}
		//��� �������� ����, ���� ������� ������� ��������� �� ������
		MPI_Barrier(MPI_COMM_WORLD);
		//��� ������ �������� ��������� ������
		MPI_Abort(MPI_COMM_WORLD, MPI_ERR_OTHER);
		return -1;
	}
	N = N / size;
	if (rank == 0)
	{
		Avector = new double[N*M];
		Bvector = new double[N*M];
		/* � ����� ������� ������� �������� ���� ��������� ��������� �� ������ ������ A � B
		��� ����� ����������� ������� � �������, ����� ������� ���� ����������*/
		for (i = 1; i < size; i++)
		{
			for (i1 = 0; i1 < N; i1++)
			{
				for (j1 = 0; j1 < M; j1++)
				{
					Avector[i1*M + j1] = A[N*i + i1][j1];
				}
			}
			for (i1 = 0; i1 < N; i1++)
			{
				for (j1 = 0; j1 < M; j1++)
				{
					Bvector[i1*M + j1] = B[i*N + i1][j1];
				}
			}
			MPI_Send(Avector, N*M, MPI_DOUBLE, i, 3, MPI_COMM_WORLD);
			MPI_Send(Bvector, N*M, MPI_DOUBLE, i, 4, MPI_COMM_WORLD);
		}
		/* ���� ������ ������� B ������� ������� ���� ����������� � ������ */
		for (i1 = 0; i1 < N; i1++)
		{
			for (j1 = 0; j1 < M; j1++)
			{
				Bvector[i1*M + j1] = B[i1][j1];
			}
		}

	}
	else
	{
		Avector = new double[N*M];
		Bvector = new double[N*M];
		// ��� ��������� �������� �������� �� �������� ������� A � B
		MPI_Recv(Avector, N*M, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD, &st);
		MPI_Recv(Bvector, N*M, MPI_DOUBLE, 0, 4, MPI_COMM_WORLD, &st);
		// � ����������� ������� � �� ������� � �������
		for (i = 0; i < N; i++)
		{
			for (j = 0; j < M; j++)
			{
				A[i][j] = Avector[i*M + j];
			}
		}
	}
	//������ ������� ������� ���� ������ ��� ��������
	printf("Process %d:\n", rank);
	fflush(stdout);
	printf("Matrix A:\n");
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < M; j++)
		{
			printf("%f ", A[i][j]);
		}
		printf("\n");
	}
	printf("Matrix B:\n");
	for (i = 0; i < M; i++)
	{
		for (j = 0; j < N; j++)
		{
			printf("%f ", Bvector[j*M + i]);
		}
		printf("\n");
	}
	fflush(stdout);
	/* �������� ������ dims � ��������� ������ periods ��� ��������� "������" */
	for (i = 0; i < NUM_DIMS; i++) { dims[i] = 0; periods[i] = 1; }
	/* ��������� ������ dims, ��� ����������� ������� (����������) ������� */
	MPI_Dims_create(size, NUM_DIMS, dims);
	/* ������� ��������� "������" � communicator(��) comm_cart */
	MPI_Cart_create(MPI_COMM_WORLD, NUM_DIMS, dims, periods, reorder, &comm_cart);
	/* ���������� ����� �� ���������� �����������, � ����� ����������� ����������� ��������
	   ����������� ��������� �� ���������� ��������� �������. */
	MPI_Cart_coords(comm_cart, rank, NUM_DIMS, new_coords);
	/* ������ ����� ������� ����� ������� ����� ������, � ����������� ������� �������� ������ */
	MPI_Cart_shift(comm_cart, 0, -1, &sour, &dest);
	/* �������� ������ ��������� ������ */
	time1 = MPI_Wtime();
	/*������ ����� ���������� ��������� ����� ����� ������ */
	/* ����� ������� ���� for(k) - ���� �� ����������� */
	for (k = 0; k < size; k++)
	{
		//������ ������� B �� ������ Bvector
		for (i = 0; i < N; i++)
		{
			for (j = 0; j < M; j++)
			{
				B[i][j] = Bvector[i*M + j];
			}
		}
		/*������ ����� ��������� ����������(����� ������) ��� �������������� ��������� ������� C,
		������� ������� �� ������ ����� k  � ����� ����������. */
		d = ((rank + k) % size)*N;
		/*������ ����� ���������� ��������� ����� ������ ������� A �� ������� ������ ������� B */
		for (i = 0; i < N; i++)
		{
			for (j = 0; j < N; j++)
			{
				for (i1 = 0; i1 < M; i1++)
				{
					C[i][j + d] += A[i][i1] * B[j][i1];
				}
			}
		}
		/* ��������� ������ ����� ������� A �� ������ �������� ������� B � ������ ����� ��������� */
		/* ������ ����� �������� ����� �������� ������ � ������� ������ ������������ ������ ������� B. �.�.
		������ ������� B ���������� ����� ������ ����������� */
		MPI_Sendrecv_replace(Bvector, N*M, MPI_DOUBLE, dest, 12, sour, 12, comm_cart, &st);
	}
	/* ��������� ���������. ������ ����� �������� ���� ������ ����� ������� A  �� ��� ������
	�������� ������� B. �������� ����� � ������� ��������� */
	time2 = MPI_Wtime();
	dt1 = (int)(time2 - time1);
	printf("rank=%d Time=%d\n", rank, dt1);
	/* ��� ����� ��������� ��������� ��������, ��������� � ���������� comm_cart */
	MPI_Comm_free(&comm_cart);
	/* �������� ������� C � �������� � ������ 0	*/
	if (rank != 0)
	{
		Cvector = new double[N*N*size];
		for (i = 0; i < N; i++)
		{
			for (j = 0; j < N*size; j++)
			{
				Cvector[i*N*size + j] = C[i][j];
			}
		}
		MPI_Send(Cvector, N*N*size, MPI_DOUBLE, 0, 5, MPI_COMM_WORLD);

	}
	else
	{
		Cvector = new double[N*N*size];
		for (k = 1; k < size; k++)
		{
			MPI_Recv(Cvector, N*N*size, MPI_DOUBLE, k, 5, MPI_COMM_WORLD, &st);
			for (i = 0; i < N; i++)
			{
				for (j = 0; j < N*size; j++)
				{
					C[i + N * k][j] = Cvector[i*N*size + j];
				}
			}
		}
		// ������� �������: ������� ���������
		printf("Matrix C:\n");
		for (i = 0; i < N*size; i++)
		{
			for (j = 0; j < N*size; j++)
			{
				printf("%f ", C[i][j]);
			}
			printf("\n");
		}
	}
	delete[]Avector;
	delete[]Bvector;
	delete[]Cvector;
	MPI_Finalize();
	return 0;
}

