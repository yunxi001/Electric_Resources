#include<stdio.h>
int main()
{
	int i,j,n;
	int a[2][5]={{1,4,8,7,5},{2,5,9,9,3}};
	int b[2][5]={{0},{0}};
	for(i=0;i<=4;i++)
	if(a[0][i]<a[1][i])
	    b[0][i]=a[0][i];
	else
	    b[1][i]=a[1][i];
	for(i=0;i<=1;i++)
	for(j=0;j<=3;j++)
	if(a[i][j]>a[i][j+1])
	{n=a[i][j];
	a[i][j]=a[i][j+1];
	a[i][j+1]=n;
	}
	for(i=0;i<=1;i++)
	for(j=0;j<=4;j++)
	if(b[i][j]==a[i][4])
	printf("%d\n",b[i][j]);
	return 0;
}
