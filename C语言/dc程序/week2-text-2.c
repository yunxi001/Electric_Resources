#include<stdio.h>
int main()
{
	int i,j,h,l,m=0,n=0,sum=0;
	int a[100][100];
	scanf("%d%d",&h,&l);
	for(i=0;i<h;i++)
	for(j=0;j<l;j++)
	scanf("%d",&a[i][j]);
	for(i=0;i<h;i++)
	for(j=0;((j==0)||(j==l-1));j=j+l-1)
	{
	m=m+a[i][j];

	n=n+a[j][i];}
	sum=m+n-a[0][0]-a[0][h-1]-a[h-1][0]-a[h-1][l-1];
	printf("%d",sum);
	return 0;
	
}
