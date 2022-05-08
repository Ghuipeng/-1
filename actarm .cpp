#include<iostream>
#include <sys/time.h>
#include<unistd.h>
#include <stdlib.h>
#include <cstdint>
#include <pthread.h>
#include <semaphore.h>
#define RANDOM_ADD 2
using namespace std;
#define N 8192
#define NUM_THREAD 4
typedef struct {
	int num[NUM_THREAD];
	int k;//消去的次数
	int t_id; // 线程 id
}threadParam_t;
float** M;
void* threadFunc_Sta2(void* param) {
	//这个版本是不需要信号量同步的，线程是动态创建的，每次执行完自己的行之后就会结束
	threadParam_t* p = (threadParam_t*)param;
	int num[NUM_THREAD];
	for (int j = 0; j < NUM_THREAD; j++) {
		num[j] = p->num[j];
	}
	int k = p->k;
	int t_id = p->t_id; //线程编号
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
gettimeofday(&start,NULL);
	for (int k = 0; k < N; k++) {
		for (int j = k + 1; j < N; j++) {
			M[k][j] = M[k][j] / M[k][k];
		}
		M[k][k] = 1;
		pthread_t thread_handles[NUM_THREAD];
		threadParam_t param[NUM_THREAD];//这是参数
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
			pthread_create(&thread_handles[t_id], NULL, threadFunc_Sta2, (void*)&param[t_id]);//参数强制转换类型
		}
		for (int t_id = 0; t_id < NUM_THREAD; ++t_id)
			pthread_join(thread_handles[t_id], NULL);
	}
	cout << N << endl;
	 gettimeofday(&end,NULL);

	 cout<<1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec<<"ms"<<endl;
}
