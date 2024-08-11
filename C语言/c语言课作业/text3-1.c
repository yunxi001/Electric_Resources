#include <stdio.h>
int main()
{
	int a,b,c,i;
	printf("班级：通信工程一班\t");
	printf("姓名：张迪\t");
	printf("学号：22020311036\n");
	printf("请输入三个整数："); 
	scanf("%d%d%d",&a,&b,&c);
	if(a>b)
	{	i=a;
		a=b;
		b=i;	}
	if(a>c)
	{	i=a;
		a=c;
		c=i;	}
	if(b>c)
	{	i=b;
		b=c;
		c=i;	}
	printf("%d<%d<%d\n",a,b,c);
	return 0;
}
