#include <stdio.h>

int main() {
    int num;
    int count = 0;
	int i = 0,j = 0;
	int digit = 0;
	
    scanf("%d", &num);
	
    int num_copy = num; // 保存原始输入数字
    
    // 计算位数
    while (num > 0) {
        num /= 10;
        count++;
    }

    printf("%d\n", count); // 输出位数

    num = num_copy; // 恢复原始输入数字

    // 正序输出每一位数字
    for (i = count - 1; i >= 0; i--)  
	{
		int power = 1;
		for(j = i; j >= 1; j--){
			power *= 10;
		}
		
		digit = (num / power) % 10;
//      digit = (num / (int)pow(10, i)) % 10;//pow()函数(计算幂)返回的结果是一个浮点数
        printf("%d", digit);
        if (i > 0) {
            printf(" ");
        }
    }
    printf("\n");

    // 按逆序输出数字
    while (num_copy > 0) {
        printf("%d", num_copy % 10);
        num_copy /= 10;
    }
    printf("\n");

    return 0;
}



