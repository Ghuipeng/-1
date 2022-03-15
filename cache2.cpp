#include <iostream>
#include<sys/time.h>
#include <stdlib.h>
#include<unistd.h>
using namespace std;


const int   N=1;

double a[N],b[N][N],sum[N];

void inita(){
    for(int i=0;i<N;i++)
        a[i]=i;
}
void initb(){
	for(int i=0;i<N;i++)
		for(int j=0;j<N;j++)
			b[i][j]=i+j;
}

int main(void)
{
   struct  timeval   start,end;
    inita();
    initb();
    gettimeofday(&start,NULL);
for(int time=0;time<10000;time++){
   for(int  i = 0; i < N; i++){
	sum[i] = 0.0;
       for(j = 0; j < N; j++)
             sum[i] += b[j][i] * a[j];
	}
  }
     gettimeofday(&end,NULL);
     cout<<1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec<<"ms"<<endl;
    return 0;
}
