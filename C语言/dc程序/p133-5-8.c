#include<stdio.h>
int main()
{
	int x=1,y=1,n;
	printf("1\t1\t");
	for(n=2;n<=20;n++)
	{
		x=x+y;
		y=x+y;
		printf("%d\t%d\t",x,y);
}
return 0;
}
