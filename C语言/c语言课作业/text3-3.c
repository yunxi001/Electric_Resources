#include <stdio.h>
int main()
{
	int i,max,a[3];
	printf("�༶��ͨ�Ź���һ��\t");
	printf("�������ŵ�\t");
	printf("ѧ�ţ�22020311036\n");
	printf("����������������"); 
	scanf("%d%d%d",&a[0],&a[1],&a[2]);
	max=a[0];
	for(i=1;i<3;i++)
	{
		if(a[i]>=max)
		max=a[i];	
	}
	printf("�ϴ���Ϊ��%d\n",max);
	return 0;
}
