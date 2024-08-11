#include<stdio.h>
#include<string.h>
int main()
{
	char str[3][20];
	char string[20];
	int i;
	for(i=0;i<=2;i++)
	gets(str[i]);
	printf("\n");
	for(i=0;i<=1;i++)
	if(strcmp(str[i],str[i+1])>0)
	{ strcpy(string,str[i]);
	strcpy(str[i],str[i+1]);
	strcpy(str[i+1],string);
	}
	printf("%s",str[2]);
	return 0;
}
