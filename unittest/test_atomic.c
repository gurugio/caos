
#include "../src/include/atomic.h"

int main(void)
{
    size_t val = 0x12345678ABCDEF00;
    size_t old_val;
    int i;

    for (i = 0; i < 0x1ff; i++)
    {
        old_val = atomic_cmpxchg(&val, val, val+i);
        if (val != old_val + i)
            printf("atomic_cmpxchg fails\n");
    }
    
    return 0;
}
