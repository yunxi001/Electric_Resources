#include<stdio.h>

int main()
{
	int shumu(int n);
	int n;
	printf("������ڼ��꣺\n");
	scanf("%d",&n);	
	printf("ĸţ������Ϊ��\n");
	printf("%d",shumu(n));
}
int shumu(int n)
{
	void jisuan(int n);
	int i=0;
	if(n==0) return(0);
	else if(n>4) return(jisuan(n));
	else
	for(i=1;i<=4;i++)
	if(n==i) return(i);
}
void jisuan(int n)
{
	int x=0,t=0;
	if(n>4)
	x=jisuan(n-1)+jisuan(n-3);	
}
