MAIN:   add r3, LIST
LOOP:   prn    #48
macr macrom ac
cmp    r3, #-6
bne    END
endmacr
lea    STR, r6
inc    r8
mov    *r6, K
sub    r1, r4
m_macr
dec    K
jmp    LOOP
END:    stop
STR:    .string "abcd"
LIST:   .data  6, -9
.data  -100
K:      .data  31
add r1, r2
.extern f1
lea f1,r5
