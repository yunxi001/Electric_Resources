#include <stdio.h>

int main()
{
	printf("�༶��ͨ�Ź���һ��	ѧ�ţ�22020311036	�������ŵ�\n"); 
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
