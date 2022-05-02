.global _cio

CIOV = $e456

_cio:
    tax             ; move IOCB value to X
    jsr CIOV        ; call CIOV
    tya             ; move CIOV result code to Accum
    rts