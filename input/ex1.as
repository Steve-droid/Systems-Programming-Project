lea f1,r5
MAIN:   add r3, LIST
LOOP:   prn    #48
cmp    r3, #-6
bne    END
lea    STR, r6
inc    r0
mov    *r6, K
sub    r1, r4
dec    K
jmp    LOOP
END:    stop
STR:    .string "abcd"
LIST:   .data  6, -9
.data  -100
K:      .data  31
add r1, r2
.extern f1
