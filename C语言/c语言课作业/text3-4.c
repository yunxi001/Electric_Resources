#include <stdio.h>
int main()
{
	char a;
	printf("班级：通信工程一班\t");
	printf("姓名：张迪\t");
	printf("学号：22020311036\n");
	printf("请输入任意一个字符："); 
	scanf("%c",&a);
    if(a>='0' && a<='9')
	printf("数字\n");
	else if(a>='A' && a<='z')
	printf("字母\n");
	else if(a==' ' || a=='\n')
	printf("分隔符\n");
	else if(a>='0' && a<='9')
	printf("数字\n");
	else
	printf("其他\n");
	return 0;
}
