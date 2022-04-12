#include <arm_neno.h>

#include<iostream>

#include <sys/time.h>

#include <stdlib.h>

#include <cstdint>

#include<unistd.h>

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

    float32x4_t t1, t2, t3, t4;

    int last ;
    if (n == 0)
        last = N;
    else
        last = N + n - 4;
    for (int k = 0; k < N; k++)

    {

        float tmp[4] = { matrix[k][k], matrix[k][k], matrix[k][k], matrix[k][k] };

        t1 = vld1q_f32(tmp);
        for (int j = last; j < N; j++) {
            matrix[k][j] = matrix[k][j] / tmp[0];
        }
        for (int j = last - 4; j >= k; j -= 4) //从后向前每次取四个

        {

            t2 =vld1q_f32(matrix[k] + j);

            t3 = vdivq_f32(t2, t1);//除法

            vst1q_f32(matrix[k] + j, t3);

        }
        for (int j = k; j % 4 != (last % 4); j++)

        {
             matrix[k][j] = matrix[k][j] / tmp[0];
         }

        for (int i = k + 1; i < N; i++)

        {

            float tmp[4] = { matrix[i][k], matrix[i][k], matrix[i][k], matrix[i][k] };

            t1 = vld1q_f32(tmp);

            for (int j = last; j < N; j++) {
                matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
            }
            for (int j = last - 4; j > k; j -= 4)

            {

                t2 = _vld1q_f32(matrix[i] + j);

                t3 = vld1q_f32(matrix[k] + j);

                t4 = vsubq_f32(t2, vmulq_f32(t1, t3)); //减法

                vst1q_f32(matrix[i] + j, t4);

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

   float32x4_t t1, t2, t3, t4;

    for (int k = 0; k < N; k++)

    {

        float tmp[4] = { matrix[k][k], matrix[k][k], matrix[k][k], matrix[k][k] };

        t1 = vld1q_f32(tmp);

        for (int j = N - 4; j >= k; j -= 4) //从后向前每次取四个

        {

            t2 = vld1q_f32(matrix[k] + j);

            t3 = vdivq_f32(t2, t1);//除法

             vst1q_f32(matrix[k] + j, t3);

        }

        for (int j = k; j % 4 != (N % 4); j++)

        {
             matrix[k][j] = matrix[k][j] / tmp[0];

        }

        for (int i = k + 1; i < N; i++)

        {

            float tmp[4] = { matrix[i][k], matrix[i][k], matrix[i][k], matrix[i][k] };

            t1 =  vld1q_f32(tmp);

            for (int j = N - 4; j > k; j -= 4)

            {

                t2 =  vld1q_f32(matrix[i] + j);

                t3 = vld1q_f32(matrix[k] + j);

                t4 = vsubq_f32(t2, vmulq_f32(t1, t3)); //减法

                vst1q_f32(matrix[i] + j, t4);

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
    struct  timeval   start,end;

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
    cout << endl << endl << "不使用SSE串行的高斯消去法" << endl;

    int y = (4 - (intptr_t(&matrix[0][0]) % 16) / 4) % 4;
    cout << y << endl;
    gettimeofday(&start,NULL);

    normal_gaosi(matrix);

    gettimeofday(&end,NULL);

     cout<<1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec<<"ms"<<endl;
     print(matrix);
      cout << endl << endl<< "使用SSE并行的laod高斯消去法" << endl;
     int x = (4 - (intptr_t(&matrix3[0][0]) % 16) / 4) % 4;
    cout << x << endl;

      gettimeofday(&start,NULL);

    SSE_gaosi(matrix3);

   gettimeofday(&end,NULL);

     cout<<1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec<<"ms"<<endl;
    print(matrix3);
    cout << endl << endl << "使用SSE并行loadu的高斯消去法" << endl;

    int n = (4 - (intptr_t(&matrix2[0][0]) % 16) / 4) % 4;
    cout << n << endl;
     gettimeofday(&start,NULL);

    SSE_gaosi(matrix2, 1);

    gettimeofday(&end,NULL);

    cout<<1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec<<"ms"<<endl;
    print(matrix2);
    return 0;

}
