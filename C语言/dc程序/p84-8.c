#include<stdio.h>
int main()
{
	char c1,c2;
	c1=getchar();
	c2=getchar();
	putchar(c1);
	putchar(c2);
	putchar('\n');
	printf("%d,%d",c1,c2);
	return 0;
}
