#include <stdio.h>
int main()
{
	int a,i,j,k;
	printf("班级：通信工程一班\t");
	printf("姓名：张迪\t");
	printf("学号：22020311036\n");
	printf("请输入一个三位数的整数："); 
	scanf("%d",&a);
	i=a/100;
	j=a%100/10;
	k=a%10;
	if(a==i*i*i+j*j*j+k*k*k)
	printf("yes\n");
	else
	printf("no\n");
	return 0;
}
