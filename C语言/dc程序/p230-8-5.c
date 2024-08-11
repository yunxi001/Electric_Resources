#include<stdio.h>
int main()
{
	void exchange(int *p1,int *p2,int *p3);
	int a,b,c;
	int *p1,*p2,*p3;
	printf("请输入三个整数：");
	scanf("%d%d%d",&a,&b,&c);
	p1=&a;p2=&b;p3=&c;
	exchange(p1,p2,p3);
	printf("%d>%d>%d",a,b,c);
	return 0;
}
void swap(int *p1,int *p2)
{
	int temp;
	temp=*p1;
	*p1=*p2;
	*p2=temp;	
}
void exchange(int *p1,int *p2,int *p3)
{
	void swap(int *p1,int *p2);
	if(*p1<*p2) swap(p1,p2);
	if(*p1<*p3) swap(p1,p3);
	if(*p2<*p3) swap(p2,p3);
}
 
