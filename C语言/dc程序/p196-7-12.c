#include<stdio.h>
int main()
{
	void min(int m,int b[]);
	int i;
	int a[10];
	printf("请输入十个整数：");
	for(i=0;i<10;i++)
	scanf("%d",&a[i]);
	
	min(i,a);
	for(i=0;i<10;i++)
	printf("%d\t",a[i]);
	return 0;
	 
}

void min(int m,int b[])
{
	int i,j,n,t;
	for(j=0;j<m-1;j++)
	for(i=j,b[n]=b[i];i<10;i++)
	if(b[n]<b[i])
	{
		t=b[n];
		b[n]=b[i];
		b[i]=t;
	}
	
}
