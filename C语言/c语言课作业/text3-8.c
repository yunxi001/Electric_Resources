#include <stdio.h>
int main()
{
	int a,b,c;
	printf("�༶��ͨ�Ź���һ��	ѧ�ţ�22020311036	�������ŵ�\n"); 
	printf("��ֱ����������ε����߳�(����)��");
	scanf("%d%d%d",&a,&b,&c);
    if(a+b>c && b+c>a && a+c>b)
    {
        if(a==b && a==c)
            printf("�������ǵȱ�������\n");
        else if(a==b||a==c||b==c)
            printf("�������ǵ���������\n");
        else if((a*a+b*b==c*c)||(a*a+c*c==b*b)||(b*b+c*c==a*a))
            printf("��������ֱ��������\n");
        else
            printf("����������ͨ������\n");
    }
    else
    printf("���ܹ���������\n");
	return 0;
}
