#pragma once

#pragma once

#include "yaCommon.h"
#include "yaLearnBase.h"

#define INPUTSIZE 11 //���͐�
#define FILTERSIZE 3 //�t�B���^�T�C�Y
#define FILTERNO 2 //�t�B���^��
#define POOLSIZE 3 //�v�[�����O�T�C�Y
#define POOLOUTSIZE 3 //�v�[�����O�̏o�̓T�C�Y
#define HIDDENNO 3 //���ԑw�̃Z����
#define ALPHA 10 //�w�K�W��
#define MAXINPUTNO 100 //�w�K�f�[�^�̍ő��
#define BIGNUM 100 //�덷�̏����l
#define LIMIT 0.001 //�덷�̏���l

using namespace std;
using namespace System;
using namespace System::IO;
using namespace System::Security::Cryptography;
using namespace System::Net::NetworkInformation;
using namespace System::Collections;

ref class yaCNN
{
public:
	yaCNN();

	static int updateCNN(double e[][INPUTSIZE][INPUTSIZE], int t[],int n_of_e)
	{
		//double e[MAXINPUTNO][INPUTSIZE][INPUTSIZE];//���̓f�[�^
		//int t[MAXINPUTNO];//���t�f�[�^
		//int n_of_e;//�w�K�f�[�^�̌�

		double filter[FILTERNO][FILTERSIZE][FILTERSIZE];//�t�B���^
		double convout[INPUTSIZE][INPUTSIZE] = { 0 };//�􍞂ݏo��
		double poolout[POOLOUTSIZE][POOLOUTSIZE];//�o�̓f�[�^
		int i, j, m, n;
		double err = BIGNUM;//�덷�̕]��
		int count = 0;
		double ef[POOLOUTSIZE*POOLOUTSIZE*FILTERNO + 1];//�S�����w�ւ̓��̓f�[�^�A+1�͋��t�f�[�^�p
		double o;//�ŏI�o��
		double hi[HIDDENNO + 1];//���ԑw�̏o��
		double wh[HIDDENNO][POOLOUTSIZE*POOLOUTSIZE*FILTERNO + 1];//���ԑw�̏d��
		double wo[HIDDENNO + 1];//�o�͑w�̏d��

		srand(SEED);

		//�t�B���^������
		initfilter(filter);

		//�d�ݏ�����
		initwh(wh);
		initwo(wo);
		print(wh, wo);

		//�f�[�^����
		//n_of_e = getdata(e, t);
		showdata(e, t, n_of_e);

		//�w�K
		while (err > LIMIT) {
			err = 0.0;
			for (i = 0; i < n_of_e; ++i) {//�w�K�f�[�^���ƂɌJ��Ԃ�
				for (j = 0; j < FILTERNO; ++j) {//�t�B���^���ƂɌJ��Ԃ�
					conv(filter[j], e[i], convout);//�􍞂�
					pool(convout, poolout);//�v�[�����O

					//�S�����w�փR�s�[
					for (m = 0; m < POOLOUTSIZE; ++m) {
						for (n = 0; n < POOLOUTSIZE; ++n) {
							ef[j*POOLOUTSIZE*POOLOUTSIZE + POOLOUTSIZE*m + n] = poolout[m][n];
						}
					}
					ef[POOLOUTSIZE*POOLOUTSIZE*FILTERNO] = t[i];//���t�f�[�^
				}

				//�������̌v�Z
				o = forward(wh, wo, hi, ef);
				//�o�͑w�̏d�ݒ���
				olearn(wo, hi, ef, o);
				//���ԑw�̏d�ݒ���
				hlearn(wh, wo, hi, ef, o);
				//�덷�̐ώZ
				err += (o - t[i])*(o - t[i]);
			}
			++count;

			fprintf(stderr, "%d\t%lf\n", count, err);
		}

		printf("\n***Results***\n");
		printf("Weights\n");
		print(wh, wo);

		printf("Network Output\n");
		printf("#\tteacher\toutput\n");

		for (i = 0; i < n_of_e; ++i) {
			for (j = 0; j < FILTERNO; ++j) {
				conv(filter[j], e[i], convout);
				pool(convout, poolout);

				for (m = 0; m < POOLOUTSIZE; ++m) {
					for (n = 0; n < POOLOUTSIZE; ++n) {
						ef[j*POOLOUTSIZE*POOLOUTSIZE + POOLOUTSIZE*m + n] = poolout[m][n];
					}
				}
				ef[POOLOUTSIZE*POOLOUTSIZE*FILTERNO] = t[i];
			}

			o = forward(wh, wo, hi, ef);
			printf("%lf\n", o);
		}

		return 0;
	}

	//���ԑw�̏d�ݒ���
	static void hlearn(double wh[HIDDENNO][POOLOUTSIZE*POOLOUTSIZE*FILTERNO + 1], double wo[HIDDENNO + 1], double hi[], double e[POOLOUTSIZE*POOLOUTSIZE*FILTERNO + 1], double o)
	{
		int i, j;
		double dj;

		for (j = 0; j < HIDDENNO; ++j) {
			dj = hi[j] * (1 - hi[j])*wo[j] * (e[POOLOUTSIZE*POOLOUTSIZE*FILTERNO]/*���t�f�[�^*/ - o/*�o��*/)*o*(1 - o);
			for (i = 0; i < POOLOUTSIZE*POOLOUTSIZE*FILTERNO; ++i) {
				wh[j][i] += ALPHA*e[i] * dj;
			}
			wh[j][i] += ALPHA*(-1)*dj;
		}
	}

	//�o�͑w�̏d�ݒ���
	static void olearn(double wo[HIDDENNO + 1], double hi[], double e[POOLOUTSIZE*POOLOUTSIZE*FILTERNO + 1], double o)
	{
		int i;
		double d;

		d = (e[POOLOUTSIZE*POOLOUTSIZE*FILTERNO]/*���t�f�[�^*/ - o/*�o��*/)*o*(1 - o);

		for (i = 0; i < HIDDENNO; ++i) {
			wo[i] += ALPHA*hi[i] * d;
		}
		wo[i] += ALPHA*(-1)*d;
	}

	//�������̌v�Z
	static double forward(double wh[HIDDENNO][POOLOUTSIZE*POOLOUTSIZE*FILTERNO + 1], double wo[HIDDENNO + 1], double hi[], double e[POOLOUTSIZE*POOLOUTSIZE*FILTERNO + 1])
	{
		int i, j;
		double u;
		double o;

		for (i = 0; i < HIDDENNO; ++i) {
			u = 0;
			for (j = 0; j < POOLOUTSIZE*POOLOUTSIZE*FILTERNO; ++j) {
				u += e[j] * wh[i][j];
			}
			u -= wh[i][j];
			hi[i] = f(u);
		}

		o = 0;
		for (i = 0; i < HIDDENNO; ++i) {
			o += hi[i] * wo[i];
		}
		o -= wo[i];

		return(f(o));
	}

	//���ԑw�̏d�ݏ�����
	static void initwh(double wh[HIDDENNO][POOLOUTSIZE*POOLOUTSIZE*FILTERNO + 1])
	{
		int i, j;

		for (i = 0; i < HIDDENNO; ++i) {
			for (j = 0; j < POOLOUTSIZE*POOLOUTSIZE*FILTERNO + 1; ++j) {
				wh[i][j] = yaLearnBase::drnd();
			}
		}
	}

	//�o�͑w�̏d�ݏ�����
	static void initwo(double wo[HIDDENNO + 1])
	{
		int i;

		for (i = 0; i < HIDDENNO + 1; ++i) {
			wo[i] = yaLearnBase::drnd();
		}
	}

	//�t�B���^�̏�����
	static void initfilter(double filter[FILTERNO][FILTERSIZE][FILTERSIZE])
	{
		int i, j, k;

		for (i = 0; i < FILTERNO; ++i) {
			for (j = 0; j < FILTERSIZE; ++j) {
				for (k = 0; k < FILTERSIZE; ++k) {
					filter[i][j][k] = yaLearnBase::drnd();
				}
			}
		}
	}

	//�v�[�����O�̌v�Z
	static void pool(double convout[][INPUTSIZE], double poolout[][POOLOUTSIZE])
	{
		int i, j;

		for (i = 0; i < POOLOUTSIZE; ++i) {
			for (j = 0; j < POOLOUTSIZE; ++j) {
				poolout[i][j] = maxpooling(convout, i, j);
			}
		}
	}

	//�ő�l�v�[�����O
	static double maxpooling(double convout[][INPUTSIZE], int i, int j)
	{
		int m, n;
		double max;
		int halfpool = POOLSIZE / 2;

		max = convout[i*POOLOUTSIZE + 1 + halfpool][j*POOLOUTSIZE + 1 + halfpool];

		for (m = POOLOUTSIZE*i + 1; m <= POOLOUTSIZE*i + 1 + (POOLSIZE - halfpool); ++m) {
			for (n = POOLOUTSIZE*j + 1; n <= POOLOUTSIZE*j + 1 + (POOLSIZE - halfpool); ++n) {
				if (max < convout[m][n]) {
					max = convout[m][n];
				}
			}
		}

		return max;
	}

	//�􍞂݂̌v�Z
	static void conv(double filter[][FILTERSIZE], double e[][INPUTSIZE], double convout[][INPUTSIZE])
	{
		int i, j;
		int startpoint = FILTERSIZE / 2;

		for (i = startpoint; i < INPUTSIZE - startpoint; ++i) {
			for (j = startpoint; j < INPUTSIZE - startpoint; ++j) {
				convout[i][j] = calconv(filter, e, i, j);
			}
		}
	}

	//�t�B���^�̓K�p
	static double calconv(double filter[][FILTERSIZE], double e[][INPUTSIZE], int i, int j)
	{
		int m, n;
		double sum = 0;

		for (m = 0; m < FILTERSIZE; ++m) {
			for (n = 0; n < FILTERSIZE; ++n) {
				sum += e[i - FILTERSIZE / 2 + m][j - FILTERSIZE / 2 + n] * filter[m][n];
			}
		}

		return sum;
	}

	//�`�B�֐�
	static double f(double u)
	{
		return(1.0 / (1.0 + exp(-u)));
	}

	//�f�[�^�\��
	static void showdata(double e[][INPUTSIZE][INPUTSIZE], int t[], int n_of_e)
	{
		int i, j, k;

		for (i = 0; i < n_of_e; ++i) {
			printf("N=%d category=%d\n", i, t[i]);
			for (j = 0; j < INPUTSIZE; ++j) {
				for (k = 0; k < INPUTSIZE; ++k) {
					printf("%.3lf ", e[i][j][k]);
				}
				printf("\n");
			}
			printf("\n");
		}
	}

	//�f�[�^�ǂݍ���
	static int getdata(double e[][INPUTSIZE][INPUTSIZE], int t[])
	{
		int i = 0, j = 0, k = 0;

		while (scanf("%d", &t[i]) != EOF) {
			while (scanf("%lf", &e[i][j][k]) != EOF) {
				++k;
				if (k >= INPUTSIZE) {
					k = 0;
					++j;
					if (j >= INPUTSIZE) {
						break;
					}
				}
			}
			j = 0; k = 0;
			++i;
		}

		return i;
	}

	//���ʂ̏o��
	static void print(double wh[HIDDENNO][POOLOUTSIZE*POOLOUTSIZE*FILTERNO + 1], double wo[HIDDENNO + 1])
	{
		int i, j;

		for (i = 0; i < HIDDENNO; ++i) {
			for (j = 0; j < POOLOUTSIZE*POOLOUTSIZE*FILTERNO + 1; ++j) {
				printf("%lf ", wh[i][j]);
			}
		}
		printf("\n");

		for (i = 0; i < HIDDENNO + 1; ++i) {
			printf("%lf ", wo[i]);
		}
		printf("\n");

	}





};
