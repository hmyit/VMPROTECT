#include "../include/vmcpu.hpp"

VMCPU::VMCPU()
{
    AS = (PADDRESS_SPACE) new ADDRESS_SPACE;
    REGS = (PREGISTERSS) new REGISTERSS;

    REGS->PC = 0;
    REGS->SP = sizeof(AS->stack) / sizeof(WORD);
    REGS->Flags = 0;
}

VMCPU::~VMCPU()
{
    delete AS;
    delete REGS;
}

bool VMCPU::loadCode(BYTE *mcode)
{
    unsigned int mcodeSize = sizeof(mcode)/sizeof(mcode[0]);

    if(mcodeSize > sizeof(AS->data)) 
        return false;
    memcpy(AS->data, mcode, mcodeSize);
    return true;
}

void VMCPU::run()
{
    bool exit = false;
    BYTE opcode;

    while(!exit)
    {
        opcode = AS->data[REGS->PC++];
        switch(opcode)
        {
            /* NOP */
            case 0x00:
            case 0x01:
                vmPrintf("[DEBUG] NOP");
                break;
            /* EC - end of code */
            case 0xEC:
                vmPrintf("[DEBUG] EC");
                exit = true;
                break;
            /* MOV - move from register to register */
            case 0x03:
                exit = true;
                break;
            default:
            EXCEPTION:
                vmPrintf("vCPU ERROR!");
                exit = true;
        }
    }
}

void VMCPU::vmPrintf(const char *s)
{
    std::cout << s << std::endl;
}