#include<stdio.h>
int main()
{
	char y;
	float x;
	printf("������ɼ���");
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
	printf("���ĳɼ��ȼ�Ϊ��%c",y);
	return 0;  
}
