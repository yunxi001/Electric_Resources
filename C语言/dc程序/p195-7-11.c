#include<stdio.h>
int main()
{
	float pingjunshu(float str[],int n);
	int i;
	float chengji1[5];
	float chengji2[10];
	printf("�������һ����ɼ���");
	for(i=0;i<5;i++)
	scanf("%f",&chengji1[i]);
		printf("��һ����ƽ���ɼ�Ϊ��%f\n",pingjunshu(chengji1,5));
	printf("������ڶ�����ɼ���");
	for(i=0;i<10;i++)
	scanf("%f",&chengji2[i]);

	printf("�ڶ�����ƽ���ɼ�Ϊ��%f\n",pingjunshu(chengji2,10));
	return 0;
}
float pingjunshu(float str[],int n)
{
	int i;
	float m;
	for(i=0;i<n;i++)
	 m=m+str[i];
	 m=m/n;
	return(m);
}
