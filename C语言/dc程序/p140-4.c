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

	printf("Ӣ����ĸ����Ϊ��%d\t�ո����Ϊ��%d\n",a,b);
	printf("���ָ���Ϊ��%d\t�����ַ�����Ϊ��%d\n",c,d);
	return 0;
}
