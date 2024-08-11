#include<stdio.h>
int main()
{
	int a,b,n;
	for(n=100;n<=200;n=n+1)
	{if(n%3==0)
	continue;
	printf("%d\t",n);
	}
	printf("\n");
	return 0;
	
}
