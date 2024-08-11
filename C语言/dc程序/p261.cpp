#include<stdio.h>
int main()
{
	void exchange(char *a1,char *b1);
	char *a="I am a teacher.";
	char str[]="You are a student.";
	char *b=str;
	printf("a=%s,b=%s\n",a,b);
	printf("°ÑA¸³¸øBºó£º");
	exchange(a,b);
	printf("a=%s,b=%s\n",a,b);
	return 0;
	
}
void exchange(char *a1,char *b1)
{
	for(;*a1!='\0';a1++,b1++)
	*b1=*a1;
	*b1='\0';
}
