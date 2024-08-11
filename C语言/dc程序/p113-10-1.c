#include<stdio.h>
int main()
{
	double i,w,a=0.1,b=0.075,c=0.05,d=0.03,e=0.015,f=0.01,m=100000.0;
	printf("请输入当月利润：");
	scanf("%lf",&i);
	if(i<=100000)
	w=m*a;
	else if(i<=2*m)
	w=m*a+(i-m)*b;
	else if(i<=4*m)
	w=m*a+m*b+(i-2*m)*c;
	else if(i<=6*m)
	w=m*a+m*b+2*m*c+(i-4*m)*d;
	else if(i<=10*m)
	w=m*a+m*b+2*m*c+2*m*d+(i-6*m)*e;
	else w=m*a+m*b+2*m*c+2*m*d+4*m*e+(i-10*m)*f;
	printf("应发奖金为：%f\n",w);
	return 0; 
 } 
