#include <stdio.h>
int main()
{
	printf("�༶��ͨ�Ź���һ��	ѧ�ţ�22020311036	�������ŵ�\n"); 
	int i,n[10] = {0};
	for(i=0;i<10;i++)
	{
		scanf("%d",&n[i]);
		if(n[i] < 60)
		printf("����60�ֵ�Ϊ��%d\t",n[i]);
	}
	return 0;
}
