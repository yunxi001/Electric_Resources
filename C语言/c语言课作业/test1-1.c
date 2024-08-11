#include <stdio.h>
int main()
{
	int a,b,c;
	scanf("%d%d",&a,&b);
	if(a<b)
	{
		c=a;
	}
	else
    {
    	c=b;
	}
	printf("a,b中较小值为：%d\n",c);
	return 0;
}
