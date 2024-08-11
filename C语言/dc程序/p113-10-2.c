#include<stdio.h>
int main()
{
	double i,w,a=0.1,b=0.075,c=0.05,d=0.03,e=0.015,f=0.01,m=100000.0;
	int n;
	printf("请输入当月利润：");
	scanf("%lf",&i);
	n=i/100000;
	switch(n)
	{
		case 0:w=i*a;break;
		case 1:
		case 2:w=m*a+(i-m)*b ;break;
		case 3:
		case 4:w=m*a+m*b+(i-2*m)*c;break; 
		case 5:
		case 6:w=m*a+m*b+2*m*c+(i-4*m)*d;break;
		case 7:
		case 8:
		case 9:
		case 10:w=m*a+m*b+2*m*c+2*m*d+(i-6*m)*e;break;
		default:w=m*a+m*b+2*m*c+2*m*d+4*m*e+(i-10*m)*f;
	}
	printf("应发奖金为：%f",w);
	return 0;
}
	
