#include<stdio.h>
int main()
{
	int i,j,x,y,max;
	int a[3][4]={{1,2,3,4},{9,8,7,6},{-10,10,-5,2}};
	max=a[0][0];
	for(i=0;i<=2;i++)
	for(j=0;j<=3-i;j++)
	{
		if(max<a[i][j+1])
		{
	max=a[i][j+1];
		x=i;
		y=j+1;	}
	}
	printf("max=%d\n行号为%d\n列号为%d\n",max,x,y);
	return 0;

 
 } 
