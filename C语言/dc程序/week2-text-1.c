#include<stdio.h>
int main()
{
	char str[11][11];
	int i,j,k,h,l;
	char f;
	scanf("%d%d %c %d",&h,&l,&f,&k);
	for(i=0;i<h;i++)
	for(j=0;j<l;j++)
	str[i][j]=f;
	if(k!=1)
	{
		for(i=1;i<h-1;i++)
		for(j=1;j<l-1;j++)
		str[i][j]=' ';
	}
	for(i=0;i<h;i++){	
	for(j=0;j<l;j++)
	printf("%c",str[i][j]);
	printf("\n");
	}
	return 0;
	
}
