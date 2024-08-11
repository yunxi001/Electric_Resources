#include "stdio.h"

int main(void)
{
    int Num[4] = {3,4,6,9};
    int NUM_q[4] = {5,7,1,0};
    
    short Target;
    
    scanf("%hd", &Target);
    int i = 0; 
    int j = 0;
    for(i = 0; i < 4; i++)
    {
        for(j = 0; j < 4; j++)
        {
            int result = 0;
            result = NUM_q[i] + NUM_q[j];
            if(result == Target)
            {
                printf("i = %d j = %d ", i, j);
                printf("result = %d ", result);
                printf("NUM_q[i] = %d NUM_q[j] = %d\n", NUM_q[0], NUM_q[1]);                        
            } 
            else
            {
                printf("error\n");
                
                    printf("NUM_q[j] = %d\n", NUM_q[0]);
               
            }
        }
    }
    return 0;
}

