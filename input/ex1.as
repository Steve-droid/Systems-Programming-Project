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
lea f1,r5
MAIN:   addition r3, LIST
LOOP:   prn    #48
add: mov *r6, k
cmp    r3, #-6
macr mac_miller
bne    END
lea    STR, r6
inc    r0
mov    *r6, K
sub r, r4
sub    r1, r4
dec    K
endmacr
jmp    LOOP
END:    stop
STR :    .string "abcd"
LIST:   .data  6, -9
.data  -100
K:      .data  31
add r1, r2
.extern f1
