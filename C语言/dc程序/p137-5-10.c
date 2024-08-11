#include<stdio.h>
#include<math.h>
int main()
{
	int i,t=0,k,n;
	for(n=101;n<=200;n=n+2)
	{
		k=sqrt(n);
		for(i=2;i<=k;i++)
		if(n%i==0) break;
	 	if(i>=k+1) 
	 	{printf("%d\t",n);
	 	t=t+1;	
		 }
		 if(t%10==0) printf("\n");	
	}
	
	return 0;
}
