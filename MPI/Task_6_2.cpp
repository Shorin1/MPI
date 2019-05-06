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

		/* Обнуляем массив dims и заполняем массив periods для топологии "кольцо" */
		for (i = 0; i < NUM_DIMS; i++) {
			dims[i] = 0;
			periods[i] = 1;
		}
		/* Заполняем массив dims, в котором указываются размеры решетки */
		MPI_Dims_create(size, NUM_DIMS, dims);
		/* Создаем топологию "кольцо" с communicator-ом comm_cart */
		MPI_Cart_create(MPI_COMM_WORLD, NUM_DIMS, dims, periods, reorder, &comm_cart);
		/* Каждая ветвь находит своих соседей вдоль кольца, в направлении больших значений рангов */
		MPI_Cart_shift(comm_cart, 0, 1, &source, &dest);
		/* Каждая ветвь передает свои данные (значение переменной A) соседней ветви
		с большим рангом и принимает данные в B от соседней ветви с меньшим рангом.
		Ветвь с рангом size-1 передает свои данные ветви с рангом 0, а ветвь 0 принимает
		данные от ветви size-1. */

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
		/* Каждая ветвь печатает свой ранг (он же и был послан соседней ветви с большим рангом и значение
		переменной B (ранг соседней ветви с меньшим рангом). */
		/* Все ветви завершают системные процессы, связанные с топологией comm_cart и завершают
		выполнение программы */
		MPI_Comm_free(&comm_cart);
	}
	MPI_Finalize();
	return 0;
}
