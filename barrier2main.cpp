#include<iostream>
#include <windows.h>

#include <stdlib.h>

#include <pthread.h>
#include <semaphore.h>
#define RANDOM_ADD 2
using namespace std;
#define N 2048
#define NUM_THREAD 4
typedef struct {
	int t_id; // 线程 id
}threadParam_t;
pthread_barrier_t barrier_Division;
pthread_barrier_t barrier_Elimination;
float** M;
void* threadFunc_Sta2(void* param) {
	threadParam_t* p = (threadParam_t*)param;
	int t_id = p->t_id; //线程编号
	for (int k = 0; k < N; k++) {
		//t_id为0的线程做除法

			for (int j = k + 1+t_id; j < N; j+=NUM_THREAD)
				M[k][j] = M[k][j] / M[k][k];
			M[k][k] = 1;

		//第一个同步点,barrier的作用是所有来到这个地方的线程在barrier的线程数未满时都要做等待工作
		pthread_barrier_wait(&barrier_Division);


		//循环划分任务
		for (int i = k + 1 + t_id; i < N; i += NUM_THREAD) {
			for (int j = k + 1; j < N; ++j)
				M[i][j] = M[i][j] - M[i][k] * M[k][j];
			M[i][k] = 0;
		}
		//第二个同步点,等待所有的行都做完减法之后才能继续下一行
		pthread_barrier_wait(&barrier_Elimination);

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
	pthread_t thread_handles[NUM_THREAD];
	threadParam_t param[NUM_THREAD];//这是参数
	pthread_barrier_init(&barrier_Division, NULL, NUM_THREAD);
	pthread_barrier_init(&barrier_Elimination, NULL, NUM_THREAD);//屏障可限制的线程数为NUM_THREAD
	cout<<5<<endl;
	cout<<N<<endl;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	QueryPerformanceCounter((LARGE_INTEGER*)&head);
	for (int t_id = 0; t_id < NUM_THREAD; ++t_id) {
		param[t_id].t_id = t_id;
		pthread_create(&thread_handles[t_id], NULL, threadFunc_Sta2, (void*)&param[t_id]);//参数强制转换类型
	}
	for (int t_id = 0; t_id < NUM_THREAD; ++t_id)
		pthread_join(thread_handles[t_id], NULL);
	QueryPerformanceCounter((LARGE_INTEGER*)&tail);

	cout << (tail - head) * 1000.0 / freq << "ms" << endl;
	pthread_barrier_destroy(&barrier_Division);
	pthread_barrier_destroy(&barrier_Elimination);
}
