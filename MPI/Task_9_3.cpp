#include <iostream>
#include <fstream>

using namespace std;

double** a, * b, * result;
int n, j, k;

void swap() {
	double max;
	int index;

	max = a[k][k];
	index = k;

	for (int i = k + 1; i < n; i++)
	{
		if (a[i][k] > max)
		{
			max = a[i][k];
			index = i;
		}
	}

	if (index != k) {
		for (int i = 0; i < n; i++) {
			double temp = a[k][i];
			a[k][i] = a[index][i];
			a[index][i] = temp;
		}

		double temp = b[k];
		b[k] = b[index];
		b[index] = temp;
	}
}

void normalize() {
#pragma omp parallel
	{
#pragma omp for private (j)
		for (int i = k; i < n; i++) {
			double temp = a[i][k];
			if (abs(temp) != 0) {

				for (j = k; j < n; j++) {
					a[i][j] /= temp;
				}

				b[i] /= temp;
			}
		}
	}
}

void substraction() {
#pragma omp parallel
	{
#pragma omp for private (j)
		for (int i = k; i < n; i++) {
			if (i != k) {

				for (j = k; j < n; j++) {
					a[i][j] -= a[k][j];
				}

				b[i] -= b[k];
			}
		}
	}
}

void reverse() {
	//cout << "B:" << endl;

	//for (int i = 0; i < n; i++) {
	//	cout << b[i] << " ";
	//}

	//for (int i = n - 1; i >= 0; i--) {
	//	result[i] = b[i];

	//	for (int j = 0; j < i; j++) {
	//		b[j] -= a[j][i] * result[i];
	//	}
	//}

	for (int i = n - 1; i >= 0; i--) {
		result[i] = 0.0;

		for (j = i; j < n - 1; j++) {
			result[i] -= result[j + 1] * a[i][j + 1];
		}

		result[i] += b[i];
	}
}

void gauss()
{
	for (k = 0; k < n; k++)
	{
		swap();
		normalize();
		substraction();
	}

	reverse();
}

void readAll() {
	fstream fa("D://Projects//C++//openMP_9//Debug//a.txt");

	fa >> n;

	a = new double* [n];
	b = new double[n];
	result = new double[n];

	for (int i = 0; i < n; i++) {
		a[i] = new double[n];

		for (int j = 0; j < n; j++) {
			fa >> a[i][j];
		}
	}

	fa.close();

	fstream fb("D://Projects//C++//openMP_9//Debug//b.txt");

	for (int i = 0; i < n; i++) {
		fb >> b[i];
	}

	fb.close();
}

void printAll() {
	cout << "A:" << endl;

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			cout << a[i][j] << " ";
		}
		cout << endl;
	}

	cout << "B:" << endl;

	for (int i = 0; i < n; i++) {
		cout << b[i] << " ";
	}

	cout << endl << "C:" << endl;

	for (int i = 0; i < n; i++) {
		cout << result[i] << " ";
	}
}

int main()
{
	readAll();
	gauss();
	printAll();

	cin >> n;
	return 0;
}