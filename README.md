# VMPROTECT
A code obfuscation method using virtual machines to protect a product. 

A virtual machine simulates a CPU along with a few other hardware components, allowing it to perform arithmetic, read and write to memory and interact with I/O devices. It can understand a machine language which you can use to program it. Virtual machines used in code obfuscation are completely different than common virtual machnines. They are very specific to the task of executing a few set of instructions. Each instruction is given a custom opcode (often generated at random).

## Documentation
The VM will simulate a fictional cpu (16-bit). It has a custom instrucion set compared to x86-64.

### Memory
The VM has 4,096 memory locations, each of which stores a 8-bit value (it can store a total of 4kb). The VM has stack, which is a separate data structure. The stack has 256 memory locations, each of which stores a 16-bit value (it can store a total of 512b).

```c++
typedef uint8_t BYTE;
typedef uint16_t WORD;

BYTE data[4096];
WORD stack[256];
```

### Registers
A register is a slot for storing value on the CPU. The VM has 8 total registers, each of which is 2 bytes (16 bits). The six of them are general purpose, one has designated role as program counter and another has role as stack pointer. The VM has also two regisers ZF (Zero Flag) and CF (Carry Flag). These two provide information about the most recently executed calculation (allows to check logical conditions such as *AND*).

```c++
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
```

### Instructions
An instruction is a command which tells the CPU (and the VM's cpu) to do some task, such compare two values. Instructions have both an opcode which indicates the kind of task to perform and a set of parameters which provide inputs to the task being performed.

```assembly
ADRR R2, R1 => 22 02 01
```

<details>
  <summary>Show full list of opcodes</summary>

OPCODE | Mnemonic and params | Description
--- | --- | ---
00  | NOP | No operation |
01  | NOPV | No operation |
EC  | EC | End of code and end of the VM's cpu |
02  | MOV r<sub>dst</sub>, r<sub>src</sub> | Move from a register to a register|
03  |  MOVMB r<sub>dst</sub>, addr<sub>src</sub> | Move and extend byte from memory to a register|
04  |  MOVMW r<sub>dst</sub>, addr<sub>src</sub> | Move word from memory to a register |
05  |  MOVB r<sub>dst</sub>, byte | Move and extend byte to a register  |
06  |  MOVW r<sub>dst</sub>, word | Move word to a register |
07  |  MOVBM addr<sub>dst</sub>, r<sub>src</sub> | Move byte from a register to memory location |
08  |  MOVWM addr<sub>dst</sub>, r<sub>src</sub> | Move word from a register to memory location |
09  |  MOVMRB r<sub>dst</sub>, r<sub>src</sub> | Move and extend byte from memory to a register; get an address from a register |
0A  |  MOVMRW r<sub>dst</sub>, r<sub>src</sub> | Move word from memory to a register; get an address from a register |
11  |  JMP addr | Unconditional jump |
12  |  JZ addr | Jump if equal; it set up PC to the specified location if the ZF is set (1) |
13  |  JNZ addr | Jump if not equal; it set up PC to the specified location if the ZF is not set (0) |
14  |  JAE addr | Jump if above or equal; it set up PC to the specified location if the ZF is set (1) and the CF is not set (0) |
15  |  JBE addr | Jump if below or equal; it set up PC to the specified location if the ZF is set (1) and the CF is set (1) |
16  |  JB addr | Jump if below; it set up PC to the specified location if the ZF is not set (0) and the CF is set (1) |
17  |  JA addr | Jump if above; it set up PC to the specified location if the ZF is not set (0) and the CF is not set (0) |
21  |  ADVR r<sub>dst</sub>, word | Add value to a register |
22  |  ADRR r<sub>dst</sub>, r<sub>src</sub> | Add two registers |
23  |  ADRRL r<sub>dst</sub>, r<sub>src</sub> | Add two registers (the low byte) |
24  |  SUBVR r<sub>dst</sub>, word | Substract value from a register |
25  |  SUBRR r<sub>dst</sub>, r<sub>src</sub> | Substract two registers |
26  |  SUBRRL r<sub>dst</sub>, r<sub>src</sub> | Substract two registers (the low byte) |
27  |  xOR r<sub>dst</sub>, r<sub>src</sub> | Xor two registers |
28  |  xOR r<sub>dst</sub>, r<sub>src</sub> | Xor two registers (the low byte) |
29  |  NOT r<sub>dst</sub>| Bitwise NOT on value in a register |
2A  |  NOT r<sub>dst</sub> | Bitwise NOT on value in a register (the low byte) |
31  |  CMP r<sub>dst</sub>, r<sub>src</sub> | Compare two registers |
32  |  CMPL r<sub>dst</sub>, r<sub>src</sub> | Compare two registers (the low byte) |
61  |  PUSH r<sub>src</sub> | Push value from a register to stack |
62  |  POP r<sub>dst</sub> | Pop value from stack to a register |
51  |  POC  | Print char without new line, the value must be at the top of the stack |
52  |  POCN  | Print char with new line, the value must be at the top of the stack |

</details>

## Solution

<details>
  <summary>Click to show!</summary>

  ```
  Answer: vmobfuscationisthebest
  ```
</details>