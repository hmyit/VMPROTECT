#include "../include/main.hpp"
#include <string.h>

BYTE* loadProtectedCode()
{
    BYTE c[] = {
        0x00, 0x01, 0xEC, 0x50, 0x41, 0x53, 0x53, 0x00, 0x46, 
        0x41, 0x49, 0x4c, 0x45, 0x44, 0x00
    };

    BYTE *mc = new BYTE(sizeof(c)/sizeof(c[0]));

    memcpy(mc, c, sizeof(c)/sizeof(c[0]));
    return mc;
}

void deleteProtectedCode(BYTE* mc)
{
    delete[] mc;
}