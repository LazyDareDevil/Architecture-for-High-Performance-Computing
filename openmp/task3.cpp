#include <iostream>
#include <cstdlib>
#include <omp.h>
#include <time.h>

using namespace std;

const double MIN_DOUBLE = 0.;
const double MAX_DOUBLE = 10.;

double generate_double() {
	double result = (double)rand() / RAND_MAX;
	return MIN_DOUBLE + result*(MAX_DOUBLE - MIN_DOUBLE);
}

double** example_mtx() {
	double** mtx = new double*[5];
	for (int i = 0; i < 5; ++i) {
		mtx[i] = new double[9];
		for (int j = 0; j < 9; ++j) {
			mtx[i][j] = (double)i + 2 + j;
		}
	}
	return mtx;
}

double* example_vec() {
	double* vec = new double[9];
	for (int i = 0; i < 9; ++i) {
		vec[i] = (double)i + 1;
	}
	return vec;
}

double** generate_matrix(int number_of_rows, int number_of_columns) {

	double** mtx = new double*[number_of_rows];
	for (int i = 0; i < number_of_rows; ++i) {
		mtx[i] = new double[number_of_columns];
		for (int j = 0; j < number_of_columns; ++j) {
			mtx[i][j] = generate_double();
		}
	}
	return mtx;
}

double* generate_vector(int number_of_elements) {
	double* vec = new double[number_of_elements];
	for (int i = 0; i < number_of_elements; ++i) {
		vec[i] = generate_double();
	}
	return vec;
}

void print_matrix(int number_of_rows, int number_of_columns, double** mtx) {
	for (int i = 0; i < number_of_rows; ++i) {
		for (int j = 0; j < number_of_columns; ++j) {
			cout << mtx[i][j] << " ";
		}
		cout << "\n";
	}
}

void print_vector(int number_of_elements, double* vec) {
	for (int i = 0; i < number_of_elements; ++i) {
		cout << vec[i] << " ";
	}
	cout << "\n";
}

double* multiplication_by_rows(int rows, int columns, double** mtx,
	int number_of_elements, double* vec, int number_of_threads) {

	if (columns != number_of_elements) {
		throw 1;
	}
	double* result = new double[rows];
	for (int i = 0; i < rows; ++i) {
		result[i] = 0.;
	}
	int i;
	#pragma omp parallel num_threads(number_of_threads) 
	#pragma omp for schedule(guided)
	for (i = 0; i < rows; ++i) {
		for (int j = 0; j < columns; ++j) {
			result[i] += mtx[i][j] * vec[j];
		}
	}
	return result;
}

double* multiplication_by_columns(int rows, int columns, double** mtx,
	int number_of_elements, double* vec, int number_of_threads) {

	if (columns != number_of_elements) {
		throw 1;
	}
	double* result = new double[rows];
	for (int i = 0; i < rows; ++i) {
		result[i] = 0.;
	}
	int i;
	#pragma omp parallel num_threads(number_of_threads)
	#pragma omp for schedule(guided)
	for (i = 0; i < number_of_elements; ++i) {
		for (int j = 0; j < rows; ++j) {
			result[j] += mtx[j][i] * vec[i];
		}
	}

	return result;
}

double* multiplication_by_blocks(int rows, int columns, double** mtx,
	int number_of_elements, double* vec, int number_of_threads) {

	if (columns != number_of_elements) {
		throw 1;
	}
	double* result = new double[rows];
	for (int i = 0; i < rows; ++i) {
		result[i] = 0.;
	}
	int s = 0, q = 0, k = 0, l = 0;
	// k - размер блока по строкам
	// l - размер блока по столбцам
	// s - количество блоков по строкам
	// q - количество блоков по столбцам

	if (number_of_threads == 1) {
		s = 1, q = 1;
	}
	if (number_of_threads == 2) {
		if (rows % 2 == 0) {
			s = 2, q = 1;
		}
		if (columns % 2 == 0) {
			s = 1, q = 2;
		}
	}
	if (number_of_threads == 3) {
		if (rows % 3 == 0) {
			s = 3, q = 1;
		}
		if (columns % 3 == 0) {
			s = 1, q = 3;
		}
	}
	if (number_of_threads == 4) {
		bool row2 = false, row4 = false, col2 = false, col4 = false;
		if (rows % 2 == 0) row2 = true;
		if (rows % 4 == 0) row4 = true;
		if (columns % 2 == 0) col2 = true;
		if (columns % 4 == 0) col4 = true;
		if (row2 && col2) {
			s = 2, q = 2;
		}
		if (row4) {
			s = 4, q = 1;
		}
		if (col4) {
			s = 1, q = 4;
		}
	}
	if (number_of_threads == 5) {
		if (rows % 5 == 0) {
			s = 5, q = 1;
		}
		if (columns % 5 == 0) {
			s = 1, q = 5;
		}
	}
	if (number_of_threads == 6) {
		bool row2 = false, row3 = false, col2 = false, col3 = false;
		if (rows % 2 == 0) row2 = true;
		if (rows % 3 == 0) row3 = true;
		if (columns % 2 == 0) col2 = true;
		if (columns % 3 == 0) col3 = true;
		if (row2) {
			if (col3) {
				s = 2; q = 3;
			}
			else {
				if (row3) {
					s = 6, q = 1;
				}
			}
		}
		else {
			if (row3) {
				if (col2) {
					s = 3, q = 2;
				}
			}
			else {
				if (col2 && col3) {
					s = 1, q = 6;
				}
			}
		}
	}
	if (number_of_threads == 7) {
		if (rows % 7 == 0) {
			s = 7, q = 1;
		}
		if (columns % 7 == 0) {
			s = 1, q = 7;
		}
	}
	if (number_of_threads == 8) {
		bool row2 = false, row4 = false, row8 = false, col2 = false, col4 = false, col8 = false;
		if (rows % 2 == 0) row2 = true;
		if (rows % 4 == 0) row4 = true;
		if (rows % 8 == 0) row8 = true;
		if (columns % 2 == 0) col2 = true;
		if (columns % 4 == 0) col4 = true;
		if (columns % 8 == 0) col8 = true;
		if (row2) {
			if (row4) {
				if (row8) {
					s = 8, q = 1;
				}
				else {
					if (col2) {
						s = 4, q = 2;
					}
				}
			}
			else {
				if (col4) {
					s = 2, q = 4;
				}
			}
		}
		else {
			if (col8) {
				s = 1, q = 8;
			}
		}
	}
	if (number_of_threads == 9) {
		bool row3 = false, row9 = false, col3 = false, col9 = false;
		if (rows % 3 == 0) row3 = true;
		if (rows % 9 == 0) row9 = true;
		if (columns % 3 == 0) col3 = true;
		if (columns % 9 == 0) col9 = true;
		if (col3) {
			if (col9) {
				q = 9, s = 1;
			}
			if (row3) {
				q = 3, s = 3;
			}
		}
		else {
			if (row9) {
				q = 1, s = 9;
			}
		}
	}
	if (number_of_threads == 10) {
		bool row2 = false, row5 = false, col2 = false, col5 = false;
		if (rows % 2 == 0) row2 = true;
		if (rows % 5 == 0) row5 = true;
		if (columns % 2 == 0) col2 = true;
		if (columns % 5 == 0) col5 = true;
		if (col2) {
			if (col5) {
				q = 10, s = 1;
			}
			else {
				if (row5) {
					q = 2, s = 5;
				}
			}
		}
		if (col5) {
			if (row2) {
				s = 2, q = 5;
			}
		}
		if (row5 && row2) {
			q = 1, s = 10;
		}
	}
	if (number_of_threads == 11) {
		if (rows % 11 == 0) s = 11, q = 1;
		if (columns % 11 == 0) s = 1, q = 11;
	}
	if (number_of_threads == 12) {
		bool row2 = false, row3 = false, row4 = false, col2 = false, col3 = false, col4 = false;
		if (rows % 2 == 0) row2 = true;
		if (rows % 3 == 0) row3 = true;
		if (rows % 4 == 0) row4 = true;
		if (columns % 2 == 0) col2 = true;
		if (columns % 3 == 0) col3 = true;
		if (columns % 4 == 0) col4 = true;
		if (row2) {
			if (row3) {
				if (row4) {
					s = 12, q = 1;
				}
				else {
					if (col2) {
						s = 6, q = 2;
					}
				}
			}
			else {
				if (col3 && col2) {
					s = 2, q = 6;
				}
			}
			if (row4) {
				if (col3) {
					s = 4, q = 3;
				}
			}
		}
		if (row3) {
			if (col4) {
				s = 3, q = 4;
			}
		}
		if (row4 && col3) {
			s = 4, q = 3;
		}
		if (col4 && col3) {
			s = 1, q = 12;
		}
	}
	if (number_of_threads == 13) {
		if (rows % 13 == 0) s = 13, q = 1;
		if (columns % 13 == 0) s = 1, q = 13;
	}
	if (number_of_threads == 14) {
		bool row2 = false, row7 = false, col2 = false, col7 = false;
		if (rows % 2 == 0) row2 = true;
		if (rows % 7 == 0) row2 = true;
		if (columns % 2 == 0) col2 = true;
		if (columns % 7 == 0) col7 = true;
		if (row2 && col7) {
			s = 2, q = 7;
		}
		if (row7 && col2) {
			s = 7, q = 2;
		}
		if (row2 && row7) {
			s = 14, q = 1;
		}
		if (col2 && col7) {
			s = 1, q = 14;
		}
	}
	if (number_of_threads == 15) {
		bool row3 = false, row5 = false, col3 = false, col5 = false;
		if (rows % 3 == 0) row3 = true;
		if (rows % 5 == 0) row5 = true;
		if (columns % 3 == 0) col3 = true;
		if (columns % 5 == 0) col5 = true;
		if (row5 && row3) {
			s = 15, q = 1;
		}
		if (col3 && col5) {
			s = 1, q = 15;
		}
		if (row3 && col5) {
			s = 3, q = 7;
		}
		if (row5 && col3) {
			s = 5, q = 3;
		}
	}
	if (number_of_threads == 16) {
		bool row2 = false, row4 = false, row8 = false, row16 = false, col2 = false, col4 = false, col8 = false, col16 = false;
		if (rows % 2 == 0) {
			row2 = true;
			if (rows % 4 == 0) {
				row4 = true;
				if (rows % 8 == 0) {
					row8 = true;
					if (rows % 16 == 0) {
						row16 = true;
					}
				}
			}
		}
		if (columns % 2 == 0) {
			col2 = true;
			if (columns % 4 == 0) {
				col4 = true;
				if (columns % 8 == 0) {
					col8 = true;
					if (columns % 16 == 0) {
						col16 = true;
					}
				}
			}
		}
		if (col16) s = 1, q = 16;
		if (row16) s = 16, q = 1;
		if (row2) {
			if (col8) s = 2, q = 8;
			else if (row4) {
				if (col4) s = 4, q = 4;
				else if (row8) {
					if (col2) s = 8, q = 2;
				}
			}
		}		
	}
	if (s == 0 || q == 0) {
		cout << "\ncan't do block multiplacation\n";
		return result;
	}
	k = rows / s, l = columns / q;
	int i;
	double*** preRes = new double**[s];
	for (i = 0; i < s; ++i) {
		preRes[i] = new double*[q];
		for (int j = 0; j < q; ++j) {
			preRes[i][j] = new double[k];
			for (int r = 0; r < k; ++r) {
				preRes[i][j][r] = 0.;
			}
		}
	}
	#pragma omp parallel num_threads(number_of_threads)
	#pragma omp for schedule(guided)
	for (i = 0; i < s; ++i) {
		for (int j = 0; j < q; ++j) {
			for (int v = 0; v < k; ++v) {
				for (int u = 0; u < l; ++u) {
					preRes[i][j][v] += mtx[i*k + v][j*l + u] * vec[j*l + u];
				}
			}
		}
	}
	int r;
	#pragma omp parallel num_threads(number_of_threads)
	#pragma omp for schedule(guided)
	for (r = 0; r < rows; ++r) {
		for (int j = 0; j < q; ++j) {
			result[r] += preRes[r / k][j][r - r / k * k];
		}
	}

	return result;
}


int main() {

	srand(time(NULL));

	double** mtx = example_mtx();
	double* vec = example_vec();
	int rows = 5, columns = 9, number_of_elements = 9;

	double start, end;
	double time_1, time_2, time_3;
	double *mult_1, *mult_2, *mult_3;
	try {

		omp_set_dynamic(0);
		cout << "input matrix number of rows:";
		cin >> rows;
		cout << "input matrix number of columns:";
		cin >> columns;
		cout << "input number of elements in vector:";
		cin >> number_of_elements;
		mtx = generate_matrix(rows, columns);
		vec = generate_vector(number_of_elements);
		/*
		cout << "\nmatrix:\n";
		print_matrix(rows, columns, mtx);
		cout << "\nvector:\n";
		print_vector(number_of_elements, vec);
		*/
		for (int i = 1; i < 20; ++i) {
			cout << "\nnumber of threads: " << i << "\n";
			start = omp_get_wtime();
			mult_1 = multiplication_by_rows(rows, columns, mtx, number_of_elements, vec, i);
			end = omp_get_wtime();
			time_1 = end - start;
			start = omp_get_wtime();
			mult_2 = multiplication_by_columns(rows, columns, mtx, number_of_elements, vec, i);
			end = omp_get_wtime();
			time_2 = end - start;
			start = omp_get_wtime();
			mult_3 = multiplication_by_blocks(rows, columns, mtx, number_of_elements, vec, i);
			end = omp_get_wtime();
			time_3 = end - start;
			cout << "\nmultiplication by rows:\n";
			//print_vector(rows, mult_1);
			cout << "in " << time_1*1000. << " milliseconds\n";
			cout << "\nmultiplication by columns:\n";
			//print_vector(rows, mult_2);
			cout << "in " << time_2*1000. << " milliseconds\n";
			cout << "\nmultiplication by blocks:\n";
			//print_vector(rows, mult_3);
			cout << "in " << time_3*1000. << " milliseconds\n";
		}
	}
	catch (int i) {
		if (i == 1) cout << "\nwrong dementions for multiplications\n";
		if (i == 2) cout << "\ncan't do block multiplication\n";
	}

	cin >> rows;
	return 0;
}