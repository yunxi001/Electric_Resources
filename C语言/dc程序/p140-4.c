#include<stdio.h>
int main()
{
	int a=0,b=0,c=0,d=0,x;
	while((x=getchar())!='\n')
	{
		
		if(x>='a'&&x<='z'||x>='A'&&x<='Z')
		a++;
		else if(x>='0'&&x<='9')
		c++;
		else if(x==' ')
		b++;
		
		else d++;
	}

	printf("英文字母个数为：%d\t空格个数为：%d\n",a,b);
	printf("数字个数为：%d\t其他字符个数为：%d\n",c,d);
	return 0;
}
