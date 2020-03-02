#include "../include/vmcpu.hpp"

//#define V_DEBUG

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
    BYTE bTmp_0, bTmp_1, bTmp_2;
    WORD wTmp_0, wTmp_1, wTmp_2;

    while(!exit)
    {
        opcode = AS->data[REGS->PC++];
        switch(opcode)
        {
            /* NOP */
            case 0x00:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] NOP" << std::endl;
                #endif
                opcode+=20;
                break;
            case 0x01:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] NOPV" << std::endl;
                #endif
                opcode*=5;
                break;
            /* EC - end of code */
            case 0xEC:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] EC" << std::endl;
                #endif
                exit = true;
                break;
            /*  ********************************
                            MOV
                ******************************** 
            */
            /*
                MOV - move from register to register
                02 25 => MOV R2, R5
                02 00 => MOV R0, R0
            */
            case 0x02:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] MOV" << std::endl;
                #endif
                bTmp_0 = AS->data[REGS->PC++];
                if((bTmp_0 & 0xF0) <= 0x60 && (bTmp_0 & 0x0F) <= 5){
                    // bTmp_0 == XXXX XXXX, XXXX XXXX & 0xF0 == XXXX 0000
                    REGS->R[(bTmp_0 & 0xF0) >> 4] = REGS->R[bTmp_0 & 0x0F];
                }
                else goto EXCEPTION;
                break;
            /* 
                MOVMB - move and extend byte from memory to register 
                02 03 04 01 => MOVMB R3, 0104
            */
            case 0x03:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] MOVMB" << std::endl;
                #endif
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
                04 03 04 01 => MOVMW R3, 0104
            */
            case 0x04:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] MOVMW" << std::endl;
                #endif
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                wTmp_0 = *(WORD*) &AS->data[REGS->PC];
                if(wTmp_0 >= sizeof(AS->data)) goto EXCEPTION;
                REGS->PC += 2;
                REGS->R[bTmp_0] = *(WORD*) &AS->data[wTmp_0];
                break;  
            /* 
                MOVB - move and extend byte to register 
                05 02 43 => MOVB R2, 43
            */
            case 0x05:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] MOVB" << std::endl;
                #endif
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                REGS->R[bTmp_0] = 0;
                *(BYTE *) &REGS->R[bTmp_0] = AS->data[REGS->PC++];
                break; 
            /* 
                MOVW - move word to register 
                06 01 15 28 => MOVW R1, 2815
            */
            case 0x06:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] MOVW" << std::endl;
                #endif
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                REGS->R[bTmp_0] = *(WORD *) &AS->data[REGS->PC];
                REGS->PC += 2;
                break; 
            /* 
                MOVBM - move byte from register to memory location 
                07 04 43 13 => MOVBM 1343, R4
            */
            case 0x07:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] MOVBM" << std::endl;
                #endif
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                wTmp_0 = *(WORD*) &AS->data[REGS->PC];
                if(wTmp_0 >= sizeof(AS->data)) goto EXCEPTION;
                REGS->PC += 2;
                AS->data[wTmp_0] = *(BYTE*) &REGS->R[bTmp_0];
                break;  
            /* 
                MOVWM - move word from register to memory location 
                08 04 43 13 => MOVWM 1343, R4
            */
            case 0x08:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] MOVWM" << std::endl;
                #endif
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
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] MOVMRB" << std::endl;
                #endif
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                bTmp_1 = AS->data[REGS->PC++];
                if(bTmp_1 > 5) goto EXCEPTION;
                if(REGS->R[bTmp_1] >= sizeof(AS->data)) goto EXCEPTION;
                REGS->R[bTmp_0] = 0;
                *(BYTE*) &REGS->R[bTmp_0] = AS->data[REGS->R[bTmp_1]];
                break;
            /* 
                MOVMRW - move word from memory to register
                        get addr from register
                0A 02 01 => MOVMRW R2, R1
            */
            case 0x0A:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] MOVMRW" << std::endl;
                #endif
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                bTmp_1 = AS->data[REGS->PC++];
                if(bTmp_1 > 5) goto EXCEPTION;
                if(REGS->R[bTmp_1] >= sizeof(AS->data)) goto EXCEPTION;
                REGS->R[bTmp_0] = *(WORD*) &AS->data[REGS->R[bTmp_1]];
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
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] JMP" << std::endl;
                #endif
                wTmp_0 = *(WORD*) &AS->data[REGS->PC];
                REGS->PC += 2;
                if(wTmp_0 > sizeof(AS->data)) goto EXCEPTION;
                REGS->PC = wTmp_0;
                break; 
            /*
                JZ - jump if equal
                12 15 00 => JZ 0015
            */
            case 0x12:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] JZ" << std::endl;
                #endif
                wTmp_0 = *(WORD*) &AS->data[REGS->PC];
                REGS->PC += 2;
                if(wTmp_0 > sizeof(AS->data)) goto EXCEPTION;
                if(REGS->ZF) REGS->PC = wTmp_0;
                break;
            /*
                JNZ - jump if not equal
                13 15 00 => JNZ 0015
            */
            case 0x13:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] JNZ" << std::endl;
                #endif
                wTmp_0 = *(WORD*) &AS->data[REGS->PC];
                REGS->PC += 2;
                if(wTmp_0 > sizeof(AS->data)) goto EXCEPTION;
                if(!REGS->ZF) REGS->PC = wTmp_0;
                break;
            /*
                JAE - jump if above or equal
                14 15 00 => JAE 0015
            */
            case 0x14:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] JAE" << std::endl;
                #endif
                wTmp_0 = *(WORD*) &AS->data[REGS->PC];
                REGS->PC += 2;
                if(wTmp_0 > sizeof(AS->data)) goto EXCEPTION;
                if(REGS->ZF || !REGS->CF) REGS->PC = wTmp_0;
                break;
            /*
                JBE - jump if below or equal
                15 15 00 => JBE 0015
            */
            case 0x15:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] JBE" << std::endl;
                #endif
                wTmp_0 = *(WORD*) &AS->data[REGS->PC];
                REGS->PC += 2;
                if(wTmp_0 > sizeof(AS->data)) goto EXCEPTION;
                if(REGS->ZF || REGS->CF) REGS->PC = wTmp_0;
                break;
            /*
                JB - jump if below
                16 15 00 => JB 0015
            */
            case 0x16:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] JB" << std::endl;
                #endif
                wTmp_0 = *(WORD*) &AS->data[REGS->PC];
                REGS->PC += 2;
                if(wTmp_0 > sizeof(AS->data)) goto EXCEPTION;
                if(!REGS->ZF && REGS->CF) REGS->PC = wTmp_0;
                break;
            /*
                JA - jump if above
                17 15 00 => JA 0015
            */
            case 0x17:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] JA" << std::endl;
                #endif
                wTmp_0 = *(WORD*) &AS->data[REGS->PC];
                REGS->PC += 2;
                if(wTmp_0 > sizeof(AS->data)) goto EXCEPTION;
                if(!REGS->ZF && !REGS->CF) REGS->PC = wTmp_0;
                break;
            /*  ********************************
                            ARITHMETIC
                ********************************
            */
            /*
                ADVR - Add value to register
                21 02 10 00 => ADVR R2, 10
            */
            case 0x21:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] ADVR" << std::endl;
                #endif
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                wTmp_0 = *(WORD*) &AS->data[REGS->PC];
                REGS->PC += 2;
                wTmp_1 = REGS->R[bTmp_0] + wTmp_0;          
                if(wTmp_1 == 0) REGS->ZF = 1;
                else REGS->ZF = 0;
                if(wTmp_1 < REGS->R[bTmp_0]) REGS->CF = 1;
                else REGS->CF = 0;
                REGS->R[bTmp_0] = wTmp_1;
                break;
            /*
                ADRR - Add two registers
                        and save result in first
                22 02 01 => ADRR R2, R1
            */
            case 0x22:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] ADRR" << std::endl;
                #endif
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                bTmp_1 = AS->data[REGS->PC++];
                if(bTmp_1 > 5) goto EXCEPTION;
                wTmp_0 = REGS->R[bTmp_0];
                wTmp_1 = REGS->R[bTmp_1];
                wTmp_2 = wTmp_0 + wTmp_1;
                REGS->R[bTmp_0] = wTmp_2;
                if(wTmp_2 == 0) REGS->ZF = 1;
                else REGS->ZF = 0;
                if(wTmp_2 < wTmp_0) REGS->CF = 1;
                else REGS->CF = 0;
                break;
            /*
                ADRRL - Add two registers (low byte)
                        and save result in first
                23 02 01 => ADRR R2, R1
            */
            case 0x23:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] ADRRL" << std::endl;
                #endif
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                bTmp_1 = AS->data[REGS->PC++];
                if(bTmp_1 > 5) goto EXCEPTION;
                bTmp_2 = *(BYTE *) &REGS->R[bTmp_0];
                bTmp_1 = *(BYTE *) &REGS->R[bTmp_1];
                bTmp_1 += bTmp_2;
                *(BYTE *) &REGS->R[bTmp_0] = bTmp_1;
                if(bTmp_1 == 0) REGS->ZF = 1;
                else REGS->ZF = 0;
                if(bTmp_1 < bTmp_2) REGS->CF = 1;
                else REGS->CF = 0;
                break;
            /*
                SUBVR - Substract value from register
                24 02 10 00 => SUBVR R2, 10
            */
            case 0x24:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] SUBVR" << std::endl;
                #endif
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                wTmp_0 = *(WORD*) &AS->data[REGS->PC];
                REGS->PC += 2;
                wTmp_1 = REGS->R[bTmp_0] - wTmp_0;          
                if(wTmp_1 == 0) REGS->ZF = 1;
                else REGS->ZF = 0;
                if(wTmp_1 > REGS->R[bTmp_0]) REGS->CF = 1;
                else REGS->CF = 0;
                REGS->R[bTmp_0] = wTmp_1;
                break;
            /*
                SUBRR - Substract two registers
                        and save result in first
                25 02 01 => SUBRR R2, R1
            */
            case 0x25:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] SUBRR" << std::endl;
                #endif
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                bTmp_1 = AS->data[REGS->PC++];
                if(bTmp_1 > 5) goto EXCEPTION;
                wTmp_0 = REGS->R[bTmp_0];
                wTmp_1 = REGS->R[bTmp_1];
                wTmp_2 = wTmp_0 - wTmp_1;
                REGS->R[bTmp_0] = wTmp_2;
                if(wTmp_2 == 0) REGS->ZF = 1;
                else REGS->ZF = 0;
                if(wTmp_2 > wTmp_0) REGS->CF = 1;
                else REGS->CF = 0;
                break;
            /*
                SUBRRL - Substract two registers (low byte)
                        and save result in first
                26 02 01 => ADRR R2, R1
            */
            case 0x26:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] SUBRRL" << std::endl;
                #endif
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                bTmp_1 = AS->data[REGS->PC++];
                if(bTmp_1 > 5) goto EXCEPTION;
                bTmp_2 = *(BYTE *) &REGS->R[bTmp_0];
                bTmp_1 = *(BYTE *) &REGS->R[bTmp_1];
                bTmp_1 -= bTmp_2;
                *(BYTE *) &REGS->R[bTmp_0] = bTmp_1;
                if(bTmp_1 == 0) REGS->ZF = 1;
                else REGS->ZF = 0;
                if(bTmp_1 > bTmp_2) REGS->CF = 1;
                else REGS->CF = 0;
                break;
            /*
                XOR - Xor two registers
                    and save result in first
                27 02 01 => XOR R2, R1
            */
            case 0x27:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] XOR" << std::endl;
                #endif 
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                bTmp_1 = AS->data[REGS->PC++];
                if(bTmp_1 > 5) goto EXCEPTION;
                wTmp_0 = REGS->R[bTmp_0];
                wTmp_1 = REGS->R[bTmp_1];
                wTmp_2 = wTmp_0 ^ wTmp_1;
                if(wTmp_2 == 0) REGS->ZF = 1;
                else REGS->ZF = 0;
                REGS->R[bTmp_0] = wTmp_2;
                REGS->CF = 0;
                break;
            /*
                XORL - Xor two registers (lower bytes)
                    and save result in first
                28 02 01 => XOR R2, R1
            */
            case 0x28:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] XORL" << std::endl;
                #endif 
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                bTmp_1 = AS->data[REGS->PC++];
                if(bTmp_1 > 5) goto EXCEPTION;
                bTmp_2 = REGS->R[bTmp_0];
                bTmp_1 = REGS->R[bTmp_1];
                bTmp_2 ^= bTmp_1;
                if(bTmp_2 == 0) REGS->ZF = 1;
                else REGS->ZF = 0;
                *(BYTE *) &REGS->R[bTmp_0] = bTmp_2;
                REGS->CF = 0;
                break;
            /*
                NOT - Bitwise not on value in a register
                    and save result in this register
                29 02 => NOT R2
            */
            case 0x29:
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                REGS->R[bTmp_0] = ~ REGS->R[bTmp_0];
                break;
            /*
                NOTB - Bitwise not on value in a register (lower bytes)
                    and save result in this register
                29 02 => NOT R2
            */
            case 0x2A:
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                *(BYTE *) &REGS->R[bTmp_0] = ~ (*(BYTE *) &REGS->R[bTmp_0]);
                break;
            /*  ********************************
                            COMPARE
                ********************************
            */
            /*
                CMP - compare two registers
                31 02 01 => CMP R2, R1
            */
            case 0x31:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] CMP" << std::endl;
                #endif
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                bTmp_1 = AS->data[REGS->PC++];
                if(bTmp_1 > 5) goto EXCEPTION;
                wTmp_0 = REGS->R[bTmp_0];
                wTmp_1 = REGS->R[bTmp_1];
                if(wTmp_1 == wTmp_0) REGS->ZF = 1;
                else REGS->ZF = 0;
                if(wTmp_1 > wTmp_0) REGS->CF = 1;
                else REGS->CF = 0;
                break;
            /*
                CMPL - compare two registers (lower byte)
                32 02 01 => CMP R2, R1
            */
            case 0x32:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] CMPL" << std::endl;
                #endif
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                bTmp_1 = AS->data[REGS->PC++];
                if(bTmp_1 > 5) goto EXCEPTION;
                bTmp_0 = *(BYTE*) &REGS->R[bTmp_0];
                bTmp_1 = *(BYTE*) &REGS->R[bTmp_1];
                if(bTmp_0 == bTmp_1) REGS->ZF = 1;
                else REGS->ZF = 0;
                if(bTmp_0 < bTmp_1) REGS->CF = 1;
                else REGS->CF = 0;
                break;
            /*  ********************************
                            STACK
                ********************************
            */
            /*
                PUSH REGISTER
                61 03 => PUSH R3
            */
            case 0x61:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] PUSH" << std::endl;
                #endif
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                REGS->SP -= 1;
                if(REGS->SP == 0xFFFF) {
                    #ifdef _VM_TEST_
                        vcpuFlag = VCpuFlag::OVERFLOW;
                    #endif
                    #ifndef _VM_TEST_
                        std::cout << "[ERROR] STACK OVERFLOW!" << std::endl;
                    #endif
                    goto EXCEPTION;
                }
                AS->stack[REGS->SP] = REGS->R[bTmp_0];
                break;
             /*
                POP TO A REGISTER
                62 03 => POP R3
            */
            case 0x62:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] POP" << std::endl;
                #endif
                bTmp_0 = AS->data[REGS->PC++];
                if(bTmp_0 > 5) goto EXCEPTION;
                if(&AS->stack[REGS->SP] == &AS->stack[sizeof(AS->stack)/sizeof(WORD)]){
                    #ifdef _VM_TEST_
                        vcpuFlag = VCpuFlag::UNDERFLOW;
                    #endif
                    #ifndef _VM_TEST_
                        std::cout << "[ERROR] STACK UNDERFLOW!" << std::endl;
                    #endif
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
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] POC" << std::endl;
                #endif
                if(&AS->stack[REGS->SP] == &AS->stack[sizeof(AS->stack)/sizeof(WORD)]){
                    #ifdef _VM_TEST_
                        vcpuFlag = VCpuFlag::UNDERFLOW;
                    #endif
                    #ifndef _VM_TEST_
                        std::cout << "[ERROR] STACK UNDERFLOW!" << std::endl;
                    #endif
                    goto EXCEPTION;
                }
                bTmp_0 = *(BYTE*) &AS->stack[REGS->SP++];
                vmPrint(bTmp_0);
                break;
            /*
                POCN - Print char with new line
                    the value must be at the top of
                    the stack
                42 => POCN
            */
            case 0x52:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] POCN" << std::endl;
                #endif
                if(&AS->stack[REGS->SP] == &AS->stack[sizeof(AS->stack)/sizeof(WORD)]){
                    #ifdef _VM_TEST_
                        vcpuFlag = VCpuFlag::UNDERFLOW;
                    #endif
                    #ifndef _VM_TEST_
                        std::cout << "[ERROR] STACK UNDERFLOW!" << std::endl;
                    #endif
                    goto EXCEPTION;
                }
                bTmp_0 = *(BYTE*) &AS->stack[REGS->SP++];
                vmPrintN(bTmp_0);
                break;
            /*  
                ********************************
                            DEFAULT
                ********************************
            */
            default:
                #ifdef V_DEBUG
                    std::cout << "[DEBUG] DEFAULT" << std::endl;
                #endif
                EXCEPTION:
                    #ifdef _VM_TEST_
                        if(vcpuFlag == VCpuFlag::OK) vcpuFlag = VCpuFlag::ERROR;
                    #endif
                    #ifndef _VM_TEST_
                        std::cout << "[ERROR] vCPU CRASH!" << std::endl;
                    #endif
                    exit = true;
        }
    }
}

void VMCPU::vmPrint(BYTE s)
{
    std::cout << s;
}

void VMCPU::vmPrintN(BYTE s)
{
    std::cout << s << std::endl;
}