#include<stdio.h>
#include<math.h>
int main()
{
	int m,n,t,i,h;
	scanf("%d%d",&m,&n);
	if(m>n)
	;
	else{
 t=m;
	m=n;
	n=t;	}
	for(i=n;i<=n;i--)
	if(m%i==0&&n%i==0) break;
	printf("最大公约数为：%d\n",i);
	for(h=m;h>=m;h++)
	if(h%m==0&&h%n==0) break;
	printf("最小公倍数为：%d\n",h);
	return 0;

}
