#include<stdio.h>

int main()
{
int a,b,c,m;
scanf("%d,%d,%d",&a,&b,&c);
if(a>b)
{
if(a>c)m=a;
else m=c;
}    
else if(b>c)m=b;
else m=c;
    printf("最大值为:%d\n",m);
    return 0;
}
