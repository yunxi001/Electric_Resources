#include<stdio.h>
#include<math.h>
int main()
{
	double r,h,pi=3.14159,c,s1,s2,v1,v2;
	printf("������Բ�뾶��Բ���ߣ�"); 
	scanf("%lf%lf",&r,&h);
	c=pi*r*2;
	s2=4*pi*r*r;
	s1=pi*r*r;
	v1=4*pi*r*r/3;
	v2=h*pi*r*r;
	printf("Բ�ܳ�Ϊ��%7.2f\nԲ���Ϊ��%7.2f\nԲ������Ϊ��%7.2f\n",c,s1,s2);
	printf("Բ�����Ϊ��%7.2f\nԲ�����Ϊ��%7.2f\n",v1,v2);
	return 0;
	
}
