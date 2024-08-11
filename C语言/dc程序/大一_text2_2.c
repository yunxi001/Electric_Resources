#include <stdio.h>

int main() {
    char ch;
    int letterCount = 0, spaceCount = 0, digitCount = 0, otherCount = 0;

    printf("请输入一行字符:\n");

    while ((ch = getchar()) != '\n') {
        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
            letterCount++;
        } else if (ch == ' ') {
            spaceCount++;
        } else if (ch >= '0' && ch <= '9') {
            digitCount++;
        } else {
            otherCount++;
        }
    }

    // 输出统计结果
    printf("%d,%d,%d,%d\n", letterCount, spaceCount, digitCount, otherCount);

    return 0;
}

