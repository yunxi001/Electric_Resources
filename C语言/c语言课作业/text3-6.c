#include <stdio.h>
#include <stdlib.h>

int main()
{
	int score;
	char grade;
	printf("�༶��ͨ�Ź���һ��	ѧ�ţ�22020311036	�������ŵ�\n"); 
	printf("������һ���ٷ��Ʒ�����");
	
	scanf("%d",&score);
	if(score<0 || score>100) 	/*�Է��������Ƿ���ȷ�����ж�*/
	{
		printf("����������������������\n");
		exit(0);
	}
	else
	{
		switch(score/10)	/* �Է�����������*/
		{
			case 10:
			case 9:grade='A';break;		/*60�ֵ�100�� */
			case 8:grade='B';break;
			case 7:grade='C';break;
			case 6:grade='D';break;
			default:grade='E';			/*60������*/
		}
		printf("%d---->%c\n",score,grade);
	}
	return 0;
}
