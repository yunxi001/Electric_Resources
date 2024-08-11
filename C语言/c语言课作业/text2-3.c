#include <stdio.h>

int main()
{
	int a,x,y,z;
	printf("班级：通信工程一班\t");
	printf("姓名：张迪\t");
	printf("学号：22020311036\n");
	printf("输入：");
	scanf("%d",&a);
	printf("\n");
	x=a%100%10;
	y=a%100/10;
	z=a/100;
	printf("输出：%d%d%d\n",x,y,z);
	return 0;
}
