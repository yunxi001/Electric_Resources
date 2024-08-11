#include<stdio.h>
#include<math.h>
int main()
{
	double m,p=6000,d=300000,r=0.01;
	m=log(p/(p-d*r))/log(1+r);
	printf("还清月数为：%f\n",m);
	return 0;
	
	
}
