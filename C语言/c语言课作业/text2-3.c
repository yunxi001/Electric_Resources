#include <stdio.h>

int main()
{
	int a,x,y,z;
	printf("�༶��ͨ�Ź���һ��\t");
	printf("�������ŵ�\t");
	printf("ѧ�ţ�22020311036\n");
	printf("���룺");
	scanf("%d",&a);
	printf("\n");
	x=a%100%10;
	y=a%100/10;
	z=a/100;
	printf("�����%d%d%d\n",x,y,z);
	return 0;
}
