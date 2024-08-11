#include <stdio.h>
#include <stdlib.h>

int main()
{
	int score;
	char grade;
	printf("班级：通信工程一班	学号：22020311036	姓名：张迪\n"); 
	printf("请输入一个百分制分数：");
	
	scanf("%d",&score);
	if(score<0 || score>100) 	/*对分数输入是否正确进行判断*/
	{
		printf("输入数据有误，请重新输入\n");
		exit(0);
	}
	else
	{
		switch(score/10)	/* 对分数进行评级*/
		{
			case 10:
			case 9:grade='A';break;		/*60分到100分 */
			case 8:grade='B';break;
			case 7:grade='C';break;
			case 6:grade='D';break;
			default:grade='E';			/*60分以下*/
		}
		printf("%d---->%c\n",score,grade);
	}
	return 0;
}
