# Usr input (addr) saved in R0
# DATA LABEL
# JUMP LOCATION LABEL

    MOVW R1, 1DL@
    MOV R2, R1
    ADVR R2, 0001
    MOVW R3, 0000
    MOVW R4, 0000
5JLL:
    MOVMRB R3, R1
    PUSH R3
    POC
    ADVR R1, 0001
    ADVR R2, 0001
    MOVMRB R3, R2
    CMP R3, R4
    JZ 4JLL@
    JNZ 5JLL@
4JLL:
    MOVMRB R3, R1
    PUSH R3
    POCN
    EC

1DL:
    "PASS",0
2DL:
    "FAILED",0