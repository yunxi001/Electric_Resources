#include<stdio.h>
int main()
{
	void output(int n,int *y);
	int year_input(int n,int *y);
	int pes;int year[100];
	printf("请输入患病总人数：");
	scanf("%d",&pes);
	printf("请输入各年龄的病患：\n");
	if(year_input(pes,year))
		output(pes,year);
}
int year_input(int n,int *y)
{
	int cheak(int n,int *y);
	int t=0,i=0;
	for(t=0;t<n;t++)
	scanf("%d",y++);
	cheak(n,y);
}
void output(int n,int *t)
{
	int i;
	float a=0.0,b=0.0,c=0.0,d=0.0;
	for(i=0;i<n;i++)
	{
		if((*(t+i)>=1)&&(*(t+i)<=18)) a++;
		else if((*(t+i)>=19)&&(*(t+i)<=35)) b++;
		else if((*(t+i)>=36)&&(*(t+i)<=60)) c++;
		else d++;
	}
	printf("0-18年龄段患病人数占总年龄段患病人数的比例：%5.2f%%\n",a/n*100);
	printf("19-35年龄段患病人数占总年龄段患病人数的比例：%5.2f%%\n",b/n*100);
	printf("36-60年龄段患病人数占总年龄段患病人数的比例：%5.2f%%\n",c/n*100);
	printf("61岁及以上年龄段患病人数占总年龄段患病人数的比例：%5.2f%%\n",d/n*100);
}
int cheak(int n,int *y)
{
	int i,j;
	for(i=0;i<n;i++)
	{
		if((*y<0)||(*y>=100))
		{
		printf("erroy");
		return(0);}
		else return(1);
	}
}
