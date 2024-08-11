#include <stdio.h>

int main()
{
	printf("班级：通信工程一班	学号：22020311036	姓名：张迪\n"); 
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
