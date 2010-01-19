#include "common.h"

void *memset(void *s, u8int c, u32int n)
{
        int i;
        u8int *p;

        p = (u8int*)s;
        for(i=0; i < n; i++)
        {
                *p=c;
                p++;
        }

        return s;
}

void *memcpy(void *dest, const void *src, int count)
{
	char *tmp = dest;
	const char *s = src;

	while (count--)
		*tmp++ = *s++;
	return dest;
}

int strlen(const char *s)
{
        int len=0;

        while( '\0' != *s++)
                len++;

        return len;
}

