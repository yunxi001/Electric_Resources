#include<stdio.h>
#include<math.h>
double x1,x2;
int main()
{
	void da(int a,int b,int c);
	void deng(int a,int b,int c);
	void xiao(int a,int b,int c);
	int a,b,c; 
	printf("�����뷽��(����ax*x+bx+c)��a,b,c��ֵ:");
	scanf("%d%d%d",&a,&b,&c);
	if(b*b-4*a*c>0)
	{
	da(a,b,c);
	printf("���̵ĸ�Ϊ��%f\t%f",x1,x2);}
	if(b*b-4*a*c==0){
	deng(a,b,c);
	printf("���̵ĸ�Ϊ��%f",x1);}
	if(b*b-4*a*c<0){
	xiao(a,b,c);
	printf("���̵ĸ�Ϊ��%f\t%f",x1,x2);}	
	return 0;
}

void da(int a,int b,int c)
{
	x1=((-b-sqrt(b*b-4*a*c))/2*a);
	x2=((-b+sqrt(b*b-4*a*c))/2*a);	
}
void deng(int a,int b,int c)
{
	x1=((-b+sqrt(b*b-4*a*c))/2*a);
		
}
void xiao(int a,int b,int c)
{
	x1=((-b+sqrt(b*b-4*a*c))/2*a);
	x2=((-b-sqrt(b*b-4*a*c))/2*a);	
}
