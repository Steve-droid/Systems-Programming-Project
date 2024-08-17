MAIN: add r3, LIST
LOOP: prn    #48
cmpare r3  #-6
bne END,
lea ST r9
inc r
mov *r9, K
subr1
dec K 2,
jmp to LOOP
END: stop .
ST<R34: .string "abcd"
