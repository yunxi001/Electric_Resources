#include <stdio.h>

int main()
{
	printf("�༶��ͨ�Ź���һ��	ѧ�ţ�22020311036	�������ŵ�\n"); 
	int i,j,num=0;
	for(i=2;i<101;i++)
	{
		for(j=2;j<=i;j++)
		{
			if( i%j == 0 && j != i)
			break;
			if(j == i)
			{
				printf("%d\t",i);
				num++;
				if(num%5 == 0)	
			 	printf("\n");	
			 } 
			
		}	
	}
	return 0;
}
