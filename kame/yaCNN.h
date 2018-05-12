#pragma once

#pragma once

#include "yaCommon.h"
#include "yaLearnBase.h"

#define INPUTSIZE 11 //入力数
#define FILTERSIZE 3 //フィルタサイズ
#define FILTERNO 2 //フィルタ個数
#define POOLSIZE 3 //プーリングサイズ
#define POOLOUTSIZE 3 //プーリングの出力サイズ
#define HIDDENNO 3 //中間層のセル数
#define ALPHA 10 //学習係数
#define MAXINPUTNO 100 //学習データの最大個数
#define BIGNUM 100 //誤差の初期値
#define LIMIT 0.001 //誤差の上限値

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
		//double e[MAXINPUTNO][INPUTSIZE][INPUTSIZE];//入力データ
		//int t[MAXINPUTNO];//教師データ
		//int n_of_e;//学習データの個数

		double filter[FILTERNO][FILTERSIZE][FILTERSIZE];//フィルタ
		double convout[INPUTSIZE][INPUTSIZE] = { 0 };//畳込み出力
		double poolout[POOLOUTSIZE][POOLOUTSIZE];//出力データ
		int i, j, m, n;
		double err = BIGNUM;//誤差の評価
		int count = 0;
		double ef[POOLOUTSIZE*POOLOUTSIZE*FILTERNO + 1];//全結合層への入力データ、+1は教師データ用
		double o;//最終出力
		double hi[HIDDENNO + 1];//中間層の出力
		double wh[HIDDENNO][POOLOUTSIZE*POOLOUTSIZE*FILTERNO + 1];//中間層の重み
		double wo[HIDDENNO + 1];//出力層の重み

		srand(SEED);

		//フィルタ初期化
		initfilter(filter);

		//重み初期化
		initwh(wh);
		initwo(wo);
		print(wh, wo);

		//データ入力
		//n_of_e = getdata(e, t);
		showdata(e, t, n_of_e);

		//学習
		while (err > LIMIT) {
			err = 0.0;
			for (i = 0; i < n_of_e; ++i) {//学習データごとに繰り返し
				for (j = 0; j < FILTERNO; ++j) {//フィルタごとに繰り返し
					conv(filter[j], e[i], convout);//畳込み
					pool(convout, poolout);//プーリング

					//全結合層へコピー
					for (m = 0; m < POOLOUTSIZE; ++m) {
						for (n = 0; n < POOLOUTSIZE; ++n) {
							ef[j*POOLOUTSIZE*POOLOUTSIZE + POOLOUTSIZE*m + n] = poolout[m][n];
						}
					}
					ef[POOLOUTSIZE*POOLOUTSIZE*FILTERNO] = t[i];//教師データ
				}

				//順方向の計算
				o = forward(wh, wo, hi, ef);
				//出力層の重み調整
				olearn(wo, hi, ef, o);
				//中間層の重み調整
				hlearn(wh, wo, hi, ef, o);
				//誤差の積算
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

	//中間層の重み調整
	static void hlearn(double wh[HIDDENNO][POOLOUTSIZE*POOLOUTSIZE*FILTERNO + 1], double wo[HIDDENNO + 1], double hi[], double e[POOLOUTSIZE*POOLOUTSIZE*FILTERNO + 1], double o)
	{
		int i, j;
		double dj;

		for (j = 0; j < HIDDENNO; ++j) {
			dj = hi[j] * (1 - hi[j])*wo[j] * (e[POOLOUTSIZE*POOLOUTSIZE*FILTERNO]/*教師データ*/ - o/*出力*/)*o*(1 - o);
			for (i = 0; i < POOLOUTSIZE*POOLOUTSIZE*FILTERNO; ++i) {
				wh[j][i] += ALPHA*e[i] * dj;
			}
			wh[j][i] += ALPHA*(-1)*dj;
		}
	}

	//出力層の重み調整
	static void olearn(double wo[HIDDENNO + 1], double hi[], double e[POOLOUTSIZE*POOLOUTSIZE*FILTERNO + 1], double o)
	{
		int i;
		double d;

		d = (e[POOLOUTSIZE*POOLOUTSIZE*FILTERNO]/*教師データ*/ - o/*出力*/)*o*(1 - o);

		for (i = 0; i < HIDDENNO; ++i) {
			wo[i] += ALPHA*hi[i] * d;
		}
		wo[i] += ALPHA*(-1)*d;
	}

	//順方向の計算
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

	//中間層の重み初期化
	static void initwh(double wh[HIDDENNO][POOLOUTSIZE*POOLOUTSIZE*FILTERNO + 1])
	{
		int i, j;

		for (i = 0; i < HIDDENNO; ++i) {
			for (j = 0; j < POOLOUTSIZE*POOLOUTSIZE*FILTERNO + 1; ++j) {
				wh[i][j] = yaLearnBase::drnd();
			}
		}
	}

	//出力層の重み初期化
	static void initwo(double wo[HIDDENNO + 1])
	{
		int i;

		for (i = 0; i < HIDDENNO + 1; ++i) {
			wo[i] = yaLearnBase::drnd();
		}
	}

	//フィルタの初期化
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

	//プーリングの計算
	static void pool(double convout[][INPUTSIZE], double poolout[][POOLOUTSIZE])
	{
		int i, j;

		for (i = 0; i < POOLOUTSIZE; ++i) {
			for (j = 0; j < POOLOUTSIZE; ++j) {
				poolout[i][j] = maxpooling(convout, i, j);
			}
		}
	}

	//最大値プーリング
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

	//畳込みの計算
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

	//フィルタの適用
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

	//伝達関数
	static double f(double u)
	{
		return(1.0 / (1.0 + exp(-u)));
	}

	//データ表示
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

	//データ読み込み
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

	//結果の出力
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
