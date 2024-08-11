#include<stdio.h>
int main()
{
	int a,b,c,max;
	printf("请输入a b c:\n"); 
	scanf("%d%d%d",&a,&b,&c);
	if(a>b)
	{
		if(a>c)
		max=a;
		else max=c;
	}
	else if(c>b)
	max=c;
	else max=b;
	printf("最大数为：%d\n",max);
	return 0;
}
