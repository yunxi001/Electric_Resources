#include<stdio.h>
#include<math.h>
int main()
{
	int su(int x);
	int x;
	printf("请输入一个整数：");
	scanf("%d",&x);
	if(su(x)==0)
	printf("不是素数\n");
	else printf("是素数\n");
	return 0;
}
int su(int x)
{
	int i,t=1;
	if(x<=1)
	t=0;
	for(i=2;i<=sqrt(x);i++)
	if(x%i==0)
	t=0;
	return(t);
}
