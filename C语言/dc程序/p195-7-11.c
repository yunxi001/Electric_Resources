#include<stdio.h>
int main()
{
	float pingjunshu(float str[],int n);
	int i;
	float chengji1[5];
	float chengji2[10];
	printf("请输入第一个班成绩：");
	for(i=0;i<5;i++)
	scanf("%f",&chengji1[i]);
		printf("第一个班平均成绩为：%f\n",pingjunshu(chengji1,5));
	printf("请输入第二个班成绩：");
	for(i=0;i<10;i++)
	scanf("%f",&chengji2[i]);

	printf("第二个班平均成绩为：%f\n",pingjunshu(chengji2,10));
	return 0;
}
float pingjunshu(float str[],int n)
{
	int i;
	float m;
	for(i=0;i<n;i++)
	 m=m+str[i];
	 m=m/n;
	return(m);
}
