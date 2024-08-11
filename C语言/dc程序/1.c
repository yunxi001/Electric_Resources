#include<stdio.h>

int main()
{
	int a[6][6];
	int n,i,j,max,maxi,maxj;
	scanf("%d",&n);
	for(i=0;i<n;i++)
	 for(j=0;j<n;j++)
	  scanf("%d",&a[i][j]);
	maxi=maxj=0;
	max=a[0][0];
	for(i=0;i<n;i++)
	 for(j=0;j<n;j++)
	 {
	 	if(a[i][j]>max)
	 	{
	 		max=a[i][j];
	 		maxi=i;
	 		maxj=j;
		 }
	 }
	printf("%d %d %d",max,maxi+1,maxj+1);
	
	return 0;  
 } 
