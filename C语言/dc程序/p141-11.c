#include<stdio.h>
int main()
{
	float x=100,y=0,t;
	for(t=1;t<=10;t++)
	{
		y=2*x+y;
		x=x/2;
	
	}
	printf("%f\n%f",x,y-100);
	return 0;
}
