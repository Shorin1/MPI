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
	/* ќбнул€ем массив dims и заполн€ем массив periods дл€ топологии "линейка" */
	dims[0] = 0; 
	periods[0] = 0; 
	/* «аполн€ем массив dims, где указываютс€ размеры (одномерной) решетки */
	MPI_Dims_create(size, NUM_DIMS, dims);
	/* —оздаем топологию "линейка" с communicator-ом comm_cart */
	MPI_Cart_create(MPI_COMM_WORLD, NUM_DIMS, dims, periods, reorder, &comm_cart);
	/* ќтображаем ранги в координаты и выводим их */
	MPI_Cart_coords(comm_cart, rank, NUM_DIMS, new_coords);
	A = rank;
	B = -1;
	/*  ажда€ ветвь находит своих соседей вдоль линейки, в направлении больших значений номеров компьютеров
	и в направлении меньших значений номеров. ¬етви с номером new_coords[0]==0 не имеют соседей
	с меньшим номером, поэтому с этого направлени€ эти ветви принимают данные от несуществующих
	ветвей, т.е. от ветвей sourcem=MPI_PROC_NULL, и, соответственно, передают данные в этом направлении
	ветв€м destm=MPI_PROC_NULL. јналогично определ€етс€ соседство дл€ ветвей с номером new_coords[0]==dims[0]-1. */
	if (rank == 0) // ≈сли 0 процесс
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
	/*  ажда€ ветвь передает свои данные (значение переменной ј) своей соседней ветви с большим
	номером и принимает данные в B от соседней ветви с меньшим номером. —вой номер и номер, прин€тый
	в B вывод€тс€ на печать. */
	MPI_Sendrecv(&A, 1, MPI_INT, destb, 12, &B, 1, MPI_INT, sourcem, 12, comm_cart, &status);
	printf("new_coords[0]=%d B=%d\n", new_coords[0], B);
	/* —двиг данных в противоположную сторону и вывод соответствующих данных */
	MPI_Sendrecv(&A, 1, MPI_INT, destm, 12, &B, 1, MPI_INT, sourceb, 12, comm_cart, &status);
	printf("new_coords[0]=%d B=%d\n", new_coords[0], B);
	/* ¬се ветви завершают системные процессы, св€занные с топологие comm_cart и завершают программы */
	MPI_Comm_free(&comm_cart);
	MPI_Finalize();
	return 0;
}
