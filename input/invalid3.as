; file ps.as
.entry LIST
.extern fn1
MAIN:    add r3, LIST
jsr fn1
LOOP:    prn #48
lea STR, r6

;macro with inavalid 'endmacr'
macr mac_miller
cmp r3, #-6
bne END
add r7, *r6
endmacr hi mac

mac_miller

