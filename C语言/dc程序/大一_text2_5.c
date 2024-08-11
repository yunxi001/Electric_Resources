#include <stdio.h>

int main() {
	int cock = 0,hen = 0;
    for (cock = 0; cock <= 20; cock++) {
        for (hen = 0; hen <= 33; hen++) {
            int chicken = 100 - cock - hen;
            if (cock * 5 + hen * 3 + chicken / 3 == 100 && chicken % 3 == 0) {
                // 输出符合条件的解
                printf("cock=%d,hen=%d,chicken=%d\n", cock, hen, chicken);
            }
        }
    }
    return 0;
}

