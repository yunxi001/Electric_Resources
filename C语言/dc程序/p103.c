#include<stdio.h>
#include<math.h>
int main()
{
	int year,leap;
	scanf("%d",&year);
	if((year%4==0&&year%100!=0)||(year%400==0))
	leap=1;
	else
	leap=0;
	if(leap)
	printf("是闰年",year);
	else
	printf("不是闰年",year);
	return 0;
}
	
	
