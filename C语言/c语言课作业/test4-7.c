#include <stdio.h>
#include <math.h>

int main()
{
	int i,j,n,a,sum,x=1,y=0;
	printf("�༶��ͨ�Ź���һ��	ѧ�ţ�22020311036	�������ŵ�\n"); 
	scanf("%d%d",&n,&a);
	for(i=n;i>0;i--)
	{
		x=1;
		y=0;
		for(j=i;j>0;j--)
		{
			x =a*pow((double)10,(double)j-1);
			y += x;
		
		}		
		sum+=y;
	}
	printf("%d\n",sum); 
	return 0;
}

