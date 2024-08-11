#include<stdio.h>

void main()
{
	float a=0,num=-1.0;
	int i,n;
	printf("班级：通信工程一班	学号：22020311036	姓名：张迪\n"); 
	printf("清输入通项数：");
	scanf("%d",&n);
	for(i=0;i<n;i++)
	{
		num=-1*num;
		a+=num/(2*i+1);
	}
	printf("结果为：%7.3f\n",a);
}

