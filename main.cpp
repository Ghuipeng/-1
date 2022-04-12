#include<pmmintrin.h>

#include<xmmintrin.h>

#include<iostream>

#include <windows.h>

#include <stdlib.h>

#include <cstdint>

#define N 8192



using namespace std;



void normal_gaosi(float matrix[N][N]) //没加SSE串行的高斯消去法

{

    for (int k = 0; k < N; k++)

    {

        float tmp = matrix[k][k];

        for (int j = k; j < N; j++)

        {

            matrix[k][j] = matrix[k][j] / tmp;

        }

        for (int i = k + 1; i < N; i++)

        {

            float tmp2 = matrix[i][k];

            for (int j = k + 1; j < N; j++)

            {

                matrix[i][j] = matrix[i][j] - tmp2 * matrix[k][j];

            }

            matrix[i][k] = 0;

        }

    }

}



void SSE_gaosi(float matrix[N][N],int n) //加了SSE并行的高斯消去法

{

    __m128 t1, t2, t3, t4;

    int last ;
    if (n == 0)
        last = N;
    else
        last = N + n - 4;
    for (int k = 0; k < N; k++)

    {

        float tmp[4] = { matrix[k][k], matrix[k][k], matrix[k][k], matrix[k][k] };

        t1 = _mm_loadu_ps(tmp);
        for (int j = last; j < N; j++) {
            matrix[k][j] = matrix[k][j] / tmp[0];
        }
        for (int j = last - 4; j >= k; j -= 4) //从后向前每次取四个

        {

            t2 = _mm_loadu_ps(matrix[k] + j);

            t3 = _mm_div_ps(t2, t1);//除法

            _mm_storeu_ps(matrix[k] + j, t3);

        }
        for (int j = k; j % 4 != (last % 4); j++)

        {
             matrix[k][j] = matrix[k][j] / tmp[0];
         }

        for (int i = k + 1; i < N; i++)

        {

            float tmp[4] = { matrix[i][k], matrix[i][k], matrix[i][k], matrix[i][k] };

            t1 = _mm_loadu_ps(tmp);

            for (int j = last; j < N; j++) {
                matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
            }
            for (int j = last - 4; j > k; j -= 4)

            {

                t2 = _mm_loadu_ps(matrix[i] + j);

                t3 = _mm_loadu_ps(matrix[k] + j);

                t4 = _mm_sub_ps(t2, _mm_mul_ps(t1, t3)); //减法

                _mm_storeu_ps(matrix[i] + j, t4);

            }

            for (int j = k + 1; j % 4 != (last % 4); j++)

            {

                matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];

            }

            matrix[i][k] = 0;

        }

    }


}
void SSE_gaosi(float matrix[N][N]) //加了SSE并行的高斯消去法

{

    __m128 t1, t2, t3, t4;

    for (int k = 0; k < N; k++)

    {

        float tmp[4] = { matrix[k][k], matrix[k][k], matrix[k][k], matrix[k][k] };

        t1 = _mm_loadu_ps(tmp);

        for (int j = N - 4; j >= k; j -= 4) //从后向前每次取四个

        {

            t2 = _mm_load_ps(matrix[k] + j);

            t3 = _mm_div_ps(t2, t1);//除法

            _mm_store_ps(matrix[k] + j, t3);

        }

        for (int j = k; j % 4 != (N % 4); j++)

        {
             matrix[k][j] = matrix[k][j] / tmp[0];

        }

        for (int i = k + 1; i < N; i++)

        {

            float tmp[4] = { matrix[i][k], matrix[i][k], matrix[i][k], matrix[i][k] };

            t1 = _mm_loadu_ps(tmp);

            for (int j = N - 4; j > k; j -= 4)

            {

                t2 = _mm_load_ps(matrix[i] + j);

                t3 = _mm_load_ps(matrix[k] + j);

                t4 = _mm_sub_ps(t2, _mm_mul_ps(t1, t3)); //减法

                _mm_store_ps(matrix[i] + j, t4);

            }

            for (int j = k + 1; j % 4 != (N % 4); j++)

            {

                matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];

            }

            matrix[i][k] = 0;

        }

    }

}


void print(float matrix[N][N]) //输出

{



        for (int j = 0; j < 20; j++)

        {

            cout << matrix[1][j] << " ";

        }

        cout << endl;



}

float matrix[N][N];
float matrix2[N][N];
float matrix3[N][N];
int main()

{
    long long head, tail, freq, head2, tail2, freq2,head3,tail3,freq3;
    //cout << "我们生成了初始随机矩阵" << endl
    for (int i = 0; i < N; i++)

    {

        for (int j = 0; j < N; j++)

        {

            matrix[i][j] = rand() % 100;

            matrix2[i][j] = matrix[i][j];

            matrix3[i][j] = matrix[i][j];
        }

    }
    print(matrix);
    print(matrix2);
    print(matrix3);
    cout << endl << endl << endl << "不使用SSE串行的高斯消去法" << endl;

    int y = (4 - (intptr_t(&matrix[0][0]) % 16) / 4) % 4;
    cout << y << endl;
    QueryPerformanceFrequency((LARGE_INTEGER *) &freq);

    QueryPerformanceCounter((LARGE_INTEGER *) &head);

    normal_gaosi(matrix);

    QueryPerformanceCounter((LARGE_INTEGER *) & tail);

    cout << (tail - head) * 1000.0 / freq << "ms" << endl;
     print(matrix);
    cout << endl << endl << endl << "使用SSE并行的laod高斯消去法" << endl;
    int x = (4 - (intptr_t(&matrix3[0][0]) % 16) / 4) % 4;
    cout << x << endl;

    QueryPerformanceFrequency((LARGE_INTEGER*)&freq3);

    QueryPerformanceCounter((LARGE_INTEGER*)&head3);

    SSE_gaosi(matrix3);

    QueryPerformanceCounter((LARGE_INTEGER*)&tail3);

    cout << (tail3 - head3) * 1000.0 / freq3 << "ms" << endl;
    print(matrix3);
    cout << endl << endl << endl << "使用SSE并行loadu的高斯消去法" << endl;

    int n = (4 - (intptr_t(&matrix2[0][0]) % 16) / 4) % 4;
    cout << n << endl;
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq2);

    QueryPerformanceCounter((LARGE_INTEGER*)&head2);

    SSE_gaosi(matrix2, 1);

    QueryPerformanceCounter((LARGE_INTEGER*)&tail2);

    cout << (tail2 - head2) * 1000.0 / freq2 << "ms" << endl;
    print(matrix2);
    return 0;

}
