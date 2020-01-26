#include "../include/main.hpp"
#include <string.h>

BYTE* loadProtectedCode()
{
    BYTE c[] = {
        0, 1
    };

    BYTE *mc = new BYTE(sizeof(c)/sizeof(c[0]));

    memcpy(mc, c, sizeof(c)/sizeof(c[0]));
    return mc;
}

void deleteProtectedCode(BYTE* mc)
{
    delete[] mc;
}