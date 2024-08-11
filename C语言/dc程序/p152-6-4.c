#include<stdio.h>
int main()
{
	int a[2][3]={{1,2,3},{4,5,6}};
	int b[3][2];
	int x,y;
	for(x=0;x<2;x++)
	for(y=0;y<3;y++)
	{
		printf("%d\t",a[x][y]);
		b[y][x]=a[x][y];
		
	}
	printf("\n");
	for(x=0;x<3;x++)
	for(y=0;y<2;y++)
	printf("%d\t",b[x][y]);
	return 0;
}
