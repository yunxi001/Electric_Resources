#include <stdio.h>

int main() {
    float fahrenheit, celsius;

    // 读取输入的华氏温度
    printf("请输入华氏温度: ");
    scanf("%f", &fahrenheit);

    // 根据公式进行温度转换
    celsius = 5 * (fahrenheit - 32) / 9;

    // 输出摄氏温度，保留两位小数
    printf("摄氏温度为: %.2f\n", celsius);

    return 0;
}

