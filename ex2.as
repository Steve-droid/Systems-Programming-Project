.extern function
MAIN:   add r3, LIST
jsr function
LOOP:   prn #48
lea STR, r6
inc r6
mov *r6, L3
sub r1, r4
cmp r3, #-6
bne END
add r7, *r6
clr K
sub L3, L3
.entry MAIN
jmp LOOP
END:    stop
STR:    .string "Hi Mom!"
LIST:   .data -10,-5,5,10,15,20
        .data -100, 1, 2, 3,4,5
K:      .data 31
.extern L3
