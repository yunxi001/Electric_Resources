#include<stdio.h>
int main()
{
	char str[3][81];
	int i,j,d=0,x=0,s=0,k=0,q=0;
	printf("����������(�޺���)��\n");
	for(i=0;i<=79;i++)
	gets(str[i]);
	for(i=0;i<=2;i++)
	for(j=0;j<=79;j++)
	{
		if((str[i][j]>='0')&&(str[i][j]<='9'))
			s=s+1;
		else if((str[i][j]>='A')&&(str[i][j]<='Z'))
			 d=d+1;	
		else if((str[i][j]>='a')&&str[i][j]<='z')
			x=x+1;
		else if(str[i][j]==' ')
			k=k+1;
		else q=q+1;		 	
	}
	printf("Ӣ�Ĵ�д��ĸ����Ϊ��%d\nӢ��Сд��ĸ����Ϊ��%d\n",d,x);
	printf("���ָ���Ϊ��%d\n�ո����Ϊ��%d\n�����ַ�����Ϊ��%d\n",s,k,q);
	return 0;
}
