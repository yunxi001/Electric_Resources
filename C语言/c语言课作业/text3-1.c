#include <stdio.h>
int main()
{
	int a,b,c,i;
	printf("�༶��ͨ�Ź���һ��\t");
	printf("�������ŵ�\t");
	printf("ѧ�ţ�22020311036\n");
	printf("����������������"); 
	scanf("%d%d%d",&a,&b,&c);
	if(a>b)
	{	i=a;
		a=b;
		b=i;	}
	if(a>c)
	{	i=a;
		a=c;
		c=i;	}
	if(b>c)
	{	i=b;
		b=c;
		c=i;	}
	printf("%d<%d<%d\n",a,b,c);
	return 0;
}
