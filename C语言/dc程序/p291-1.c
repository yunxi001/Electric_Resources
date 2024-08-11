#include<stdio.h>
int main()
{
	 void output(int *p,int n);
	int a[3],n=3,i;
	printf("请输入三个整数：");
	for(i=0;i<=2;i++)
	scanf("%d",&a[i]);
	output(a,n);
	for(i=0;i<=2;i++)
	printf("%d\t",a[i]);
	return 0;
 }
 void output(int *p,int n)
 {
 	int i,j,t;
 	for(i=0;i<=n-2;i++)
	for(j=i;j<=n-1;j++)
 	if(*(p+i)>*(p+j))
 	{
 		t=*(p+i);
		*(p+i)=*(p+j);
		*(p+j)=*(p+i);	
	 }
 	
  } 
