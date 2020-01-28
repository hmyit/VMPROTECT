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

bool VMCPU::loadCode(BYTE *mcode, int mcsize, BYTE *usrInput, int sizeUserIn)
{
    if((unsigned) mcsize > sizeof(AS->data)) 
        return false;
    memcpy(AS->data, mcode, mcsize);
    REGS->R[0] = (WORD) mcsize;
    memcpy(AS->data + mcsize, usrInput, sizeUserIn);
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
                std::cout << "[DEBUG] NOP" << std::endl;
                break;
            case 0x01:
                std::cout << "[DEBUG] NOPV" << std::endl;
                break;
            /* EC - end of code */
            case 0xEC:
                std::cout << "[DEBUG] EC" << std::endl;
                exit = true;
                break;
            /*  ********************************
                            MOV
                ******************************** 
            */
            /*
                MOV - move from register to register
                02 25 => MOV R2,R5
                02 00 => MOV R0,R0
            */
            case 0x02:
                std::cout << "[DEBUG] MOV" << std::endl;
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
                std::cout << "[DEBUG] MOVMB" << std::endl;
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
                std::cout << "[DEBUG] MOVMW" << std::endl;
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
                std::cout << "[DEBUG] MOVB" << std::endl;
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
                std::cout << "[DEBUG] MOVW" << std::endl;
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
                std::cout << "[DEBUG] MOVBM" << std::endl;
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
                std::cout << "[DEBUG] MOVWM" << std::endl;
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
                std::cout << "[DEBUG] MOVMRB" << std::endl;
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
                std::cout << "[DEBUG] MOVMRW" << std::endl;
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                bTmp_1 = AS->data[REGS->PC++];
                if(bTmp_1 > 5) goto EXCEPTION;
                wTmp_0 = REGS->R[bTmp_1];
                if(wTmp_0 >= sizeof(AS->data)) goto EXCEPTION;
                REGS->R[bTmp_0] = *(WORD*) &AS->data[wTmp_0];
                break;
            /*  ********************************
                            JUMP
                ********************************
            */
            /*
                JMP - unconditional jump
                11 15 00 => JMP 0015
            */
            case 0x11:
                std::cout << "[DEBUG] JMP" << std::endl;
                wTmp_0 = *(WORD*) &AS->data[REGS->PC];
                REGS->PC += 2;
                if(wTmp_0 > sizeof(AS->data)) goto EXCEPTION;
                REGS->PC = wTmp_0;
                break; 
            /*
                JZ - jump if equal
                12 15 00
            */
            case 0x12:
                std::cout << "[DEBUG] JZ" << std::endl;
                wTmp_0 = *(WORD*) &AS->data[REGS->PC];
                REGS->PC += 2;
                if(wTmp_0 > sizeof(AS->data)) goto EXCEPTION;
                if(REGS->ZF) REGS->PC = wTmp_0;
                break;
            /*
                JNZ - jump if not equal
                13 15 00
            */
            case 0x13:
                std::cout << "[DEBUG] JNZ" << std::endl;
                wTmp_0 = *(WORD*) &AS->data[REGS->PC];
                REGS->PC += 2;
                if(wTmp_0 > sizeof(AS->data)) goto EXCEPTION;
                if(!REGS->ZF) REGS->PC = wTmp_0;
                break;
            /*
                JAE - jump if above or equal
                14 15 00
            */
            case 0x14:
                std::cout << "[DEBUG] JAE" << std::endl;
                wTmp_0 = *(WORD*) &AS->data[REGS->PC];
                REGS->PC += 2;
                if(wTmp_0 > sizeof(AS->data)) goto EXCEPTION;
                if(REGS->ZF || !REGS->CF) REGS->PC = wTmp_0;
                break;
            /*
                JBE - jump if below or equal
                15 15 00
            */
            case 0x15:
                std::cout << "[DEBUG] JBE" << std::endl;
                wTmp_0 = *(WORD*) &AS->data[REGS->PC];
                REGS->PC += 2;
                if(wTmp_0 > sizeof(AS->data)) goto EXCEPTION;
                if(REGS->ZF || REGS->CF) REGS->PC = wTmp_0;
                break;
            /*
                JB - jump if below
                16 15 00
            */
            case 0x16:
                std::cout << "[DEBUG] JB" << std::endl;
                wTmp_0 = *(WORD*) &AS->data[REGS->PC];
                REGS->PC += 2;
                if(wTmp_0 > sizeof(AS->data)) goto EXCEPTION;
                if(!REGS->ZF && REGS->CF) REGS->PC = wTmp_0;
                break;
            /*
                JA - jump if above
                17 15 00
            */
            case 0x17:
                std::cout << "[DEBUG] JA" << std::endl;
                wTmp_0 = *(WORD*) &AS->data[REGS->PC];
                REGS->PC += 2;
                if(wTmp_0 > sizeof(AS->data)) goto EXCEPTION;
                if(!REGS->ZF && !REGS->CF) REGS->PC = wTmp_0;
                break;
            /*  ********************************
                            ARITHMETIC
                ********************************
            */

            /*  ********************************
                            COMPARE
                ********************************
            */

            /*  ********************************
                            STACK
                ********************************
            */
            /*
                PUSH REGISTER
                61 03 => PUSH R3
            */
            case 0x61:
                std::cout << "[DEBUG] PUSH" << std::endl;
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                REGS->SP -= 1;
                if(REGS->SP == 0xFFFF) {
                    std::cout << "[ERROR] STACK OVERFLOW!" << std::endl;
                    goto EXCEPTION;
                }
                AS->stack[REGS->SP] = REGS->R[bTmp_0];
                break;
             /*
                POP TO A REGISTER
                62 03 => POP R3
            */
            case 0x62:
                std::cout << "[DEBUG] POP" << std::endl;
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                if(&AS->stack[REGS->SP] == &AS->stack[sizeof(AS->stack)/sizeof(WORD)]){
                    std::cout << "[ERROR] STACK UNDERFLOW!" << std::endl;
                    goto EXCEPTION;
                }
                REGS->R[bTmp_0] = AS->stack[REGS->SP];
                REGS->SP += 1;
                break;
            /*  ********************************
                            IN/OUT
                ********************************
            */
            /*
                POC - Print char without new line
                    the value must be at the top of
                    the stack
                51 => POC
            */
            case 0x51:
                if(&AS->stack[REGS->SP] == &AS->stack[sizeof(AS->stack)/sizeof(WORD)]){
                    std::cout << "[ERROR] STACK UNDERFLOW!" << std::endl;
                    goto EXCEPTION;
                }
                wTmp_0 = AS->stack[REGS->SP++];
                vmPrint(wTmp_0);
                break;
            /*
                POCN - Print char with new line
                    the value must be at the top of
                    the stack
                42 => POCN
            */
            case 0x52:
                if(&AS->stack[REGS->SP] == &AS->stack[sizeof(AS->stack)/sizeof(WORD)]){
                    std::cout << "[ERROR] STACK UNDERFLOW!" << std::endl;
                    goto EXCEPTION;
                }
                wTmp_0 = AS->stack[REGS->SP++];
                vmPrint(wTmp_0);
                break;
            /*  
                ********************************
                            DEFAULT
                ********************************
            */
            default:
            EXCEPTION:
                std::cout << "[ERROR] vCPU CRASH!" << std::endl;
                exit = true;
        }
    }
}

void VMCPU::vmPrint(WORD s)
{
    std::cout << s;
}

void VMCPU::vmPrintN(WORD s)
{
    std::cout << s << std::endl;
}