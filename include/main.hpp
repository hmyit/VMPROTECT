#ifndef _MAIN_HPP
#define _MAIN_HPP

#include <iostream>
#include <stdint.h>

typedef uint8_t BYTE;

BYTE* loadProtectedCode();
void deleteProtectedCode(BYTE* mc);

#endif //_MAIN_HPP