#pragma once

#include "yaCommon.h"
#include "yaLearnBase.h"
#include <math.h>

using namespace std;
using namespace System;
using namespace System::IO;
using namespace System::Security::Cryptography;
using namespace System::Net::NetworkInformation;
using namespace System::Collections;

#define GENMAX 1000
#define CHOISES (4)
#define STEPS 4
#define NODENO 341//85//((CHOISES^(STEPS+1)-1)/(CHOISES-1))
#define ALPHA 0.1
#define GAMMA 0.9
#define EPSOLON 0.3

ref class yaQLearning
{
public:
	yaQLearning();

	static System::Void initq(int qvalue[NODENO])
	{
		srand(SEED);

		for (int i = 0; i < NODENO; i++) {
			qvalue[i] = yaLearnBase::rand100();
		}
		//printqvalue(qvalue);
	}

	static int updateq(int s, int q, int qvalue[NODENO])
	{
		int qv;
		int qmax;
		int tmp_value[CHOISES];

		printf("%d\n", s);

		if (s > (pow(CHOISES, STEPS)-1) / (CHOISES - 1) - 1) {
			if (q < 150) {
				qv = qvalue[s] + ALPHA*(1000 - qvalue[s]);

				printf("add\n");

				//qv = qvalue[s] + ALPHA*(-q);
			}
			else {
				qv = qvalue[s];
			}
		}
		else {
			for (int i = 0; i < CHOISES; i++) {
				tmp_value[i] = qvalue[CHOISES * s + i + 1];
			}
			yaLearnBase::quicksort(tmp_value, 0, CHOISES-1);

			qmax = tmp_value[CHOISES-1];

			//printf("minmax %d %d\n", tmp_value[0], tmp_value[CHOISES - 1]);

			qv = qvalue[s] + ALPHA*(GAMMA*qmax - qvalue[s]);
		}

		return(qv);
	}

	static int selecta(int olds, int qvalue[NODENO])
	{
		int s;
		int tmp_value[CHOISES];

		if (yaLearnBase::rand1() < EPSOLON) {
			switch (yaLearnBase::rand03()) {

			case 0:
				s = CHOISES * olds + 1;
				break;
			case 1:
				s = CHOISES * olds + 2;
				break;
			case 2:
				s = CHOISES * olds + 3;
				break;
			case 3:
				s = CHOISES * olds + CHOISES;
				break;

			default:
				s = 0;
				break;
			}
		}
		else {
			for (int i = 0; i < CHOISES; i++) {
				tmp_value[i] = qvalue[CHOISES * olds + i + 1];
			}
			yaLearnBase::quicksort(tmp_value, 0, CHOISES-1);

			if (tmp_value[CHOISES - 1] == qvalue[CHOISES * olds + 1]) {
				s = CHOISES * olds + 1;
			}

			if (tmp_value[CHOISES - 1] == qvalue[CHOISES * olds + 2]) {
				s = CHOISES * olds + 2;
			}

			if (tmp_value[CHOISES - 1] == qvalue[CHOISES * olds + 3]) {
				s = CHOISES * olds + 3;
			}

			if (tmp_value[CHOISES - 1] == qvalue[CHOISES * olds + CHOISES]) {
				s = CHOISES * olds + CHOISES;
			}

		}

		return(s);
	}

	static System::Void printqvalue(int qvalue[NODENO])
	{
		int i, j;
		int count = 0;

		FILE* fp = fopen("qvalue.csv", "w");

		for (j = 0; j < STEPS; j++) {
			for (i = 0; i < pow(CHOISES,j); i++) {
				for (int k = 0; k < pow(CHOISES, STEPS - j - 1); k++) {
					fprintf(fp, "%d, ", qvalue[count]);
				}
				count++;
			}

			fprintf(fp,"\n");
		}

		fclose(fp);
	}

};