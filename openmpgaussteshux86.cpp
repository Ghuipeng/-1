#include <iostream>

using namespace std;

int main()
{
    #pragma omp parallel num_threads(NUM_THREADS), private(i, j, k, tmp,myzero,first)
     for (int k = 0; k < M; k++) {
            myzero=true;
            bool* p = e[k];
            #pragma omp for
            for (int i = 0; i < N; i ++)//对于zero函数的并行实现
                if (p[i] != 0)
                    myzero = false;
            //不能同时更改ifzero的值，每次更改要上锁
            #pragma omp critical (critical1)
            if (!myzero) {
            ifzero = false;
            }//必须等待所有的线程都判断完是否为0之后ifzero
            //才是一个正确的值，在此之前所有线程自然需要等待
            #pragma omp barrier
            while (!ifzero){
                if (r[E[k] != NULL]) {
                    #pragma omp for
                    for (int j = 0; j < N; j ++) {
                        e[k][j]=e[k][j]^r[E[k]][j] ;
                    }
                    ifzero = 1;//重置ifzero，开启下一次的zero操作，这里是对所有的线程均赋值，也可
                    //以仿照allbarrier版本对0线程进行判断
                    #pragma omp barrier//这里好像因为线程只操作自己的数据看起来可
                    //以跳过，事实上是不可以的，因为不管是否采用对0线程判断的版本，
                    //直接继续下列操作都可能出现ifzro被置0后再次置1的情况
                    myzero=true;
                    bool* p = e[k];
                    #pragma omp for
                    for (int i = 0; i < N; i ++)//对于zero函数的并行实现
                        if (p[i] != 0)
                            myzero = false;
                    #pragma omp critical (critical2)
                    if (!myzero) {
                        ifzero = false;
                    }
                   #pragma omp barrier
                    first=N;
                    #pragma omp for
                    for (int j = 0; j < N; j ++) {//这是用来用来更新E[i]的相关操作，
                    //E[i]保存第i个被消元行的首项的位置，这是很重要的
                        if (e[k][j] != 0)
                        {
                        first = k;
                        break;
                        }
                    }
                    E[k] = N;
                    #pragma omp barrier//这里等待的原理同上
                    #pragma omp critical (critical3)
                    if (first < E[k])//每个线程各自比较，一旦发现小于则交换
                        E[k] = first;
                    #pragma omp barrier
                }
                else {
                    pthread_barrier_wait(&barrier_new);
                    #pragma omp critical (critical4)//如果没有对应的消元行，则更新自己为消元行，
                    //之后退出循环即可
                    if(r[E[k] == NULL])
                        r[E[k]] = new bool[N];
                    #pragma omp for
                    for (int j = 0; j < N; j++) {
                        r[E[k]][j] = e[k][j];
                    }
                break;
                }
            }
        }
    return 0;
}
