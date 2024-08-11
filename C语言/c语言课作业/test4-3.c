#include <stdio.h>

int main()
{
	int i,a,b,c;
	printf("班级：通信工程一班	学号：22020311036	姓名：张迪\n"); 
	for(i=100;i<1000;i++)
	{
		a=i/100;
		b=i%100/10;
		c=i%10;
		if(a*a*a+b*b*b+c*c*c==i)
		printf("%d\t",i);
	}
	return 0;
}
