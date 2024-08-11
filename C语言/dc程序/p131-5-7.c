#include<stdio.h>
#include<math.h>
int main()
{
	double pi=1,n,a=1;
	for(n=3;1/n>=1e-8;n=n+2,a++)
	
	
	
		pi=pi+pow(-1,a)*1/n;
		
	
	printf("%10.9f\n%f",pi*4.0,a);
	return 0;
	
}
