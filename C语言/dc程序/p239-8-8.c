#include<stdio.h>
int main()
{
	void exchange(int *p,int n);
	int i,a[10],*p=a;
	printf("请输入含有十个整的数组：");
	for(i=0;i<10;i++)
	scanf("%d",p++);
	p=a;
	exchange(a,10);
	for(i=0;i<=9;i++)
	printf("%d\t",*p++);
	return 0;
}
void exchange(int *p,int n)
{
	int *i,*j,t,*x,m=(n-1)/2;
	i=p;j=p+n-1;x=p+m;
	for(;i<=x;i++,j--)
	{
		t=*i;
		*i=*j;
		*j=t;
	}
}
