#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define unlikely(expr) __builtin_expect((expr), 0)
#define   likely(expr) __builtin_expect((expr), 1)


uint_fast32_t load32(const void* V);
static inline uint_fast32_t nonzero_chunk(const char *p);
int is_empty_fast(const char * buf, size_t size);


static inline uint_fast32_t nonzero_chunk(const char *p)
{
    uint_fast32_t tmp1, tmp2;    

    tmp1 = load32(&tmp1);
    tmp2 = load32(&tmp2);

    return tmp1|tmp2;
}

uint_fast32_t load32(const void* V)
{
   uint_fast32_t Ret = 0;
   uint8_t* Val = (uint8_t*) V;

   Ret |= ((uint_fast32_t) Val[0]);
   Ret |= ((uint_fast32_t) Val[1]) << 4;
   Ret |= ((uint_fast32_t) Val[2]) << 8;
   Ret |= ((uint_fast32_t) Val[3]) << 12;
   Ret |= ((uint_fast32_t) Val[4]) << 16;
   Ret |= ((uint_fast32_t) Val[5]) << 20;
   Ret |= ((uint_fast32_t) Val[6]) << 24;
   Ret |= ((uint_fast32_t) Val[7]) << 28;

   return Ret;
}

int is_empty_fast(const char * buf, size_t size) 
{
    uint_fast32_t tmp1, tmp2;
    const unsigned width = sizeof(tmp1) * 2; // 8 byte

    if (likely(size >= width))
    {
        const char *endp = buf + size - width;  
        
        uintptr_t misalign = (uintptr_t)buf % width; // check if memory is aligned
        const char *p = buf + width  - misalign;

        while (p < endp)
        {
            if(nonzero_chunk(p)) return 0;
            p += width;
        }

        if(nonzero_chunk(endp)) return 0;
    } 
    else
    {
        const unsigned char *p = buf;
        const unsigned char *e = buf + size;
        unsigned char t = 0;

        do
        {
            t |= *(p++);
        } while (p < e);

        return t == 0;
    }
    return 1;
}


int is_empty_slow(const char * buf, size_t size) 
{
    for(size_t i = 0; i < size; i++)
    {
        if(buf[i] != 0) return 0;
    }
    return 1;
}


int main()
{
	short buf[] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 1};

	int tmp = is_empty_fast((const char*)buf, sizeof(buf));

    printf("tmp = %d\n", tmp);
	return tmp;
}
