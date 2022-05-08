#include<iostream>
#include <windows.h>

#include <stdlib.h>

#include <pthread.h>
#include <semaphore.h>
#define RANDOM_ADD 2
using namespace std;
#define N 4096
#define NUM_THREAD 4
typedef struct {
	int t_id; // 线程 id
}threadParam_t;
sem_t sem_main;
sem_t sem_workstart[NUM_THREAD ];
sem_t sem_workend[NUM_THREAD ];
float** M;
void* threadFunc_Sta2(void* param) {
	threadParam_t* p = (threadParam_t*)param;
	int t_id = p->t_id; //线程编号
	for (int k = 0; k < N; k++) {
		sem_wait(&sem_workstart[t_id]);
		//循环划分任务
		for (int i = k + 1 + t_id; i < N; i += NUM_THREAD) {
			for (int j = k + 1; j < N; ++j)
				M[i][j] = M[i][j] - M[i][k] * M[k][j];
			M[i][k] = 0;
		}
		sem_post(&sem_main);
		sem_wait(&sem_workend[t_id]);
	}
	pthread_exit(NULL);
}
int main() {
	long long head, tail, freq;
	M = new float* [N];
	for (int i = 0; i < N; i++) {
		M[i] = new float[N];
	}
	for (int i = 0; i < N; i++) {
		M[i] = new float[N];
		for (int j = 0; j < i; j++)
			M[i][j] = 0;

		for (int j = i; j < N; j++)
			M[i][j] = rand() % 50;
	}
	for (int k = 0; k < RANDOM_ADD; k++) {
		for (int i = 0; i < N; i++) {
			int temp = (k * 2 + i * 3) % N;
			for (int j = 0; j < N; j++)
				M[temp][j] += M[i][j];
		}
	}
	sem_init(&sem_main, 0, 0);
	for (int i = 0; i < NUM_THREAD ; ++i) {
		sem_init(&sem_workstart[i], 0, 0);
		sem_init(&sem_workend[i], 0, 0);
	}
	pthread_t thread_handles[NUM_THREAD];
	threadParam_t param[NUM_THREAD];//这是参数
	cout << 4 << endl;
	cout << N << endl;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	QueryPerformanceCounter((LARGE_INTEGER*)&head);
	for (int t_id = 0; t_id < NUM_THREAD; ++t_id) {
		param[t_id].t_id = t_id;
		pthread_create(&thread_handles[t_id], NULL, threadFunc_Sta2, (void*)&param[t_id]);//参数强制转换类型
	}
	for (int k = 0; k < N; k++) {
		for (int j = k + 1; j < N; j++) {
			M[k][j] = M[k][j] / M[k][k];
		}
		M[k][k] = 1.0;
		for (int t_id = 0; t_id < NUM_THREAD; t_id++) {
			sem_post(&sem_workstart[t_id]);
		}
		for (int t_id = 0; t_id < NUM_THREAD; t_id++) {
			sem_wait(&sem_main);
		}
		for (int t_id = 0; t_id < NUM_THREAD; t_id++) {
			sem_post(&sem_workend[t_id]);
		}
	}
	for (int t_id = 0; t_id < NUM_THREAD; ++t_id)
		pthread_join(thread_handles[t_id], NULL);
	QueryPerformanceCounter((LARGE_INTEGER*)&tail);

	cout << (tail - head) * 1000.0 / freq << "ms" << endl;
	sem_destroy(&sem_main);
	for (int i = 0; i < NUM_THREAD; ++i) {
		sem_destroy(&sem_workstart[i]);
		sem_destroy(&sem_workend[i]);
	}
}
