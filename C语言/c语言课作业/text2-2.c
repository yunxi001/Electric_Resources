#include <stdio.h>

int main()
{
	int a,b,c;
	
	printf("班级：通信工程一班\t");
	printf("姓名：张迪\t");
	printf("学号：22020311036\n");
	printf("请输入三个正整数\n");
	scanf("%d%d%d",&a,&b,&c);
	printf("三者的和为%7d\n",a+b+c);
	printf("三者的和为%7d\n",a*b*c);
	printf("三者的和为%7.2f\n",(float)(a+b+c)/3);
	return 0;
}
