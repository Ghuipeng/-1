#include<iostream>
#include <windows.h>

#include <stdlib.h>

#include <pthread.h>
#include <semaphore.h>
#define RANDOM_ADD 2
using namespace std;
#define N  2048
#define NUM_THREAD 4
typedef struct {
	int t_id; // �߳� id
}threadParam_t;
sem_t sem_leader;
sem_t sem_Division[NUM_THREAD - 1];
sem_t sem_Elimination[NUM_THREAD - 1];
float** M;
void* threadFunc_Sta2(void* param) {
	threadParam_t* p = (threadParam_t*)param;
	int t_id = p->t_id; //�̱߳��
	for (int k = 0; k < N; k++) {
		//t_idΪ0���߳�������
		if (t_id == 0) {
			for (int j = k + 1; j < N; j++)
				M[k][j] = M[k][j] / M[k][k];
			M[k][k] = 1;
		}
		else
			sem_wait(&sem_Division[t_id - 1]);
		//���߳�0�����������߳�
		if (t_id == 0)
			for (int i = 0; i < NUM_THREAD - 1; ++i)
				sem_post(&sem_Division[i]);


		//ѭ����������
		for (int i = k + 1 + t_id; i < N; i += NUM_THREAD) {
			for (int j = k + 1; j < N; ++j)
				M[i][j] = M[i][j] - M[i][k] * M[k][j];
			M[i][k] = 0;
		}
		if (t_id == 0) {
			for (int i = 0; i < NUM_THREAD - 1; i++)
				sem_wait(&sem_leader);
			for (int i = 0; i < NUM_THREAD - 1; i++)
				sem_post(&sem_Elimination[i]);
		}
		else {
			sem_post(&sem_leader);
			sem_wait(&sem_Elimination[t_id - 1]);
		}

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
	threadParam_t param[NUM_THREAD];//���ǲ���
	sem_init(&sem_leader, 0, 0);
	for (int i = 0; i < NUM_THREAD - 1; ++i) {
		sem_init(&sem_Division[i], 0, 0);
		sem_init(&sem_Elimination[i], 0, 0);
	}
	cout<<2<<endl;
	cout << N << endl;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	QueryPerformanceCounter((LARGE_INTEGER*)&head);
	for (int t_id = 0; t_id < NUM_THREAD; ++t_id) {
		param[t_id].t_id = t_id;
		pthread_create(&thread_handles[t_id], NULL, threadFunc_Sta2, (void*)&param[t_id]);//����ǿ��ת������
	}
	for (int t_id = 0; t_id < NUM_THREAD; ++t_id)
		pthread_join(thread_handles[t_id], NULL);
	QueryPerformanceCounter((LARGE_INTEGER*)&tail);

	cout << (tail - head) * 1000.0 / freq << "ms" << endl;
	sem_destroy(&sem_leader);
	for (int i = 0; i < NUM_THREAD; ++i) {
		sem_destroy(&sem_Division[i]);
		sem_destroy(&sem_Elimination[i]);
	}
}
