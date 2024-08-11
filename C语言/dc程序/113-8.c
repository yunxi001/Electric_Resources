#include<stdio.h>
int main()
{
	char y;
	float x;
	printf("请输入成绩：");
	scanf("%f",&x);
	if(x<60)
	y=69;
	else if(x<70)
	y=68;
	else if(x<80)
	y=67;
	else if(x<90)
	y=66;
	else
	y=65;
	printf("您的成绩等级为：%c",y);
	return 0;  
}
