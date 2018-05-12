#pragma once

#include "yaCommon.h"
#include "yaLearnBase.h"

#define PN 40
#define CN 40
#define EN 24
#define MN 16 //mutation

#define N_gene 3
#define STEP_move 1

#define Generation 4000

typedef struct {
	int gene[N_gene];
	int func;
}Gene;

using namespace std;
using namespace System;
using namespace System::IO;
using namespace System::Security::Cryptography;
using namespace System::Net::NetworkInformation;
using namespace System::Collections;

ref class yaGA
{
public:
	yaGA();

	static int updateGen(Gene* value_pn, Gene* value_cn)
	{
		int i;
		Gene* value_an;

		value_an = (Gene*)malloc((PN + CN) * sizeof(Gene));

		init(value_pn, PN);

		//p_func of parents PN
		compute(value_pn, PN);

		for (i = 0; i < Generation; i++) {

			//generate children PN+CN
			gen_children(value_pn, value_cn);

			//p_func of children
			compute(value_cn, CN);

			//selection PN+CN -> PN
			memcpy(value_an, value_pn, PN * sizeof(Gene));
			memcpy(value_an + PN, value_cn, CN * sizeof(Gene));

			quicksort_gene(value_an, 0, PN + CN - 1);

			printf("Gen %d ---- max ---- %d\n", i, value_an[PN + CN - 1].func);

			memcpy(value_pn, value_an + PN + CN - EN, EN * sizeof(Gene));

			roulette(value_pn, value_an);

			if (i % 100 == 0) {
				output_gene(value_an, i, PN + CN);
				output(value_pn, i, PN);
			}
		}

		return(0);
	}

	static int init(Gene* value, const int particle_num)
	{
		int i, j;

		srand(SEED);

		for (i = 0; i < particle_num; i++) {
			for (j = 0; j < N_gene; j++) {
				value[i].gene[j] = yaLearnBase::rand03();
			}
		}

		return(0);
	}

	static int gen_children(Gene *value_pn, Gene *value_cn)
	{
		int p0, p1, s;
		int i, j;

		for (i = 0; i < CN / 2; i++) {
			p0 = floor(PN*(double)rand() / (double)(RAND_MAX + 1.));
			p1 = floor(PN*(double)rand() / (double)(RAND_MAX + 1.));
			s = floor((N_gene + 1)*(double)rand() / (double)(RAND_MAX + 1.));

			value_cn[2 * i].gene[s] = value_pn[p0].gene[s];
			value_cn[2 * i + 1].gene[s] = value_pn[p1].gene[s];
		}

		//mutation
		for (i = 0; i < MN; i++) {
			p0 = floor(CN*(double)rand() / (double)(RAND_MAX + 1.));
			for (j = 0; j < N_gene; j++) {
				value_cn[p0].gene[j] = yaLearnBase::rand03();
			}
		}

		return(0);
	}

	static int compute(Gene* value, const int particle_num)
	{
		int i;

		for (i = 0; i < STEP_move; i++) {
			track(value, i, particle_num);
		}

		return(0);
	}

	static int track(Gene* value, int i, const int particle_num)
	{
		int kameDirection = 0;
		System::Drawing::Point kamePoint;
		System::Drawing::Point handPoint;

		handPoint.X = 275;
		handPoint.Y = 220;

		for (int i = 0; i < particle_num; i++) {

			kamePoint.X = 275;
			kamePoint.Y = 220;

			for (int j = 0; j < N_gene; j++) {
				kameDirection = yaLearnBase::move_select(value[i].gene[j], &kamePoint, &handPoint);

			}

			float distance = sqrt(kamePoint.X*kamePoint.X + kamePoint.Y*kamePoint.Y);

			value[i].func = 1000 - distance;
		}

		return(0);
	}

	static void quicksort_gene(Gene* a, int first, int last)
	{
		int i, j;
		double x;
		Gene t;

		x = a[(first + last) / 2].func;
		i = first;
		j = last;

		for (;;) {
			while (a[i].func < x) i++;
			while (x < a[j].func) j--;

			if (i >= j) break;
			memcpy(&t, &a[i], sizeof(Gene));
			memcpy(&a[i], &a[j], sizeof(Gene));
			memcpy(&a[j], &t, sizeof(Gene));
			i++;
			j--;
		}
		if (first < i - 1) quicksort_gene(a, first, i - 1);
		if (j + 1 < last) quicksort_gene(a, j + 1, last);
	}

	static int roulette(Gene* value, Gene* a)
	{
		int i;
		int p;

		for (i = EN; i < PN; i++) {
			p = floor((PN + CN - EN)*sqrt((double)rand() / (double)(RAND_MAX + 1.)));
			memcpy(&value[i], &a[p], sizeof(Gene));
		}

		return(0);
	}

	static int output(Gene* value, const int gen, const int particle_num)
	{
		int p;
		FILE *fp;
		char filename[256];

		sprintf(filename, "output-%d.txt", gen);
		fp = fopen(filename, "w");
		for (p = 0; p < particle_num; p++) {
			fprintf(fp, "%d %d\n", p, value[p].func);
		}
		fclose(fp);

		return(0);
	}

	static int output_gene(Gene* value, const int gen, const int particle_num)
	{
		int i, j;
		FILE *fp;
		char filename[256];

		sprintf(filename, "gene-%d.txt", gen);
		fp = fopen(filename, "w");
		for (i = 1; i < 6; i++) {
			fprintf(fp, "%05d |", i);
			for (j = 0; j < N_gene; j++) {
				fprintf(fp, "%d ", value[particle_num - i].gene[j]);
			}
			fprintf(fp, "|%d \n", value[particle_num - i].func);
		}

		for (i = particle_num - 4; i < particle_num + 1; i++) {
			fprintf(fp, "%05d |", i);
			for (j = 0; j < N_gene; j++) {
				fprintf(fp, "%d ", value[particle_num - i].gene[j]);
			}
			fprintf(fp, "|%d \n", value[particle_num - i].func);
		}
		fclose(fp);

		return(0);
	}
};