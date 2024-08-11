#include<stdio.h>
int main()
{
	char str[120];
	int i,j,num=0;
	for(i=0;i<=99;i++)
	str[i]=i+1;
	for(i=0;i<=99;i++)
	for(j=2;j<str[i];j++)
	if(str[i]%j==0) break;
	else
	{
	printf("%d\t",str[i]);
	num++;	
	if(num%5==0)
	printf("\n");
	break;}

	return 0;
}
