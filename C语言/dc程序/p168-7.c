#include<stdio.h>
int main()
{
	int i,j,n,x,y;
	int a[99][99];
		printf("请输入魔方阵的阶数(阶数为奇数):");
	scanf("%d",&n);	
	for(i=0;i<=n-1;i++)
	for(j=0;j<=n-1;j++)
	a[i][j]=0;
	int h=1,l=n/2+1;
	a[h-1][n/2+1-1]=1;
	for(i=2;i<=n*n;i++)
	{ 
		x=h;
		y=l;
		if(h==1)
		h=n;
		else h=h-1;
		if(l==n)
		l=1;
		else l=l+1;
		if(a[h-1][l-1]==0)
		a[h-1][l-1]=i;
		else {
		a[x-1+1][y-1]=i;
		h=x+1;
		l=y;}
	}
	for(i=0;i<=n-1;i++)
	{
	for(j=0;j<=n-1;j++)
	    {
		    printf("%d\t",a[i][j]);
	    }
	printf("\n");
	}
	return 0;	
 } 
