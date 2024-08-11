#include<stdio.h>
int main()
{
	float score[][4]={{60,70,80,90},{56,89,67,88},{34,78,90,66}};
	float *search(float (*pointer)[4],int n);
	float *p;
	int i,k;
	printf("输入要查找的学生序号：");
	scanf("%d",&k);
	printf("该学生成绩为：\n");
	p=search(score,k);
	for(i=0;i<4;i++)
		printf("%5.2f\t",*(p+i));
	return 0;	
}
float *search(float (*pointer)[4],int n)
{
	float *pt=*(pointer+n);
	return(pt);
 } 
