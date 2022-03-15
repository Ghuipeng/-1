#include <iostream>
#include<sys/time.h>
#include <stdlib.h>
#include<unistd.h>
using namespace std;


const int   N=1;

double a[N];

void inita(){
    for(int i=0;i<N;i++)
        a[i]=i;
}


int main(void)
{
   struct  timeval   start,end;
    inita();
    int sum=0;
    gettimeofday(&start,NULL);
    for(int time=0;time<10000;time++){
        for(int m = N; m > 1; m /= 2)
            for(int  i = 0; i < m / 2; i++)
                a[ i ] = a[i * 2] + a[i *2+1];
    }
     gettimeofday(&end,NULL);
     cout<<1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec<<"ms"<<endl;
    return 0;
}