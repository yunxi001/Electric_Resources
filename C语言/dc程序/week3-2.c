#include<stdio.h>

int main()
{
	int jisuan(int n);
	int i;
	int a[60]={0};
	printf("������ڼ��꣺\n");
	for(i=0;i>=0;i++){
	scanf("%d",&a[i]);
	if(a[i]==0) break;
	}
	printf("ĸţ������Ϊ��\n");
	for(i=0;i>=0;i++){
	if(a[i]==0) break;
	printf("%d\n",jisuan(a[i]));
	}
}
int jisuan(int n)
{
	int x=0;
	if(n<=4) x=n;
	else
	x=jisuan(n-1)+jisuan(n-3);
	return(x);	
}


