static volatile char *video=(volatile char*)0xB8000;

void write_string(int colour, const char *string)
{
    

    while (*string!=0) {
        *video=*string;
        string++;
        video++;
        *video=colour;
        video++;
    }
}

int start_kernel()
{
    int i=0;

    for(i=0; i<10; i++) {
        write_string(0x1F, "Welcome to StarxOS!");
    }
    return 0xBAD;
}
