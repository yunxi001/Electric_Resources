#include <stdio.h>

int main()
{
	printf("�༶��ͨ�Ź���һ��	ѧ�ţ�22020311036	�������ŵ�\n"); 
	int i,n,a[50]={1,1};
	int count = 0;
	scanf("%d",&n);
	for(i=2;i<n;i++)
	{	
		a[i]=a[i-1]+a[i-2];
	}
	for(i=0;i<n;i++)
	{
		printf("%d\t",a[i]);
		count++;
		if(count%5 == 0)
		printf("\n");
	}
	printf("\n");
	return 0;
}
