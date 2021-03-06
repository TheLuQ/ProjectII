﻿// Convolution.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>

#define PI 3.141592654

using namespace std;

typedef vector<int> Vec;
typedef vector<Vec> Matrix;

void readMat(Matrix mat);
void readMat(Vec mat);
Vec matMul( Vec& a,  Matrix& b); //a może być tablicą JEDNOWYMIAROWĄ
Matrix matMul(Matrix& a, Matrix& b); //w razie potrzeby mnożenia pełnych macierzy - chyba nie przyda się 
int odlHamm(Vec& a, Vec& b);
Vec intToVec(int intNumber);
int VecToInt(Vec binNumb);
Vec seqOut(Vec prevState, Matrix coder);
Vec addAndMove(int bitIn, Vec& prevState);
int stateToInt(Vec State); // zwraca numer następnego stanu
Matrix result(vector<vector<float>> bitSoft);
vector< vector<float> > kanal(float es_n0, Matrix bitsInput);
float gauss(float mean, float sigma);


int main()
{

	srand(time(NULL));
	

	int framesBits, frames = 0;
	float EbN0min, EbN0max, EbN0step = 0;
	Vec bitsIn;
	Matrix bitsChannel;
	Matrix coder(5, Vec(6));
	float progress = 0.0;

	bool menu = false;
	if (menu == true){
		cout << "Liczba bitow: \n";
		cin >> framesBits; //100, 500, 1000
		cout << "EbN0 max: \n";
		cin >> EbN0max;
		cout << "EbN0 min: \n";
		cin >> EbN0min;
		cout << "EbN0 step: \n";
		cin >> EbN0step;
		cout << "\n\n\n";

		bitsIn = Vec(framesBits, 0);
		for (int i = 0; i < framesBits; i++) // generowanie bitow 
		{
			bitsIn[i] = rand() % 2;
		}
	}
	else {
		bitsIn = Vec(500, 0);
		for (int i = 0; i < bitsIn.size(); i++) // generowanie bitow 
		{
			bitsIn[i] = rand() % 2;
			if (i > 25)
				bitsIn[i] = 0;
		}
		framesBits = bitsIn.capacity();
		//readMat(bitsIn);
	}

	
	//===================================================== >>> KODER<<< ==========================================
	

	
	coder = { { 1, 1, 1, 1, 1, 1 } ,{ 1, 0, 1, 1, 0, 1 },{ 0, 1, 1, 0, 1, 1 },{ 1, 0, 1, 1, 0, 1 } ,{ 1, 1, 1, 1, 1, 1 } };
	
	Vec prev =  {0,0,0,0};
	for (int i = 0; i < bitsIn.size(); i++) {
		Vec::iterator it = prev.begin(); // pozycja iteratora
		it = prev.insert(it, bitsIn[i]);
		bitsChannel.push_back(seqOut(prev, coder));
		prev.pop_back();
	}
	
	//readMat(bitsChannel); // macierz z przesłanymi sekwencjami wyjsciowymi
	cout << endl;
	//readMat(result(kanal(15,bitsChannel)));

	bitsChannel = result(kanal(-15, bitsChannel));



//===================================================== >>> DEKODER<<< ==========================================


	int nrRej = 4;
	int maxRej = 16;
	Vec prevState(4, 0);
	Matrix tempPathes(2* maxRej, Vec(framesBits)); // 4 rejestry 2^4 // tymczasowe wszyskie scieżki
	Matrix pathes(maxRej, Vec(framesBits));		// wszystkie ocalone ścieżki
	Vec lastState(2*maxRej, 0);	// ostatni stan każdej możliwej ścieżki
	Vec findDuplHamm(maxRej, 0); 
	Vec survInx(maxRej,0);		// ocalałe indexy ścieżek o najmniejszym hammingu
	Vec hamm(2 * maxRej,0);
	Vec savedHamm(maxRej, 0);
	bool flag = false;
	int ii = 0; // do pathes uzupelnianie

	pathes = Matrix(maxRej, Vec(framesBits, 0));
	tempPathes = Matrix(2 * maxRej, Vec(framesBits,0));


	for (int i = 0; i < maxRej; i++) { // pierwsze uzupelnienie pathes
		Vec temp2(nrRej, 0); 
		Vec temp = intToVec(i);
		if (temp.size() < maxRej) {
			Vec fillUp(nrRej - temp.size(), 0);
			copy(temp.begin(), temp.end(), temp2.begin()+ nrRej - temp.size());	
		}
		copy(temp2.begin(), temp2.end(), pathes[i].begin());
	}
	
	ii = 0;
	for (int i = 0; i < maxRej; i++) { //  uzupelnienie tablicy tempPathes

		copy(pathes[i].begin(), pathes[i].begin() + nrRej, tempPathes[ii].begin());
		ii++;
		copy(pathes[i].begin(), pathes[i].begin() + nrRej, tempPathes[ii].begin());
		ii++;
	}

	for (int i = 0; i < 2 * maxRej; i++) { // i-ta ścieżka
		for (int j = 0; j < nrRej; j++) {// j -ty bit ścieżki możliwej // dla późniejszych należy zwiększyć
			addAndMove(tempPathes[i][j], prevState);
			hamm[i] += odlHamm(seqOut(prevState, coder), bitsChannel[j]);
			prevState.pop_back();
		}
		prevState = Vec(4, 0);
	}

	
	for (int i = 0; i < 2*maxRej; i=i+2)
		savedHamm[i/2] = hamm[i];



	for (int b = nrRej; b < framesBits - 1; b++) { //b-ty bit wejscia

		ii = 0;
		for (int i = 0; i < maxRej; i++) { //  uzupelnienie tablicy tempPathes z ocalalych scieżek pathes

			copy(pathes[i].begin(), pathes[i].begin() + b, tempPathes[ii].begin());
			tempPathes[ii][b] = ii % 2;
			hamm[ii] = savedHamm[i];
			ii++;
			copy(pathes[i].begin(), pathes[i].begin() + b, tempPathes[ii].begin());
			tempPathes[ii][b] = ii % 2;
			hamm[ii] = savedHamm[i];
			ii++;
		}



		survInx = Vec(maxRej, 0);

		for (int j = 0; j < 2 * maxRej; j++) { // j-ta ścieżka
			vector<int> prevState;
			prevState.assign(tempPathes[j].begin() + b - nrRej, tempPathes[j].begin() + b + 1);
			reverse(prevState.begin(), prevState.end());
			hamm[j] += odlHamm(seqOut(prevState, coder), bitsChannel[b]);
			lastState[j] = stateToInt(prevState);


			if (findDuplHamm[lastState[j]] == 0) { //(*)
				findDuplHamm[lastState[j]] = hamm[j];
				survInx[lastState[j]] = j;
			}
			else if (findDuplHamm[lastState[j]] > hamm[j]) {
				findDuplHamm[lastState[j]] = hamm[j];
				survInx[lastState[j]] = j;
			}

			if (hamm[j] == 0)
				findDuplHamm[lastState[j]] = -1;	//żeby algorytm nie mógł nadpisać nad nią jakiejś ścieżki - związane z (*)

			//cout << hamm[j] << endl;
			//readMat(findDuplHamm); // dobre sprawdzanie !!!

		}


		for (int i = 0; i < findDuplHamm.size(); i++) {
			if (findDuplHamm[i] == -1)
				findDuplHamm[i] == 0;
		}

		for (int i = 0; i < maxRej; i++) {
			copy(tempPathes[survInx[i]].begin(), tempPathes[survInx[i]].begin() + b + 2, pathes[i].begin());
			savedHamm[i] = hamm[survInx[i]];
		}

		findDuplHamm = Vec(maxRej, 0);



		int barWidth = 70;
		float step = 20;
		if (b % (int)round(framesBits / step) == 0 || flag == true) {
			std::cout << "8";
			int pos = barWidth * progress;
			for (int i = 0; i < barWidth; ++i) {
				if (i < pos) std::cout << "=";
				else if (i == pos) std::cout << "D";
				else std::cout << " ";
			}
			cout << "] " << int(progress * 100.0) << " %\r";
			cout.flush();
			progress += 1/step; 
			if (flag == true) {
				cout << endl;
			}
		}
		else if (b == framesBits - 3){
			progress = 1;
			flag = true;
		}
	}
	
	//readMat(tempPathes[min_element(hamm.begin(), hamm.end()) - hamm.begin()]);

	cout << "Loading complete\n";
	cout << "errors : " << odlHamm(tempPathes[min_element(hamm.begin(), hamm.end()) - hamm.begin()], bitsIn) << endl;
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

Vec intToVec(int intNumber) {
	Vec binaryVec;

	while (intNumber>0)
	{
		binaryVec.push_back(intNumber % 2);
		intNumber = intNumber / 2;
	}
	reverse(binaryVec.begin(), binaryVec.end());
	return binaryVec;
}

int VecToInt(Vec binNumb) {

	int suma = 0;
	for (int i = 0; i < binNumb.size(); i++) {
		suma += binNumb[i] * pow(2, binNumb.size() - 1  - i);
	}
	return suma;

}

Vec seqOut(Vec prevState, Matrix coder) {

	Vec seq;
	seq = matMul(prevState, coder);
	return seq;
}

Vec addAndMove(int bitIn, Vec& prevState) {
	Vec::iterator it = prevState.begin(); // pozycja iteratora
	it = prevState.insert(it, bitIn);
	return prevState;
}

int stateToInt(Vec State) {
	State.pop_back();
	return VecToInt(State);

}

Matrix result(vector<vector<float>>  bitSoft){
	int a = bitSoft.size();
	int b = bitSoft[0].size();
	Matrix out(a, Vec(b));
	for (int i = 0; i < bitSoft.size(); i++) {
		for (int j = 0; j < bitSoft[0].size(); j++) {
			//cout << bitSoft[i][j] << " ";
			if (bitSoft[i][j] > 0)
				out[i][j] = 1;
			else
				out[i][j] = 0;
		}
	}

	return out;
}

vector< vector<float> > kanal(float es_n0, Matrix bitsInput)
{
	int a = bitsInput.size();
	int b = bitsInput[0].size();
	vector< vector<float> > bitsSoftOut(a, vector<float>(b));
	float mean = 0;
	float es = 1;
	float sygnal;
	float sigma;
	float s_n;
	long y;

	s_n = (float)pow(10, (es_n0 / 10));
	sigma = (float)sqrt(es / (2 * s_n));

	for (int i = 0; i < bitsInput.size(); i++) {
		for (int j = 0; j < bitsInput[0].size(); j++) {
			//cout << i << " " << j << endl;
			bitsSoftOut[i][j] = (float)bitsInput[i][j];
			
			sygnal = 2 * bitsInput[i][j] - 1; // zamiana z wart binarnej 0/1 na symbole -1/+1

			bitsSoftOut[i][j] = sygnal + gauss(mean, sigma);  // dodanie szumu
			//cout << bitsSoftOut[i][j] << endl;

		}
	}


	return bitsSoftOut;
}


float gauss(float mean, float sigma)
{
	double x;
	double z;
	z = (double)rand() / RAND_MAX;
	if (z == 1.0) z = 0.9999999;
	x = sigma*sqrt(2.0*log(1.0 / (1.0 - z)));

	z = (double)rand() / RAND_MAX;
	if (z == 1.0) z = 0.9999999;
	return((float)(mean + x*cos(2 * PI*z)));
}