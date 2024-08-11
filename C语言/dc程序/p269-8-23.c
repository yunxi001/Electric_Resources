#include<stdio.h>
int main()
{
	int fun(int x,int y,int (*p)(int,int));
	int min(int x,int y);
	int max(int x,int y);
	int add(int x,int y);
	int (*p)(int,int);
	int a,b,c,n;
	printf("请输入两个整数：");
	scanf("%d%d",&a,&b);
	printf("求最大数请输入1，求最小数请输入2,求和请输入3：\t");
	scanf("%d",&n);
	if(n==1) fun(a,b,max);
	else if(n==2) fun(a,b,min);
	else if(n==3) fun(a,b,add);
	else printf("输入出错！");
	return 0;
	 
}
int fun(int x,int y,int (*p)(int,int))
{
	printf("结果为：%d\n",(*p)(x,y));
}
int max(int x,int y)
{
	if(x>y)
	return(x);
	else return(y);
} 
int min(int x,int y)
{
	if(x<y)
	return(x);
	else return(y);
}
int add(int x,int y)
{
	return(x+y);
}
