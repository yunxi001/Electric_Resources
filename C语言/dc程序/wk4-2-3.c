#include<stdio.h>
int main() 
{
	int sum=1,a,count=-1;
	printf("请输入n个正整数，最后输入负一：");
	while(a!=-1)
	{
		scanf("%d",&a);
		sum=sum+a;
		count=count+1;	
	 } 
	 printf("输入数字的个数为：%d\n平均数为：%f",count,1.0*sum/count);
	 return 0;
}
