# include <stdio.h>

void main()
{
	printf("�༶��ͨ�Ź���һ��	ѧ�ţ�22020311036	�������ŵ�\n"); 
	int n,i,x;
	scanf("%d",&n);
	int a[n];
	for(i=0;i<n;i++)
	scanf("%d",&a[i]);
	x=a[0];
	for(i=0;i<n-1;i++)
	{
		if(x<a[i+1])
		x=a[i+1];	
	}
	printf("%d",x);
}
