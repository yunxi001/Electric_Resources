#include<stdio.h>
#include<math.h>
int main()
{
	double p,a,b,c,d,e;
	a=0.0414;
	b=0.0468;
	c=0.054;
	d=0.0585;
	e=0.0072;
	p=1000*(1+5*d);
	printf("(1)=%f\n",p);
	p=1000*(1+2*b);
	p=p*(1+3*c);
	printf("(2)=%f\n",p);
	p=1000*(1+3*c);
	p=p*(1+2*b);
	printf("(3)=%f\n",p);
	p=1000*pow((1+e),5);
	printf("(4)=%f\n",p);
	p=1000*pow((1+e/4),4*5);
	printf("(5)=%f\n",p);
	return 0;
	
 } 
