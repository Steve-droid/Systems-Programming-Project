; file ps.as

.entry LIST
.extern fn1
MAIN:    add r3, LIST
         jsr fn1
LOOP:    prn #48
         lea STR, r6
         inc r6
STR:     .string "abcd"
         .data -100
         mov *r4, L3
         sub r1, r4
         cmp r3, #-6
         bne END
         add r7, *r6
         clr K
         sub L3, L3
.entry MAIN
         jmp LOOP
END:     stop
LIST:    .data 6,-9
K:       .data 31
.extern L3