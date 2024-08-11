#include <stdio.h>
int main()
{
	printf("班级：通信工程一班	学号：22020311036	姓名：张迪\n"); 
	int i,n[10] = {0};
	for(i=0;i<10;i++)
	{
		scanf("%d",&n[i]);
		if(n[i] < 60)
		printf("低于60分的为：%d\t",n[i]);
	}
	return 0;
}
