#include<stdio.h>
#include<math.h>
int main()
{
	int x;
	printf("请输入x：");
	scanf("%d",&x);
	if(x>=1000||x<0)
	printf("数据错误，请重新输入") ;
	else 
	 printf("x的平方根为：%d",x=sqrt(x));
	return 0; 
}
