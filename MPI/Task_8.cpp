#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include <stdlib.h>
#include <stdarg.h>

int rank, rankc, i, j, k, m, n, l, u;

#define matr_read(f, A, m, n) \
{ \
  for(i = 0; i < m; i++) { \
  for(j = 0; j < n; j++)   { \
      fscanf(f, "%lf", &A[j+i*n]); \
    } \
  } \
}

#define matrT_read(f, A, m, n) \
{ \
  for(i = 0; i < n; i++) { \
  for(j = 0; j < m; j++)   { \
      fscanf(f, "%lf", &A[j*n+i]); \
    } \
  } \
}

#define matr_print(A, m, n) \
{ \
  for(i = 0; i < m; i++) { \
  for(j = 0; j < n; j++)   { \
      printf("%.1lf\t", A[j+i*n]); \
    } \
    putchar('\n'); \
  } \
}

#define matrT_print(A, m, n) \
{ \
  for(j = 0; j < n; j++)   { \
  for(i = 0; i < m; i++) { \
      printf("%.1lf\t", A[j+i*n]); \
    } \
    putchar('\n'); \
  } \
}

double* A, *BT, *CT;
double start, total;

int
main8_1(int argc, char **argv)
{
	MPI_Comm comm_cart;
	MPI_Status st;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &rankc);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	start = MPI_Wtime();

	if (rank == 0) {
		FILE* f = fopen("A.txt", "r");
		fscanf(f, "%d%d", &m, &n);
		A = (double*)malloc(m * n * sizeof(double));
		matr_read(f, A, m, n);
		fclose(f);

		f = fopen("B.txt", "r");
		BT = (double*)malloc(m * n * sizeof(double));
		matrT_read(f, BT, m, n);
		fclose(f);

		CT = (double*)malloc(m * m * sizeof(double));

		for (i = 1; i < rankc; i++) {
			MPI_Send(&n, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			MPI_Send(&m, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			l = (int)((double)m / rankc * i);
			u = (int)((double)m / rankc * (i + 1));
			MPI_Send(&l, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			MPI_Send(&u, 1, MPI_INT, i, 1, MPI_COMM_WORLD);

			MPI_Send(A, m * n, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
			MPI_Send(BT + l * n, n * (u - l), MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
		}
		l = 0;
		u = (int)((double)m / rankc);
	}
	else {
		MPI_Recv(&n, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &st);
		MPI_Recv(&m, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &st);
		MPI_Recv(&l, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &st);
		MPI_Recv(&u, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &st);

		A = (double*)malloc(m * n * sizeof(double));
		MPI_Recv(A, m * n, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, &st);

		BT = (double*)malloc(n * (u - l) * sizeof(double));
		MPI_Recv(BT, n * (u - l), MPI_DOUBLE, i, 1, MPI_COMM_WORLD, &st);

		CT = (double*)malloc(m * (u - l) * sizeof(double));
	}

	/* mult */
	for (i = 0; i < m * (u - l); i++) {
		CT[i] = 0.0;
	}

	for (i = 0; i < (u - l); i++) { /* по B */
		for (j = 0; j < m; j++) { /* по A */
			for (k = 0; k < n; k++) { /* поэлементный проход */
				CT[i*m + j] += A[j*n + k] * BT[i*n + k];
			}
		}
	}

	printf("rank=%d\n", rank);
	printf("l=%d, u=%d, m=%d, n=%d\n", l, u, m, n);
	printf("A:\n");
	matr_print(A, m, n);
	printf("\nBT:\n");
	matr_print(BT, u - l, n);
	printf("\nCT:\n");
	matr_print(CT, u - l, m);


	if (rank != 0) {
		MPI_Send(&l, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
		MPI_Send(&u, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
		MPI_Send(CT, m * (u - l), MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
	}
	else {
		for (i = 1; i < rankc; i++) {
			MPI_Recv(&l, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &st);
			MPI_Recv(&u, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &st);
			MPI_Recv(CT + l * m, m * (u - l), MPI_DOUBLE, i, 1, MPI_COMM_WORLD, &st);
		}
	}

	total = MPI_Wtime();

	if (rank == 0) {
		printf("\nC:\n");
		matrT_print(CT, m, m);

		printf("  total: %lf\n", total - start);
	}

	MPI_Finalize();
	return 0;
}
