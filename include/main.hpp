#ifndef _MAIN_HPP
#define _MAIN_HPP

#include <iostream>
#include <stdint.h>

typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;

BYTE* loadProtectedCode();
void deleteProtectedCode(BYTE* mc);

#endif //_MAIN_HPP