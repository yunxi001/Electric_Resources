#include <stdio.h>

int main()
{
	int i,a,b,c;
	printf("�༶��ͨ�Ź���һ��	ѧ�ţ�22020311036	�������ŵ�\n"); 
	for(i=100;i<1000;i++)
	{
		a=i/100;
		b=i%100/10;
		c=i%10;
		if(a*a*a+b*b*b+c*c*c==i)
		printf("%d\t",i);
	}
	return 0;
}
