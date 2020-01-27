#ifndef _VM_CPU_HPP
#define _VM_CPU_HPP

#include <iostream>
#include <string.h>
#include <stdint.h>

typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;

typedef struct {
    /* Here is the code and other data */
    BYTE data[4096];
    /* Size of one element is WORD 
    in order to be able to push addresses. */
    WORD stack[256];
} ADDRESS_SPACE, *PADDRESS_SPACE;

typedef struct {
    /* General Purpose Registers R0 -> R5 */
    WORD R[6];
    struct {
        /* Zero Flag 
            value 1 - flag is set if the result of the last comparison was zero
            value 0 - flag is not set
        */
        unsigned char ZF : 1;
        /* Carry Flag 
            value 1 - flag is set the results of the last comparison was moving
            value 0 - flag is not set
        */
        unsigned char CF : 1;
    };
    /* Program Counter */
    WORD PC;
    /* Stack Pointer */
    WORD SP;
} REGISTERSS, *PREGISTERSS;


class VMCPU {
    private:
        PADDRESS_SPACE AS;
        PREGISTERSS REGS;

    private:
        void vmPrintf(const char *);

    public:
        VMCPU();
        ~VMCPU();
        void run();
        bool loadCode(BYTE *, int, BYTE *, int);
};

#endif //_VM_CPU_HPP