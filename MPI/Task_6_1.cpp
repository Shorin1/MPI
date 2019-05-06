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
	/* Обнуляем массив dims и заполняем массив periods для топологии "кольцо" */
	for (i = 0; i < NUM_DIMS; i++) {
		dims[i] = 0; periods[i] = 1; 
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
	MPI_Sendrecv(&A, 1, MPI_INT, dest, 12, &B, 1, MPI_INT, source, 12, comm_cart, &status);
	/* Каждая ветвь печатает свой ранг (он же и был послан соседней ветви с большим рангом и значение
	переменной B (ранг соседней ветви с меньшим рангом). */
	printf("rank=%d B=%d\n", rank, B);
	/* Все ветви завершают системные процессы, связанные с топологией comm_cart и завершают
	выполнение программы */
	MPI_Comm_free(&comm_cart);
	MPI_Finalize();
	return 0;
}
