#include <stdio.h>

int main()
{
	printf("班级：通信工程一班	学号：22020311036	姓名：张迪\n"); 
	int letter=0,digit=0,other=0;
	char ch;
	for(ch=getchar();ch != '\n';ch=getchar())
	{
		if(ch >= '0' && ch<= '9')
		digit++;
		else if(ch >= 'A' && ch <= 'z')
		letter++;
		else 
		other++;
	}
	printf("letter=%d,digit=%d,other=%d",letter,digit,other); 
	return 0;
}
