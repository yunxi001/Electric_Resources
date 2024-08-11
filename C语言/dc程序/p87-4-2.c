#include<stdio.h>
int main()
{
	int a,b,c;
	scanf("%d%d",&a,&b);
	if(a>b)
	{
		c=a;
		a=b;
	}
	else
	{
	c=b	;
	}
	printf("%d>%d",c,a);
	return 0;
}


