#include <stdio.h>

int main()
{
	printf("�༶��ͨ�Ź���һ��	ѧ�ţ�22020311036	�������ŵ�\n"); 
	int i,j,num=0;
	for(i=1;i<=100;i++)
	{
		if(i%10 ==7 || i%7 == 0 || i/10 == 7)
		;
		else 
		{
			printf("%d\t",i);
			num++;
		}
		if(num%8 == 0)
		printf("\n");
	}
	return 0;
}
