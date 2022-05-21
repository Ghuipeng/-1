#include <iostream>

using namespace std;

int main()
{
    #pragma omp parallel num_threads(NUM_THREADS), private(i, j, k, tmp,myzero,first)
     for (int k = 0; k < M; k++) {
            myzero=true;
            bool* p = e[k];
            #pragma omp for
            for (int i = 0; i < N; i ++)//����zero�����Ĳ���ʵ��
                if (p[i] != 0)
                    myzero = false;
            //����ͬʱ����ifzero��ֵ��ÿ�θ���Ҫ����
            #pragma omp critical (critical1)
            if (!myzero) {
            ifzero = false;
            }//����ȴ����е��̶߳��ж����Ƿ�Ϊ0֮��ifzero
            //����һ����ȷ��ֵ���ڴ�֮ǰ�����߳���Ȼ��Ҫ�ȴ�
            #pragma omp barrier
            while (!ifzero){
                if (r[E[k] != NULL]) {
                    #pragma omp for
                    for (int j = 0; j < N; j ++) {
                        e[k][j]=e[k][j]^r[E[k]][j] ;
                    }
                    ifzero = 1;//����ifzero��������һ�ε�zero�����������Ƕ����е��߳̾���ֵ��Ҳ��
                    //�Է���allbarrier�汾��0�߳̽����ж�
                    #pragma omp barrier//���������Ϊ�߳�ֻ�����Լ������ݿ�������
                    //����������ʵ���ǲ����Եģ���Ϊ�����Ƿ���ö�0�߳��жϵİ汾��
                    //ֱ�Ӽ������в��������ܳ���ifzro����0���ٴ���1�����
                    myzero=true;
                    bool* p = e[k];
                    #pragma omp for
                    for (int i = 0; i < N; i ++)//����zero�����Ĳ���ʵ��
                        if (p[i] != 0)
                            myzero = false;
                    #pragma omp critical (critical2)
                    if (!myzero) {
                        ifzero = false;
                    }
                   #pragma omp barrier
                    first=N;
                    #pragma omp for
                    for (int j = 0; j < N; j ++) {//����������������E[i]����ز�����
                    //E[i]�����i������Ԫ�е������λ�ã����Ǻ���Ҫ��
                        if (e[k][j] != 0)
                        {
                        first = k;
                        break;
                        }
                    }
                    E[k] = N;
                    #pragma omp barrier//����ȴ���ԭ��ͬ��
                    #pragma omp critical (critical3)
                    if (first < E[k])//ÿ���̸߳��ԱȽϣ�һ������С���򽻻�
                        E[k] = first;
                    #pragma omp barrier
                }
                else {
                    pthread_barrier_wait(&barrier_new);
                    #pragma omp critical (critical4)//���û�ж�Ӧ����Ԫ�У�������Լ�Ϊ��Ԫ�У�
                    //֮���˳�ѭ������
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
