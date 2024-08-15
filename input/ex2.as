.entry LIST
.extern fn1
.extern L3
MAIN: add r3, LIST
add r3, fn1
jmp L3
LOOP: prn #48
.entry MAIN
END: stop
LIST: .data 6,-9
K: .data 31
LABE&: add r1
jmp LABE&
