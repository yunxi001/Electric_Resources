#include <stdio.h>

void selectionSort(int arr[], int n) {
    int i, j, minIndex, temp;
    for (i = 0; i < n - 1; i++) {
        minIndex = i;
        for (j = i + 1; j < n; j++) {
            if (arr[j] < arr[minIndex]) {
                minIndex = j;
            }
        }
        // ���ҵ�����СԪ��������Ϊi��Ԫ�ؽ���
        temp = arr[i];
        arr[i] = arr[minIndex];
        arr[minIndex] = temp;
    }
}

int main() {
    int arr[10];
    int i;

    // ���� 10 �����������
    printf("������ 10 �����������:\n");
    for (i = 0; i < 10; i++) {
        scanf("%d", &arr[i]);
    }

    // ʹ��ѡ�������㷨��������
    selectionSort(arr, 10);

    // �������õ� 10 ������
    printf("����õ� 10 ������:\n");
    for (i = 0; i < 10; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    return 0;
}

