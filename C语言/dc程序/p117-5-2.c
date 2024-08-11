#include<stdio.h>
int main()
{
	int a=1,b=1;
	do
	{
		b=b+1;
		a=a+b;
		
		
	}
	while(b<=100);
	printf("%d",a);
	return 0;
	
	
}
