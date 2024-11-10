; /*********************************************************************
; ------------------- DECLARATION OF AUTHORSHIP ------------------------
; I hereby declare that the source-code presented herein is my own work.
;     --- Author Name: SWAGATAM PATI
;     --- Roll No.: 2301AI28
; ----------------------------------------------------------------------
; *********************************************************************/

; calculates the sum of the first n numbers; i.e. 1 + 2 + ... + n
; here n = 50

call sigma
HALT

sigma:
    stl 0
    adj -1      ; make space in stack
    loop:
        ldc    n
        ldnl   0

        ldc    res
        ldnl   0

        add

        ldc    res
        stnl   0

        ldc    n
        ldnl   0

        adc    -1       ; A -= 1
        brz    stop     ; stop if A == 0

        ldc    n
        stnl   0

        br     loop
    stop:   
        adj    1        ; restore stack
        ldl    0
        return          ; return control to procedure caller

n:   data 50
res: data 0
