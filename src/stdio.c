#include "common.h"

char buffer[128] = {0};

static void u32int2hex(char *str, u32int val)
{
    char hexmap[17]="0123456789ABCDEF";
    u32int tmp;
    char *rslt;             //0xFFFFFFFF\0
    int pos=9;
    u32int dv;
    u32int i;

    rslt = str;
    tmp = val;
    while(tmp != 0)
    {
        dv = tmp % 0x10;
        rslt[pos--] = hexmap[dv];
        tmp = tmp / 0x10;
    }


}

int sprintf(char *str, const char *format, ...)
{
    char *p;
    u32int int_var;
    u8int char_var;
    char *charp_var;
    va_list ap;
    char *dp;
    int len;

    char hexstring[11]="0x00000000";

    if (str==1)
        dp = buffer;
    else if ( str == 0)
        dp = buffer;
    else
        dp = str;

    p = (char *)format;
    va_start(ap, format);

    while(*p != '\0')
    {
        switch(*p)
        {
            case '%':
                {
                    p++;
                    switch(*p)
                    {
                        case 'x':
                            int_var = va_arg(ap, u32int);
                            u32int2hex(hexstring, int_var);
                            memcpy(dp, hexstring, 10);
                            dp += 10;
                            memcpy(hexstring, "0x00000000", 10);
                            break;
                        case 'p':
                            int_var = (long)va_arg(ap, void*);
                            u32int2hex(hexstring, int_var);
                            memcpy(dp, hexstring, 10);
                            dp += 10;
                            memcpy(hexstring, "0x00000000", 10);
                            break;
                        case 'c':
                            char_var = va_arg(ap, char);
                            *dp++ = char_var;
                            break;
                        case 's':
                            charp_var = va_arg(ap, char*);
                            len = strlen(charp_var);
                            memcpy(dp, charp_var, len);
                            dp += len;
                            break;
                        default:
                            p--;
                            *dp++ = *p;
                            break;
                    }
                }
                break;
            default:
                *dp++ = *p;
                break;
        }
        p++;
    }

    va_end(ap);

    *dp='\0';
    if (str==0)
        putstring(buffer);
    else if (str==1)
        putstring_bochsconsole(buffer);

    return 0;
}

