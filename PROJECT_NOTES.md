# Project Notes For Assembly Translator

## Directory Structure

project_root/
├── include/
│   ├── common.h
│   ├── lexer.h
│   ├── parser.h
│   ├── codegen.h
│   ├── symbol_table.h
│   └── error.h
├── src/
│   ├── main.c
│   ├── lexer.c
│   ├── parser.c
│   ├── codegen.c
│   ├── symbol_table.c
│   └── error.c
├── utils/
│   ├── utils.h
│   └── utils.c
├── Makefile
└── README.md

## Project Structre

### Hardware

* CPU

* Registers

* Memory

#### Registers

8 general purpose registers

* r0, r1, r2, r3, r4, r5, r6, r7

* Each register is 15 bits: [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14]

* LSB is at index 0

* MSB is at index 14

* 1 PSW(Program Status Word) register constaining flags that represent the state of the CPU

#### Memory

* Memory size: 4096 cells, each cell is 15 bits

* Each cell in the memory is called a **word**

### Software

* Arithmetics are done in 2's complement

#### Instruction Structure

* Each instruction is 15 bits long

*Each instruction is assembled into at most 3 words

#### Opcode

* 16 total opcodes
* mov,cmp,add,sub,lea,clr,not,inc,dec,jmp,bne,red,prn,jsr,rts,stop.
* Decimal values: [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]
* Binary values: [0000,0001,0010,0011,0100,0101,0110,0111,1000,1001,1010,1011,1100,1101,1110,1111]

#### Addressing Methods

*To use addressing methods, each operand in the instruction can contribute an additional word.
 The additional word will contain different values depending on the addressing method the operand uses.

* 4 addressing methods for source operand

* 4 addressing methods for destination operand

* If addressing mehod i is used, the i-th bit is set to 1, otherwise it is set to 0

* i is in range [3,2,1,0]

* Addressing methods: [Immediate, Direct, Indirect Register, Direct Register]

##### Immediate Addressing

* Example: `mov #-1,-2`

* Syntax:

**Operand that uses Immediate addressing will begin with a '#' character
**Operand that uses Immediate addressing is always a whole decimal number

* Additonal Word

** Bits [14,13,12,11,10,9,8,7,6,5,4,3] will contain the value of the immediate operand(two's complement)

** Bits [2,1,0] will contain A.R.E field(Absolute = 1, Relocatable = 0, External = 0)

##### Direct Addressing

* Example:
 ``
 x: .data 23

 dec x
 ``

* Syntax:
**Operand that uses direct addressing is always a label
**The label is one that was already defined, or will be defined at a later point in the file. Meaning that the label is an internal label.
**Direct addressing is a way to define a variable.
In the exaample above, x is a variable that holds the value 23.
** Direct addressing is used to access the value of the variable.
** Another way to use direct addressing is to use a label that was defined in another file. This is called external label.

* Additional Word

** Bits [14,13,12,11,10,9,8,7,6,5,4,3] will contain the address of the label in the symbol table

** Bits [2,1,0] will contain A.R.E field
If the label is an internal label, the A.R.E field will be [0,1,0]- Relocatable
If the label is an external label, the A.R.E field will be [0,0,1]- External

##### Indirect Register Addressing

* Example: `inc *r1`

* Anoter Example: `mov *r1,*r2`

* Syntax:

**Operand that uses indirect register addressing will always be a register
**Operand that uses indirect register addressing will begin with a '*' character
**The register that is used in the operand will point to a memory cell
**This is similar to the way that a pointer works in C
**Indirect register addressing is a way to access the value stored in the memory cell that the register points to

* Additional Word

If the operand is a destination operand:

** Bits [14,13,12,11,10,9,8,7,6] are all set to 0

** Bits [5,4,3] will contain the register number

** Bits [2,1,0] will contain A.R.E field: [1,0,0]- Absolute

If the operand is a source operand:

** Bits [14,13,12,11,10,9] and [5,4,3] are all set to 0

** Bits [8,7,6] will contain the register number

** Bits [2,1,0] will contain A.R.E field: [1,0,0]- Absolute

If an instruction uses indirect register addressing for both operands, they will share the same additional word

** Bits [14,13,12,11,10,9] are all set to 0

** Bits [8,7,6] will contain the register number of the source operand

** Bits [5,4,3] will contain the register number of the destination operand

** Bits [2,1,0] will contain A.R.E field: [1,0,0]- Absolute

In the example `inc *r1`, the 'inc' instruction is used to increment the value stored in the memory cell that r1 points to by 1.

In the example `mov *r1,*r2`, the 'mov' instruction is used to move the value stored in the memory cell that r1 points to to the memory cell that r2 points to. Here both operands use indirect register addressing so they share the same additional word.

##### Direct Register Addressing

* Example: `clr r1`

* Another Example: `mov r1,*r2`

* Syntax:

**Operand that uses direct register addressing will always be a register
**Direct register addressing is a way to access the value stored in the register
**The difference between direct register addressing and indirect register addressing is that direct register addressing accesses the value stored in the register itself, while indirect register addressing accesses the value stored in the memory cell that the register points to.

if the operand is a destination operand:

** Bits [14,13,12,11,10,9,8,7,6] are all set to 0

** Bits [5,4,3] will contain the register number

** Bits [2,1,0] will contain A.R.E field: [1,0,0]- Absolute

If the operand is a source operand:

** Bits [14,13,12,11,10,9] and [5,4,3] are all set to 0

** Bits [8,7,6] will contain the register number

** Bits [2,1,0] will contain A.R.E field: [1,0,0]- Absolute

If an instruction uses direct register addressing for both operands, they will share the same additional word

** Bits [14,13,12,11,10,9] are all set to 0

** Bits [8,7,6] will contain the register number of the source operand

** Bits [5,4,3] will contain the register number of the destination operand

** Bits [2,1,0] will contain A.R.E field: [1,0,0]- Absolute

#### Instruction Format- First Word

* Structure of the first word of an instruction is the same for all instructions

* The first word of an instruction is divided into 4 parts:

* Opcode: bits 14-11: [14,13,12,11]

* Addressing Method for source operand: bits 10-8: [10,9,8,7]

* Addressing Method for destination operand: bits 7-5: [6,5,4,3]

* A.R.E Field: bits 4-3: [2,1,0] (A- Absolute, R- Relocatable, E- External)

## Todo

* [ ] Memory managment for pre assembler
* [ ] Memory managment for keyword table
* [ ] Memory managment for label table
* [ ] Memory managment for lexer
* [ ] Memory managment for parser
