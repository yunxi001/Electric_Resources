#include<stdio.h>
#include<math.h>
int main()
{
	int su(int x);
	int x;
	printf("������һ��������");
	scanf("%d",&x);
	if(su(x)==0)
	printf("��������\n");
	else printf("������\n");
	return 0;
}
int su(int x)
{
	int i,t=1;
	if(x<=1)
	t=0;
	for(i=2;i<=sqrt(x);i++)
	if(x%i==0)
	t=0;
	return(t);
}
