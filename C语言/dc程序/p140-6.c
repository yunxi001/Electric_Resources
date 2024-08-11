#include<stdio.h>
int main()
{
	int i,m=1,n=1;
	for(i=2;i<=3;i++)
	{	m=m*i;
		n=n+m;
	}
	printf("%d",n);
	return 0;
 } 
