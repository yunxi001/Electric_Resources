#include<stdio.h>
int main() 
{
	int sum=1,a,count=-1;
	printf("������n����������������븺һ��");
	while(a!=-1)
	{
		scanf("%d",&a);
		sum=sum+a;
		count=count+1;	
	 } 
	 printf("�������ֵĸ���Ϊ��%d\nƽ����Ϊ��%f",count,1.0*sum/count);
	 return 0;
}
