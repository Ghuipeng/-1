#include<iostream>
#include <windows.h>
#include <stdlib.h>
#include <pthread.h>
#include  <semaphore.h>
#define NUM_THREAD 16
using namespace std;
typedef struct {
	int t_id; // 线程 id
}threadParam_t;
int M;
int N;
bool** e;
bool** r;
bool* E;
bool ifzero;
pthread_barrier_t barrier_new;
pthread_barrier_t barrier_ifzero;
pthread_barrier_t barrier_zero;
pthread_barrier_t barrier_changefirst;
pthread_mutex_t mutex_changefirst;
pthread_mutex_t mutex_zero;
pthread_mutex_t mutex_new;
bool zero(bool* p, int n) {
	for (int i = 0; i < n; i++)
		if (p[i] != 0)
			return false;
	return true;
}
void* threadgauss(void* param) {
	threadParam_t* p = (threadParam_t*)param;
	int t_id = p->t_id; //线程编号
	for (int k = 0; k < M; k++) {
		bool myzero=true;
		bool* p = e[k];
		for (int i = t_id; i < N; i += NUM_THREAD)
			if (p[i] != 0)
				myzero = false;
		pthread_mutex_lock(&mutex_zero);
		if (!myzero) {
			ifzero = false;
		}
		pthread_mutex_unlock(&mutex_zero);
		pthread_barrier_wait(&barrier_ifzero);
		while (!ifzero){
			if (r[E[k] != NULL]) {
				for (int j = t_id; j < N; j += NUM_THREAD) {
					e[k][j]=e[k][j]^r[E[k]][j] ;
				}
				ifzero = 1;
				pthread_barrier_wait(&barrier_ifzero);
				bool themyzero=true;
				bool* p = e[k];
				for (int i = t_id; i < N; i += NUM_THREAD)
					if (p[i] != 0)
						themyzero = false;
				pthread_mutex_lock(&mutex_zero);
				if (!themyzero) {
					ifzero = false;
				}
				pthread_mutex_unlock(&mutex_zero);
				pthread_barrier_wait(&barrier_ifzero);
				int first=N;
				for (int j = t_id; j < N; j += NUM_THREAD) {
					if (e[k][j] != 0)
					{
						first = k;
						break;
					}
				}
				E[k] = N;
				pthread_barrier_wait(&barrier_changefirst);
				pthread_mutex_lock(&mutex_changefirst);
				if (first < E[k])
					E[k] = first;
				pthread_mutex_unlock(&mutex_changefirst);
				pthread_barrier_wait(&barrier_changefirst);
			}
			else {
				pthread_barrier_wait(&barrier_new);
				pthread_mutex_lock(&mutex_new);
				if(r[E[k] == NULL])
					r[E[k]] = new bool[N];
				pthread_mutex_unlock(&mutex_new);
				for (int j = t_id; j < N; j+=NUM_THREAD) {
					r[E[k]][j] = e[k][j];
				}
				break;
			}
		}
	}
}