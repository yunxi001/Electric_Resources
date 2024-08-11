#include<stdio.h>
int main()
{
	int a[10];
	int i,j;
	for(i=0;i<=9;i++)
	scanf("%d",&a[i]);
	for(i=0;i<=8;i++)
	if(a[i]>a[i+1])
	{
		j=a[i];
		a[i]=a[i+1];
		a[i+1]=j;
	}
	printf("%d\n",a[9]);
	return 0;
	
}
