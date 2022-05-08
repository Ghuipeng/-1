#include<iostream>
#include <windows.h>

#include <stdlib.h>

#include <pthread.h>
#include <semaphore.h>
#define RANDOM_ADD 2
using namespace std;
#define N 8192
#define NUM_THREAD 4
typedef struct {
	int num[NUM_THREAD];
	int k;//��ȥ�Ĵ���
	int t_id; // �߳� id
}threadParam_t;
float** M;
void* threadFunc_Sta2(void* param) {
	//����汾�ǲ���Ҫ�ź���ͬ���ģ��߳��Ƕ�̬�����ģ�ÿ��ִ�����Լ�����֮��ͻ����
	threadParam_t* p = (threadParam_t*)param;
	int num[NUM_THREAD];
	for (int j = 0; j < NUM_THREAD; j++) {
		num[j] = p->num[j];
	}
	int k = p->k;
	int t_id = p->t_id; //�̱߳��
	int i = k + 1;
	for (int j = 0; j < t_id; j++) {
		i = i + num[j];
	}
	int sum = i+num[t_id];
	for (; i < sum; i++) {
		for (int j = k + 1; j < N; ++j) {
			M[i][j] = M[i][j] - M[i][k] * M[k][j];
		}
		M[i][k] = 0;
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
	cout<<3<<endl;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	QueryPerformanceCounter((LARGE_INTEGER*)&head);
	for (int k = 0; k < N; k++) {
		for (int j = k + 1; j < N; j++) {
			M[k][j] = M[k][j] / M[k][k];
		}
		M[k][k] = 1;
		pthread_t thread_handles[NUM_THREAD];
		threadParam_t param[NUM_THREAD];//���ǲ���
		int sum = N - (k + 1);
		int thenum = sum / NUM_THREAD;
		int buzu = sum % NUM_THREAD;
		int num[NUM_THREAD];
		for (int i = 0; i < NUM_THREAD; i++) {
			num[i] = thenum;
		}
		int i = 0;
		while(buzu != 0) {
			num[i]++;
			buzu--;
			i++;
		}
		for (int t_id = 0; t_id < NUM_THREAD; ++t_id) {
			for (int i = 0; i < NUM_THREAD; i++) {
				param[t_id].num[i] = num[i];
			}
			param[t_id].k = k;
			param[t_id].t_id = t_id;
			pthread_create(&thread_handles[t_id], NULL, threadFunc_Sta2, (void*)&param[t_id]);//����ǿ��ת������
		}
		for (int t_id = 0; t_id < NUM_THREAD; ++t_id)
			pthread_join(thread_handles[t_id], NULL);
	}
	cout << N << endl;
	QueryPerformanceCounter((LARGE_INTEGER*)&tail);

	cout << (tail - head) * 1000.0 / freq << "ms" << endl;
}
