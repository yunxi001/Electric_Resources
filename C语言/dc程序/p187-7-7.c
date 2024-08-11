#include<stdio.h>
int main()
{
	int fac(int n);
	int n;
	printf("请输入n(n为正整数):");
	scanf("%d",&n);
	
	printf("%d!=%d",n,fac(n));
	return 0;
	
}
int fac(int n)
{ 
	int t;
	if(n<0)
	printf("输入出错！！！");
	else if((n==0)||(n==1)) 
	t=1;
	else t=n*fac(n-1);
	return(t);
}
