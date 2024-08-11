#include <stdio.h>
int main()
{
	int i,max,a[3];
	printf("班级：通信工程一班\t");
	printf("姓名：张迪\t");
	printf("学号：22020311036\n");
	printf("请输入三个整数："); 
	scanf("%d%d%d",&a[0],&a[1],&a[2]);
	max=a[0];
	for(i=1;i<3;i++)
	{
		if(a[i]>=max)
		max=a[i];	
	}
	printf("较大数为：%d\n",max);
	return 0;
}
