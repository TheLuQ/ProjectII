// Convolution.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>


using namespace std;


int main()
{
	int framesBits, frames = 0;
	float EbN0min, EbN0max, EbN0step = 0;


	bool menu = false;
	if (menu == true){
		cout << "Podaj dane:\nLiczba ramek: \n";
		cin >> frames;
		cout << "Liczba bitow w ramce: \n";
		cin >> framesBits;
		cout << "EbN0 max: \n";
		cin >> EbN0max;
		cout << "EbN0 min: \n";
		cin >> EbN0min;
		cout << "EbN0 step: \n";
		cin >> EbN0step;
	}
	else {
		frames = 1;
		framesBits = 15;
	}







    return 0;
}

