#include<pmmintrin.h>

#include<xmmintrin.h>

#include<iostream>

#include <windows.h>

#include <stdlib.h>
using namespace std;
#define N 4096
bool zero(bool* p, int n) {
	for (int i = 0; i < n; i++)
		if (p[i] != 0)
			return false;
	return true;
}

void SSE_guass(int n, int m, bool** e, bool** r, bool* E) {
	__m128 t1, t2, t3;
	for (int i = 0; i < m; i++) {
		while (!zero(e[i], n)) {
			if (r[E[i]] != NULL)
			{
				bool* p = r[E[i]];
				for (int j = 0; j < n; j+=16) {
					t1 = _mm_load_ps((float*)(p+j));
					t2 = _mm_load_ps( (float*)(e[i]+ j));
					_mm_store_ps((float*)(e[i] + j), _mm_xor_ps(t1, t2));
				}
			}
			else
			{
				for (int j = 0; j < n; j+=16) {
					t3= _mm_load_ps((float*)(e[i] + j));
					_mm_store_ps((float*)(r[E[i]+j]), _mm_xor_ps(t1, t2));
				}
				break;
			}
		}
	}

}
void SSE_guassu(int n, int m, bool** e, bool** r, bool* E,int offest) {
	__m128 t1, t2, t3;
	for (int i = 0; i < m; i++) {
		while (!zero(e[i], n)) {
			if (r[E[i]] != NULL)
			{
				bool* p = r[E[i]];
				int end = n + offest - 4;
				for (int j = end; i < n; j++) {
					e[i][j] = e[i][j] ^ p[j];
				}
				for (int j = offest; j < end; j += 16) {
					t1 = _mm_loadu_ps((float*)(p + j));
					t2 = _mm_loadu_ps((float*)(e[i] + j));
					_mm_storeu_ps((float*)(e[i] + j), _mm_xor_ps(t1, t2));
				}
				for (int j = 0; j < offest; j++) {
					e[i][j] = e[i][j] ^ p[j];
				}
			}
			else
			{
				int end = n + offest - 4;
				for (int j = end; i < n; j++) {
					r[E[i]][j] = e[i][j];
				}
				for (int j = offest; j < end; j += 16) {
					t3 = _mm_loadu_ps((float*)(e[i] + j));
					_mm_storeu_ps((float*)(r[E[i] + j]), _mm_xor_ps(t1, t2));
				}
				for (int j = end; i < n; j++) {
					r[E[i]][j] = e[i][j];
				}
				break;
			}
		}
	}

}
//E[i]表示第i个被消元行的首项,处于动态更新之中
//n为列数
//被消元行的数目
//e是被消元行的数组表示
//r是消元子的数组标识
//注意到在r中第i行首项为i
void guass(int n,int m,bool** e,bool** r,bool *E) {
	for (int i = 0; i < m; i++) {
		while (!zero(e[i],n)) {
			if (r[E[i]] != NULL)
			{	
				bool* p = r[E[i]];
				for (int j = 0; j < n; j++) {
					e[i][j] = e[i][j] ^ p[j];
				}
			}
			else
			{
				for (int j = 0; j < n; j++) {
					r[E[i]][j] = e[i][j];
				}
				break;
			}
		}
	}
}
int nain() {
	

}