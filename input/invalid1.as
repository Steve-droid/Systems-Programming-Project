;mov with 0 args
mov

;mov with only 1 arg
mov *r6

;mov with immediate addressing method for destination operand
mov *r6, #-2

;cmp with 0 args
cmp

;cmp with 1 arg
cmp r3

;add with 0 args
add

;add with 1 arg
add #9

;add with immediate dest operand
add #9, #8

;data defined not at end of file will move to end in .ob file as needed
STR :    .string "abcd"
LIST:    .data  6, -9
         .data  -100
K:       .data  31


;.entry label that is not defined in this file
.entry MISSING
bne MISSING
lea MISSING, r1

