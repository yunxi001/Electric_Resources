#include <stdio.h>
int main()
{
	int a,b,c;
	printf("班级：通信工程一班	学号：22020311036	姓名：张迪\n"); 
	printf("请分别输入三角形的三边长(整数)：");
	scanf("%d%d%d",&a,&b,&c);
    if(a+b>c && b+c>a && a+c>b)
    {
        if(a==b && a==c)
            printf("三角形是等边三角形\n");
        else if(a==b||a==c||b==c)
            printf("三角形是等腰三角形\n");
        else if((a*a+b*b==c*c)||(a*a+c*c==b*b)||(b*b+c*c==a*a))
            printf("三角形是直角三角形\n");
        else
            printf("三角形是普通三角形\n");
    }
    else
    printf("不能构成三角形\n");
	return 0;
}
