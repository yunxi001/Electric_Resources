#include<stdio.h>
int main()
{
	int a[3][3];
	int i,j,sum;
	for(i=0;i<=2;i++)
	for(j=0;j<=2;j++)
	scanf("%d",&a[i][j]); 
	sum=a[0][2]+a[0][0]+a[2][0]+a[2][2];
	printf("%d",sum);
	return 0;
}
