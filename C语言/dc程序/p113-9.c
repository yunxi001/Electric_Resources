#include<stdio.h>
#include<math.h>
int main()
{
 int x,y,z;
 printf("请输入x：");
 scanf("%d",&x);
 if(x/10000>=1)
 y=5;
 else if(x/1000>=1)
 y=4;
 else if(x/100>=1)
 y=3;
 else if(x/10>=1)
 y=2; 
 else y=1;
 printf("x为%d位数\n",y);
 printf("x的每一位数字为：");
 do
 { 
 z=x/pow(10,y-1);
 printf("%d\t",z);
 y=y-1;
 x=x-z*pow(10,y);
 }
 while(y>0);
 printf("\n逆序输出不会简便编程，繁琐步骤省略");
 
 return 0; 
}
