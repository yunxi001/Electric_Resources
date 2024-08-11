#include<stdio.h>
#include<string.h>
int main()
{
	void shuchu(char *name[],int n);
	void paixu(char *name[],int n);
	char *name[]={"Follow me","BASIC","Great Wall","FORTRAN","Computer design"};
	int n=5;
	paixu(name,n);
	shuchu(name,n);
	return 0;
}
void paixu(char *name[],int n)
{
	char *t;
	int i,j;
	for(i=0;i<n-1;i++)
	for(j=0;j<n-i-1;j++)
	if(strcmp(name[n-i-1],name[j])<0)
	{

	t=name[n-i-1];name[n-i-1]=name[j];name[j]=t;	}
}
void shuchu(char *name[],int n)
{
	int i;
	for(i=0;i<n;i++)
	printf("%s\n",name[i]);
}
