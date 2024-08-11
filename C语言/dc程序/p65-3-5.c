#include<stdio.h>
#include<math.h>
int main()
{
	double a,b,c,p,q,x1,x2,disc;
	scanf("%lf%lf%lf",&a,&b,&c);
	disc=b*b-4*a*c;
	p=-b/(a*2);
	q=sqrt(disc)/(a*2);
	x1=p+q;
	x2=p-q;
	printf("x1=%7.2f\nx2=%7.2f\n",x1,x2);
	return 0;
}
