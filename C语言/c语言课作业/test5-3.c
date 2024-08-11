#include <stdio.h>

int main()
{
	printf("班级：通信工程一班	学号：22020311036	姓名：张迪\n"); 
	int i,j,num=0;
	for(i=1;i<=100;i++)
	{
		if(i%10 ==7 || i%7 == 0 || i/10 == 7)
		;
		else 
		{
			printf("%d\t",i);
			num++;
		}
		if(num%8 == 0)
		printf("\n");
	}
	return 0;
}
