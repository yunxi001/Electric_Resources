#include <stdio.h>

int main()
{
	int i;
	float sum=0,num=0,j=1.0;
	printf("�༶��ͨ�Ź���һ��	ѧ�ţ�22020311036	�������ŵ�\n"); 
	for(i=1;i<10;i++)
	{
		num=1.0/i;
		sum=sum+num*j;
		i=i+2;
		j=-j;
	}
	printf("%7.3f",sum);
	return 0;
}
