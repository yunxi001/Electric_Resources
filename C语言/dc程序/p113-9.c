#include<stdio.h>
#include<math.h>
int main()
{
 int x,y,z;
 printf("������x��");
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
 printf("xΪ%dλ��\n",y);
 printf("x��ÿһλ����Ϊ��");
 do
 { 
 z=x/pow(10,y-1);
 printf("%d\t",z);
 y=y-1;
 x=x-z*pow(10,y);
 }
 while(y>0);
 printf("\n��������������̣���������ʡ��");
 
 return 0; 
}
