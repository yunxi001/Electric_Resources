#include <stdio.h>

#define PI 3.14

int main()
{
	int r;
	scanf("%d",&r);
	printf("班级：通信工程一班\t");
	printf("姓名：张迪\t");
	printf("学号：22020311036\n");
	printf("请输入圆的半径：\n");
	printf("%d\n",r);
	printf("圆的面积s=%7.2f\n",(float)PI*r*r);
	printf("圆的周长z=%7.2f\n",(float)2*PI*r);
	return 0;	
}
