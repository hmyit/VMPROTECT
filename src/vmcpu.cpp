#include "../include/vmcpu.hpp"

VMCPU::VMCPU()
{
    AS = (PADDRESS_SPACE) new ADDRESS_SPACE;
    REGS = (PREGISTERSS) new REGISTERSS;

    memset(AS->data, 0, sizeof(AS->data));

    REGS->PC = 0;
    REGS->SP = sizeof(AS->stack) / sizeof(WORD);
}

VMCPU::~VMCPU()
{
    delete AS;
    delete REGS;
}

bool VMCPU::loadCode(BYTE *mcode, BYTE *usrInput, int sizeUserIn)
{
    unsigned int mcodeSize = sizeof(mcode)/sizeof(mcode[0]);

    if(mcodeSize > sizeof(AS->data)) 
        return false;
    memcpy(AS->data, mcode, mcodeSize);
    REGS->R[0] = (WORD) mcodeSize;
    memcpy(AS->data + mcodeSize, usrInput, sizeUserIn);
    return true;
}

void VMCPU::run()
{
    bool exit = false;
    BYTE opcode;
    BYTE bTmp_0, bTmp_1;
    WORD wTmp_0;

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
            /* 
                MOV - move from register to register
                02 25 => MOV R2,R5
                02 00 => MOV R0,R0
            */
            case 0x02:
                bTmp_0 = AS->data[REGS->PC++];
                if((bTmp_0 & 0xF0) <= 0x60 && (bTmp_0 & 0x0F) <= 5){
                    // bTmp_0 == XXXX XXXX, XXXX XXXX & 0xF0 == XXXX 0000
                    REGS->R[(bTmp_0 & 0xF0) >> 4] = REGS->R[bTmp_0 & 0x0F];
                }
                else goto EXCEPTION;
                break;
            /* 
                MOVMB - move and extend byte from memory to register 
                02 03 04 01 => MOVX R3,BYTE [0104]
            */
            case 0x03:
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                wTmp_0 = *(WORD*) &AS->data[REGS->PC];
                if(wTmp_0 >= sizeof(AS->data)) goto EXCEPTION;
                REGS->PC += 2;
                REGS->R[bTmp_0] = 0;
                *(BYTE*) &REGS->R[bTmp_0] = AS->data[wTmp_0];
                break;  
            /* 
                MOVMW - move word from memory to register 
                04 03 04 01 => MOV R3, WORD [0104]
            */
            case 0x04:
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                wTmp_0 = *(WORD*) &AS->data[REGS->PC];
                if(wTmp_0 >= sizeof(AS->data)) goto EXCEPTION;
                REGS->PC += 2;
                REGS->R[bTmp_0] = *(WORD*) &AS->data[wTmp_0];
                break;  
            /* 
                MOVB - move and extend byte to register 
                05 02 43 => MOVX R2, 43
            */
            case 0x05:
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                REGS->R[bTmp_0] = 0;
                *(BYTE *) &REGS->R[bTmp_0] = AS->data[REGS->PC++];
                break; 
            /* 
                MOVW - move word to register 
                06 01 15 28 => MOV R1, 2815
            */
            case 0x06:
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                REGS->R[bTmp_0] = *(WORD *) &AS->data[REGS->PC];
                REGS->PC += 2;
                break; 
            /* 
                MOVBM - move byte from register to memory location 
                07 04 43 13 => MOV BYTE [1343], R4
            */
            case 0x07:
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                wTmp_0 = *(WORD*) &AS->data[REGS->PC];
                if(wTmp_0 >= sizeof(AS->data)) goto EXCEPTION;
                REGS->PC += 2;
                AS->data[wTmp_0] = *(BYTE*) &REGS->R[bTmp_0];
                break;  
            /* 
                MOVWM - move word from register to memory location 
                08 04 43 13 => MOV WORD [1343], R4
            */
            case 0x08:
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                wTmp_0 = *(WORD*) &AS->data[REGS->PC];
                if(wTmp_0 >= sizeof(AS->data)) goto EXCEPTION;
                REGS->PC += 2;
                *(WORD*) &AS->data[wTmp_0] = REGS->R[bTmp_0];
                break;
            /* 
                MOVMRB - move and extend byte from memory to register
                        get addr from register
                09 02 01 => MOVMRB R2, R1
            */
            case 0x09:
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                bTmp_1 = AS->data[REGS->PC++];
                if(bTmp_1 > 5) goto EXCEPTION;
                wTmp_0 = REGS->R[bTmp_1];
                if(wTmp_0 >= sizeof(AS->data)) goto EXCEPTION;
                REGS->R[bTmp_0] = 0;
                *(BYTE*) &REGS->R[bTmp_0] = AS->data[wTmp_0];
                break;
            /* 
                MOVMRW - move word from memory to register
                        get addr from register
                0A 02 01 => MOVMRW R2, R1
            */
            case 0x0A:
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                bTmp_1 = AS->data[REGS->PC++];
                if(bTmp_1 > 5) goto EXCEPTION;
                wTmp_0 = REGS->R[bTmp_1];
                if(wTmp_0 >= sizeof(AS->data)) goto EXCEPTION;
                REGS->R[bTmp_0] = *(WORD*) &AS->data[wTmp_0];
                break;
            /*
                Unconditional Jump
                0B 15 00 => JMP 0015
            */
            case 0x0B:
                wTmp_0 = *(WORD*) &AS->data[REGS->PC];
                REGS->PC += 2;
                if(wTmp_0 > sizeof(AS->data)) goto EXCEPTION;
                REGS->PC = wTmp_0;
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