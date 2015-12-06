// Convolution.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <ctime>



using namespace std;

typedef vector<int> Vec;
typedef vector<Vec> Matrix;

void readMat(Matrix mat);
void readMat(Vec mat);
Vec matMul( Vec& a,  Matrix& b); //a może być tablicą JEDNOWYMIAROWĄ
Matrix matMul(Matrix& a, Matrix& b); //w razie potrzeby mnożenia pełnych macierzy - chyba nie przyda się 
int odlHamm(Vec& a, Vec& b);


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
	
	Vec prev =  {0,0,0,0};
	cout << prev.size() << endl << endl;
	for (int i = 0; i < bitsIn.size(); i++) {
		bitsChannel.push_back(matMul(prev, coder));	
		Vec::iterator it = prev.begin(); // pozycja iteratora
		it = prev.insert(it, bitsIn[i]);
		prev.pop_back();
	}
	
	
	
	readMat(bitsChannel); // macierz z przesłanymi sekwencjami wyjsciowymi

	


    return 0;
}
//===================================================== >>> FUNKCJE <<< ==========================================

void readMat(Matrix mat) {		

	for (int i = 0; i < mat.size(); i++) {
		for (int j = 0; j < mat[0].size(); j++) {
			cout << mat[i][j] << " ";
		}
		cout << "\n";
	}
}

void readMat(Vec mat) {
	for (int i = 0; i < mat.size(); i++) {
			cout << mat[i] << " ";
	}
	cout << "\n";
}


Vec matMul(Vec& avec, Matrix& b) {  //(1)

	Matrix a = { { 0 } };
	a[0] = avec;
	Vec cc;
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
	cc = c[0];
	return cc;
}

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
				suma = suma + a[i][k] * b[k][j];
			}
			c[i][j] = suma % 2;
		}
	}
	return c;
}

int odlHamm(Vec& a, Vec& b) {
	int odl = 0;
	if (a.size() != b.size())
		cout << "error\n";
	else {
		for (int i = 0; i < a.size(); i++) {
			if (a[i] != b[i])
				odl++;
		}
	}
	return odl;

}