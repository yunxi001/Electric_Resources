#include <stdio.h>

int main()
{
	int n,i,j, a=1,sum=0;
	printf("�༶��ͨ�Ź���һ��	ѧ�ţ�22020311036	�������ŵ�\n"); 
	scanf("%d",&n);
	for(i=n;i>0;i--)
	{
		a=1;
		for(j=i;j>0;j--)
		{
			a=a*j;
		}		
		sum=sum+a;
	}
	printf("%d\n",sum); 
	return 0;
}

