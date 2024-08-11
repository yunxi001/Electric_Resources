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
        // 将找到的最小元素与索引为i的元素交换
        temp = arr[i];
        arr[i] = arr[minIndex];
        arr[minIndex] = temp;
    }
}

int main() {
    int arr[10];
    int i;

    // 输入 10 个无序的数字
    printf("请输入 10 个无序的数字:\n");
    for (i = 0; i < 10; i++) {
        scanf("%d", &arr[i]);
    }

    // 使用选择排序算法进行排序
    selectionSort(arr, 10);

    // 输出排序好的 10 个整数
    printf("排序好的 10 个整数:\n");
    for (i = 0; i < 10; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    return 0;
}

