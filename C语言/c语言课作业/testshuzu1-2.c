#include <stdio.h>

int main()
{
	printf("�༶��ͨ�Ź���һ��	ѧ�ţ�22020311036	�������ŵ�\n"); 
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
