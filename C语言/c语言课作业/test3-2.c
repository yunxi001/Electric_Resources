#include <stdio.h>
int main()
{
	int a,i,j,k;
	printf("�༶��ͨ�Ź���һ��\t");
	printf("�������ŵ�\t");
	printf("ѧ�ţ�22020311036\n");
	printf("������һ����λ����������"); 
	scanf("%d",&a);
	i=a/100;
	j=a%100/10;
	k=a%10;
	if(a==i*i*i+j*j*j+k*k*k)
	printf("yes\n");
	else
	printf("no\n");
	return 0;
}
