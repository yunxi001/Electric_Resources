#include<stdio.h>
int main()
{
	int fun(int x,int y,int (*p)(int,int));
	int min(int x,int y);
	int max(int x,int y);
	int add(int x,int y);
	int (*p)(int,int);
	int a,b,c,n;
	printf("����������������");
	scanf("%d%d",&a,&b);
	printf("�������������1������С��������2,���������3��\t");
	scanf("%d",&n);
	if(n==1) fun(a,b,max);
	else if(n==2) fun(a,b,min);
	else if(n==3) fun(a,b,add);
	else printf("�������");
	return 0;
	 
}
int fun(int x,int y,int (*p)(int,int))
{
	printf("���Ϊ��%d\n",(*p)(x,y));
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
