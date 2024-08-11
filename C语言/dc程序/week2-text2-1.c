#include<stdio.h>
int main()
{
	int i,j,h,l,m=0,n=0,sum;
	int a[100][100];
	scanf("%d%d",&h,&l);
	
	for(i=0;i<h;i++)
	for(j=0;j<l;j++)
	scanf("%d",&a[i][j]);
	
	for(i=0;i<h;i++)
	for(j=0;j<l;j++)
	m=m+a[i][j];
	
	for(i=1;i<h-1;i++)
	for(j=1;j<l-1;j++)
	n=n+a[i][j];
	
	sum=m-n;
	printf("%d",sum);
	return 0;
}
