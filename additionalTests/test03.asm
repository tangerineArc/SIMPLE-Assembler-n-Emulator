; /*********************************************************************
; ------------------- DECLARATION OF AUTHORSHIP ------------------------
; I hereby declare that the source-code presented herein is my own work.
;     --- Author Name: SWAGATAM PATI
;     --- Roll No.: 2301AI28
; ----------------------------------------------------------------------
; *********************************************************************/

; multiplies two positive integers n1 and n2
; here n1 = 23 and n2 = 10
; res stores the multiplication result

loop:
    ldc  res	; load res address A -> res, B = 0
    ldnl 0	    ; A = value at res; A = res, B = 0

    ldc n1      ; B = res, A->n1
    ldnl 0      ; B = res, A = n1

    add         ; A = A + B = res + n1

    ldc  res	; load res address
    stnl 0	    ; store A value in res

    ldc  n2	    ; load n2 address
    ldnl 0	    ; A = value at n2

    adc  -1	    ; A -= 1
    brz stop	; stop if A == 0

    ldc  n2	    ; load n2 address
    stnl 0      ; store A value in n2

    br  loop
stop:
    HALT

n1:  data 23
n2:  data 10
res: data 0
