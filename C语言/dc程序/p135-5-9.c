#include<stdio.h>
#include<math.h>
int main()
{
	int n,i,k;
	scanf("%d",&n);
	k=sqrt(n);
	for(i=2;i<=k;i++)
	
		if(n%i==0)
		break;
	if(i<=k) printf("不是素数");
	else printf("是素数");
		
	
	return 0;
}
