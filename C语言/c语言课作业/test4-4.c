#include <stdio.h>

int main()
{
	int i;
	float sum=0,num=0,j=1.0;
	printf("班级：通信工程一班	学号：22020311036	姓名：张迪\n"); 
	for(i=1;i<10;i++)
	{
		num=1.0/i;
		sum=sum+num*j;
		i=i+2;
		j=-j;
	}
	printf("%7.3f",sum);
	return 0;
}
