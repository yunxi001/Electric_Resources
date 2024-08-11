#include<stdio.h>
int main()
{
	 int bei(int x,int y);
	 int yue(int x,int y);
	int x,y;
	printf("请输入两个整数：");
	scanf("%d%d",&x,&y);
	printf("最大公约数为：%d\n",yue(x,y));
	printf("最小公倍数为：%d\n",bei(x,y));
	return 0;
	
 } 
 int bei(int x,int y)
 {
 	int i,max;
 	max=(x>y?x:y);
 	for(i=max;i>=max;i++)
 	if((i%x==0)&&(i%y==0))
 	return(i);
 }
 int yue(int x,int y)
 {
 	int i,min;
 	min=(x<y?x:y);
 	for(i=min;i<=min;i--)
 	if((x%i==0)&&(y%i==0))
 	return(i);
 }
