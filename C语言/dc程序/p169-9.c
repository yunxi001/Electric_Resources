#include<stdio.h>
int main()
{
	int a[15];
	int i,j,t,x;
	int m=0,n=8;
	printf("������15������");
	for(i=0;i<=14;i++)
	scanf("%d",&a[i]);
	printf("\n");
	for(j=0;j<=13;j--)
	for(i=0;i<=j;i++)
	if(a[i]>a[i+1])
	{
		t=a[i];
		a[i]=a[i+1];
		a[i+1]=t;
	}
	printf("������Ҫ���ҵ�����");
	scanf("%d",&x);
	printf("\n");
	if(x<=a[14])
	{	
		while(n!=0) {
		
		if(x==a[n-1]){
		
		printf("����Ϊ�����е�%d��Ԫ�ص�ֵ",n); break;}
		
		else if((x>=m)&&(x<n))
		{ m=m; n=n/2;
		}
		else { t=(n-m)/2; m=n; n=n+t;
		}
		}
	 } 
	else printf("�޴���\n");
	
	return 0;
}
