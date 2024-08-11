#include <stdio.h>

int main()
{
	printf("班级：通信工程一班	学号：22020311036	姓名：张迪\n"); 
	int x,i;
	int a[10]={1,45,18,7,22,11,33,15,27,19};
	scanf("%d",&x);
	
	for(i=0;i<10;i++)
	{
		if(a[i] == x)
		break;
	}
	
	if(i<9)
		printf("Index is %d\n",i);
	else
		printf("Not Found\n");
	return 0;
}
