#include <stdio.h>
int main()
{
	char a;
	printf("�༶��ͨ�Ź���һ��\t");
	printf("�������ŵ�\t");
	printf("ѧ�ţ�22020311036\n");
	printf("����������һ���ַ���"); 
	scanf("%c",&a);
    if(a>='0' && a<='9')
	printf("����\n");
	else if(a>='A' && a<='z')
	printf("��ĸ\n");
	else if(a==' ' || a=='\n')
	printf("�ָ���\n");
	else if(a>='0' && a<='9')
	printf("����\n");
	else
	printf("����\n");
	return 0;
}
