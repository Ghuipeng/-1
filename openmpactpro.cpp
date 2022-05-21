#include <iostream>
#include <omp.h>
#include <sys/time.h>
#include <stdlib.h>
#include <cstdint>
#include<unistd.h>
using namespace std;
#define N 8192
#define NUM_THREADS 4
#define RANDOM_ADD 2
float** M;
int main()
{
    cout<<1<<endl;
   struct  timeval   start,end;
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
	int k,i,j;
	double tmp;
	 gettimeofday(&start,NULL);
	 for( k = 1; k < N; ++k){
        // 串行部分，也可以尝试并行化
        {
            tmp = M[k][k];
            for(int j = k + 1; j < N; ++j){
                M[k][j] = M[k][j] / tmp;
            }
            M[k][k] = 1.0;
        }
        // 并行部分，使用行划分
        #pragma omp parallel for num_threads(NUM_THREADS), private(i, j, k, tmp)
        for( i = k + 1; i < N; ++i){
            tmp = M[i][k];
            for(int j = k + 1; j < N; ++j){
                M[i][j] = M[i][j] - tmp * M[k][j];
            }
            M[i][k] = 0.0;
        }
        // 离开for循环时，各个线程默认同步，进入下一行的处理
    }
    gettimeofday(&end,NULL);
    cout<<1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec<<"ms"<<endl;
    return 0;
}

