#include<stdio.h>

void main()
{
	float a=0,num=-1.0;
	int i,n;
	printf("�༶��ͨ�Ź���һ��	ѧ�ţ�22020311036	�������ŵ�\n"); 
	printf("������ͨ������");
	scanf("%d",&n);
	for(i=0;i<n;i++)
	{
		num=-1*num;
		a+=num/(2*i+1);
	}
	printf("���Ϊ��%7.3f\n",a);
}

