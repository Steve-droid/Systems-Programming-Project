MAIN: add r3, LIST
LOOP: prn    #48
cmp r3  #-6
bne END
lea ST, r9
inc r-2
mov *r9, K
sub r1
dec K,2
jmp LOOP
END: stopit!
ST<R34: .string "abcd"
LIS`T: .data  6, -9
.data -100
K: .data  31
add r1, r2
.extern f1
