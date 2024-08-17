VALIDLABEL: stop

;Label syntax errors

MAIN : add r3, LIST
LOOP`: add r3, LIST
`END: stop
LI%ST: .data 6,-9
K1234&56: .data 31
add: add r1
jmp: VALIDLABEL 
