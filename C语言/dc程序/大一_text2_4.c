#include <stdio.h>

void decimalToBinary(int n) {
	int j = 0;
    printf("%d-->", n);

    if (n == 0) {
        printf("0\n");
        return;
    }

    // ��������Ʊ�ʾ
    int binary[16]; // ��������������16λ
    int i = 0;
    
    if (n < 0) {
        printf("-");
        n = -n;
    }

    while (n > 0) {
        binary[i] = n % 2;
        n /= 2;
        i++;
    }

    // �����������
    for (j = i - 1; j >= 0; j--) {
        printf("%d", binary[j]);
    }

    printf("\n");
}

int main() {
    int n;

    // ���������к��в�����50������
    printf("�������������Կո�ָ������50��������:\n");

    while (scanf("%d", &n) != EOF) {
        decimalToBinary(n);
    }

    return 0;
}

