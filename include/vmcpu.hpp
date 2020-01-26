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
    /* General Purpose Registers R0 -> R3 */
    WORD R[4];
    union {
        unsigned char Flags;
        struct {
            /* Zero Flag 
                value 1 - flag is set
                value 0 - flag is not set
            */
            unsigned char ZF : 1;
            /* Carry Flag 
                value 1 - flag is set
                value 0 - flag is not set
            */
            unsigned char CF : 1;
            unsigned char Unused : 6;

        };
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
        bool loadCode(BYTE *);
};

#endif //_VM_CPU_HPP