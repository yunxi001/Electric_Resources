#include <stdio.h>

int main() {
    float fahrenheit, celsius;

    // ��ȡ����Ļ����¶�
    printf("�����뻪���¶�: ");
    scanf("%f", &fahrenheit);

    // ���ݹ�ʽ�����¶�ת��
    celsius = 5 * (fahrenheit - 32) / 9;

    // ��������¶ȣ�������λС��
    printf("�����¶�Ϊ: %.2f\n", celsius);

    return 0;
}

