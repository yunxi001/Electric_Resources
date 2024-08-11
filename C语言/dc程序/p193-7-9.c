#include<stdio.h>
int main()
{
	int max(int x,int y);
	int a[10];
	int i,m,n;
	printf("请输入十个整数：");
	for(i=0;i<=9;i++)
	scanf("%d",&a[i]);
	for(i=0,m=a[0],n=0;i<10;i++)
	{
		if(max(m,a[i])>m)
		{
			m=max(m,a[i]);
			n=i;
		}
	}
	printf("最大数的为：%d\n该数是第%d个数\n",m,n+1);
	return 0;
 } 
 int max(int x,int y)
 {
 	return(x>y?x:y);
 }
