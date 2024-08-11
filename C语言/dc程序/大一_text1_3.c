#include <stdio.h>

int main() {
    int n;
    scanf("%d", &n);

    int maxElement = 0;
    int maxRow = 0;
    int maxCol = 0;
	int i = 0,j = 0;
    // 读取矩阵并找出绝对值最大的元素以及它的行下标和列下标
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            int element;
            scanf("%d", &element);
            //取绝对值 
            if(element < 0){
            	element = -element;
			}
			
            if (element > maxElement) {
                maxElement = element;
                maxRow = i + 1;
                maxCol = j + 1;
            }
        }
    }

    // 输出结果
    printf("%d %d %d\n", maxElement, maxRow, maxCol);

    return 0;
}

