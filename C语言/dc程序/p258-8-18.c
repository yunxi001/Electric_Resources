#include<stdio.h>
#include<string.h>
int main()
{
	char str1[]="I am a student.",str2[20];
	int i;
	strcpy(str2,str1);
	printf("str1 is:%s\n",str1);
	printf("str2 is:%s",str2);
	return 0;
}
