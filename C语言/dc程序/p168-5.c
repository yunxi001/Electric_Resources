#include<stdio.h>
int main()
{
	int a[10];
	int b[10]={0};
	int i,j;
	for(i=0;i<=9;i++)
	scanf("%d",&a[i]);

	for(i=9;i>=0;i--)
	for(j=0;j<=9-i;j++)
	b[i]=a[j];
	for(i=0;i<=9;i++)
	printf("%d\t",b[i]);
	return 0;
}
