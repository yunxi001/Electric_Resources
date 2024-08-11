#include <stdio.h>

void decimalToBinary(int n) {
	int j = 0;
    printf("%d-->", n);

    if (n == 0) {
        printf("0\n");
        return;
    }

    // 计算二进制表示
    int binary[16]; // 假设整数不超过16位
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

    // 输出二进制数
    for (j = i - 1; j >= 0; j--) {
        printf("%d", binary[j]);
    }

    printf("\n");
}

int main() {
    int n;

    // 输入数据中含有不多于50个整数
    printf("请输入整数，以空格分隔（最多50个整数）:\n");

    while (scanf("%d", &n) != EOF) {
        decimalToBinary(n);
    }

    return 0;
}

