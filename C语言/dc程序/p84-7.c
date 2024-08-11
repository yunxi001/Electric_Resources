#include<stdio.h>
#include<math.h>
int main()
{
	double r,h,pi=3.14159,c,s1,s2,v1,v2;
	printf("请输入圆半径和圆柱高："); 
	scanf("%lf%lf",&r,&h);
	c=pi*r*2;
	s2=4*pi*r*r;
	s1=pi*r*r;
	v1=4*pi*r*r/3;
	v2=h*pi*r*r;
	printf("圆周长为：%7.2f\n圆面积为：%7.2f\n圆球表面积为：%7.2f\n",c,s1,s2);
	printf("圆球体积为：%7.2f\n圆柱体积为：%7.2f\n",v1,v2);
	return 0;
	
}
