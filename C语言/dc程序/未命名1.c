#include<stdio.h>
#include<math.h>
int main()
{
float p0=1000,r1=0.0414,r2=0.0468,r3=0.054,r4=0.0585,p1,p2,p3,p4,p5;
p1=p0*(1+5*r4);
p2=(p0*(1+2*r2))*(1+3*r3);
p3=(p0*(1+3*r3))*(1+2*r2);
p4=p0*pow(1+r1,5);
p5=p0*pow(1+r1/4,4*5);
    printf("p1=%f\np2=%f\np3=%f\np4=%f\np5=%f\n",p1,p2,p3,p4,p5);
    return 0;
}
