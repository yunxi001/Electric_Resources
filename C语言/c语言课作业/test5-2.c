#include <stdio.h>

int main()
{
	printf("班级：通信工程一班	学号：22020311036	姓名：张迪\n"); 
	int num,i;
	scanf("%d",&num);
	for(i=2 ;i<num ;i++)
	if(num%i == 0)
	break;
	if(i == num)
	printf("yes");
	else printf("no");
	return 0;
}
