#include "../include/vmcpu.hpp"

VMCPU::VMCPU()
{
    AS = (PADDRESS_SPACE) new ADDRESS_SPACE;
    REGS = (PREGISTERSS) new REGISTERSS;
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
        switch(opcode)
        {
            /* NOP */
            case 0x00:
            case 0x01:
                break;
            /* MOV - move from register to register */
            case 0x02:
                ;
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