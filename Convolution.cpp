// Convolution.cpp : Defines the entry point for the console application.
// do zrobienia ewentualnie - mnożenie i odczytywanie z możliwością jedno wymiarowej tablicy (1)

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <ctime>



using namespace std;

typedef vector<int> Vec;
typedef vector<Vec> Matrix;

void readMat(Matrix mat);
//void readVec(Vec vec);
Matrix matMul( Matrix& a,  Matrix& b);

int main()
{

	srand(time(NULL));
	

	int framesBits, frames = 0;
	float EbN0min, EbN0max, EbN0step = 0;
	Vec bitsIn;
	Matrix bitsChannel;
	Matrix coder(5, Vec(6));

	bool menu = false;
	if (menu == true){
		cout << "Podaj dane:\nLiczba ramek: \n";
		cin >> frames; // calkowita liczba bitow > 10e5
		cout << "Liczba bitow w ramce: \n";
		cin >> framesBits; //100, 500, 1000
		cout << "EbN0 max: \n";
		cin >> EbN0max;
		cout << "EbN0 min: \n";
		cin >> EbN0min;
		cout << "EbN0 step: \n";
		cin >> EbN0step;
		cout << "\n\n\n";

		for (int i = 0; i < framesBits; i++) // generowanie bitow 
		{
			bitsIn.push_back(rand() % 2);
		}
	}
	else {
		bitsIn = { 1, 0, 1,0, 1,1,0,0,1,1,0,0,0,0 };
		frames = 1;
		framesBits = bitsIn.capacity();
	}


	//===================================================== >>> KODER<<< ==========================================
	

		coder = { { 1, 1, 1, 1, 1, 1 } ,{ 1, 0, 1, 1, 0, 1 },{ 0, 1, 1, 0, 1, 1 },{ 1, 0, 1, 1, 0, 1 } ,{ 1, 1, 1, 1, 1, 1 } };
	
	Matrix prev = { {0,0,0,0} };
	for (int i = 0; i < bitsIn.size(); i++) {
		Matrix wynik = matMul(prev, coder);  // (1)
		bitsChannel.push_back(wynik[0]);//(1)
		Vec::iterator it = prev[0].begin(); // pozycja iteratora
		it = prev[0].insert(it, bitsIn[i]);
		prev[0].pop_back();
	}
	
	readMat(bitsChannel); // macierz z przesłanymi sekwencjami wyjsciowymi




    return 0;
}


void readMat(Matrix mat) {		

	for (int i = 0; i < mat.size(); i++) {
		for (int j = 0; j < mat[0].size(); j++) {
			cout << mat[i][j] << " ";
		}
		cout << "\n";
	}
}

/*
void readVec(Vec vec) {

	for (int i = 0; i < vec.size(); i++) {
		cout << vec[i] << " ";
	}
	
}
*/



Matrix matMul(Matrix& a, Matrix& b) {  //(1)

	int n = a.size();
	int m = a[0].size();
	int p = b[0].size();
	int suma = 0;
	Matrix c(n, vector<int>(p));

	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < p; ++j) {

			suma = 0;
			for (int k = 0; k < m; ++k) {
				suma = suma + a[i][k]*b[k][j];
			}
			c[i][j] = suma%2;
		}
	}
	return c;
}