#include<stdio.h>
int main()
{
	int max4(int a,int b,int c,int d);
	int a,b,c,d,max;
	printf("请输入四个整数：");
	scanf("%d%d%d%d",&a,&b,&c,&d);
	max=max4(a,b,c,d);
	printf("最大的是：%d\n",max);
	return 0;
 } 
 int max4(int a,int b,int c,int d)
 {
 	int max2(int a,int b);
 	return(max2(max2(max2(a,b),c),d));
 }
 int max2(int a,int b)
 {
 	return(a>b?a:b);
 }
