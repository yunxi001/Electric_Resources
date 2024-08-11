#include<stdio.h>
int main()
{
	int i,j,num=0;
	char a[100];
	char b[100];
	gets(a);
	for(i=1;i<=99;i++)
	if(a[i]!=' ') num=num+1;
	return 0;
 } 
