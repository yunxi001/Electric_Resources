#include <stdio.h>
#include <math.h>
int isprime (int m);
int main(void)
{  
   int  m;  
   for(m=2 ; m <= 100; m++)
    {
    if (IsPrime(m) != 0 )
        printf("%6d", m );
}
   printf ("\n");
   return 0;   }
int IsPrime (int m)
{  
   int i, k;
   if ( m == 1 ) return 0;  
   k = (int)sqrt (m);
   for( i = 2; i <= k; i++ )
  {
       if (  m%i==0)
          return 0;
   }
   return 1;
}
/*
int main()
{
	printf("�༶��ͨ�Ź���һ��	ѧ�ţ�22020311036	�������ŵ�\n"); 
	int a[num] = {0};
	int i,x,n=a[0];
	for(i=0;i<num;i++)
	scanf("%d",&a[i]);
	for(i=1;i<num;i++)
	{
		if(x < a[i])
		{
			x = a[i];
			n = i;
		}	
	}
	printf("���ֵΪ%d,�±�Ϊ%d\n",x,n);
	x = a[0] ;
	n = 0;
	for(i=1;i<num;i++)
	{
		if(x > a[i])
		{
			x = a[i];
			n = i;
		}	
	}
	printf("��СֵΪ%d,�±�Ϊ%d\n",x,n);
	return 0;
}
*/

