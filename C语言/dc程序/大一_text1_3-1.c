#include <stdio.h>

int main() {
    int n;
    scanf("%d", &n);

    int matrix[6][6]; // 假设矩阵最大为6x6
	int i = 0,j = 0;
    // 读取矩阵元素
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            scanf("%d", &matrix[i][j]);
        }
    }

    int maxElement = matrix[0][0];
    int row = 0, col = 0;

    // 寻找绝对值最大的元素及其下标
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            if (abs(matrix[i][j]) > abs(maxElement)) {
                maxElement = matrix[i][j];
                row = i + 1;
                col = j + 1;
            }
        }
    }

    // 输出结果
    printf("%d %d %d\n", maxElement, row, col);

    return 0;
}

