#include <stdio.h>

int main() {
    int num;
    int count = 0;
	int i = 0,j = 0;
	int digit = 0;
	
    scanf("%d", &num);
	
    int num_copy = num; // ����ԭʼ��������
    
    // ����λ��
    while (num > 0) {
        num /= 10;
        count++;
    }

    printf("%d\n", count); // ���λ��

    num = num_copy; // �ָ�ԭʼ��������

    // �������ÿһλ����
    for (i = count - 1; i >= 0; i--)  
	{
		int power = 1;
		for(j = i; j >= 1; j--){
			power *= 10;
		}
		
		digit = (num / power) % 10;
//      digit = (num / (int)pow(10, i)) % 10;//pow()����(������)���صĽ����һ��������
        printf("%d", digit);
        if (i > 0) {
            printf(" ");
        }
    }
    printf("\n");

    // �������������
    while (num_copy > 0) {
        printf("%d", num_copy % 10);
        num_copy /= 10;
    }
    printf("\n");

    return 0;
}



