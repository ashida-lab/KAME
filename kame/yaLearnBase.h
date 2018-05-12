#pragma once

#include "yaCommon.h"

#define SEED 32767

using namespace std;
using namespace System;
using namespace System::IO;
using namespace System::Security::Cryptography;
using namespace System::Net::NetworkInformation;
using namespace System::Collections;

ref class yaLearnBase
{
public:
	yaLearnBase();

	static double rand1()
	{
		return((double)rand() / RAND_MAX);
	}

	static int rand01()
	{
		int rnd;

		while ((rnd = rand()) == RAND_MAX);

		return((int)((double)rand() / RAND_MAX * 2));
	}

	static int rand03()
	{
		int rnd;

		while ((rnd = rand()) == RAND_MAX);

		return((int)((double)rand() / RAND_MAX * 4));
	}

	static int rand100()
	{
		int rnd;

		while ((rnd = rand()) == RAND_MAX);

		return((int)((double)rand() / RAND_MAX * 101));
	}

	static double drnd()
	{
		double rndno;

		while ((rndno = (double)rand() / RAND_MAX) == 1.0);
		rndno = rndno * 2 - 1;

		return rndno;
	}

	static void quicksort(int value[], int first, int last)
	{
		int i, j;
		double x;
		int t;

		x = value[(first + last) / 2];
		i = first;
		j = last;

		for (;;){
			while (value[i] < x) i++;
			while (x < value[j]) j--;

			if (i >= j) break;
			memcpy(&t, &value[i], sizeof(int));
			memcpy(&value[i], &value[j], sizeof(int));
			memcpy(&value[j], &t, sizeof(int));
			i++;
			j--;
		}
		if (first < i - 1) quicksort(value, first, i - 1);
		if (j + 1 < last) quicksort(value, j + 1, last);
	}

	static int move_select(int selector, interior_ptr<System::Drawing::Point> kamePoint, interior_ptr<System::Drawing::Point> handPoint)
	{
		int kameDirection;

		switch (selector) {
		case 1://Œã‘Þ
			kameDirection = -1;
			kamePoint->X += 0.1*(handPoint->X - kamePoint->X);
			kamePoint->Y += 0.1*(handPoint->Y - kamePoint->Y);
			break;
		case 2://‘Oi
			kameDirection = 0;
			break;
		case 3:
			kameDirection = 90;//¶‰ñ“]
			break;
		case 0:
			kameDirection = 270;//‰E‰ñ“]
			break;
		default:
			break;
		}

		if (kameDirection % 360 == 0) {
			kamePoint->Y -= 10;
		}

		if (kameDirection % 360 == 90) {
			kamePoint->X -= 10;
		}

		if (kameDirection % 360 == 180) {
			kamePoint->Y += 10;
		}

		if (kameDirection % 360 == 270) {
			kamePoint->X += 10;
		}

		return(kameDirection);
	}

};