#include<stdio.h>
#include<math.h>
int main()
{
	double a,b,c,disc,p,q,x1,x2;
	scanf("%lf%lf%lf",&a,&b,&c);
	disc=b*b-4*a*c;
	if(disc<0)
	printf("方程无实根\n");
	else 
	{
		p=-b/(2*a);
		q=sqrt(disc)/(2*a);
		x1=p+q;
		x2=p-q;
		printf("x1=%7.2f\nx2=%7.2f\n",x1,x2);
	}
	return 0;
 } 
