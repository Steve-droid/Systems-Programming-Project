MAIN:   add r3, LIST
dec    K
macr mac
LOOP:   prn    #48
cmp    r3  #-6
bne    END
lea    ST, r6
inc    r
endmacr
mov   *r4 K
sub    r1,r1
jmp    LOOP
END:    stop
mac
mac:    .string "abcd"
LIST:   .data  6, -9
.data  -100
mac:      .data  31
add r1, r2
.extern f1
