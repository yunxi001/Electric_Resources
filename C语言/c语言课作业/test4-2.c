# include <stdio.h>

int main()
{
	printf("�༶��ͨ�Ź���һ��	ѧ�ţ�22020311036	�������ŵ�\n"); 
	int n,i,f=0,z=0,l=0;
	scanf("%d",&n);
	int a[n];
	for(i=0;i<n;i++)
	scanf("%d",&a[i]);
	for(i=0;i<n;i++)
	{
		if(a[i]<0)
		f++;
		if(a[i]==0)
		l++;
		if(a[i]>0) 
		z++;
	}
	printf("%d %d %d\n",f,l,z);
	return 0;
}
