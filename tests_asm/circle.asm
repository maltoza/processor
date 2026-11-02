IN
POP RDX
PUSH 0
POP RAX
FOR_BEG_Y:
    PUSH RAX
    PUSH 30
    JE FOR_END_Y:

        PUSH 0
        POP RBX
        FOR_BEG_X:
            PUSH RBX
            PUSH 80
            JE FOR_END_X:

            PUSH RAX
            PUSH RBX
            PUSH RDX
            PUSH RAX
            PUSH RBX
            CALL CALC_R:
            POP RCX
            POP RDX
            POP RBX
            POP RAX

            PUSH RCX
            PUSH RDX
            PUSH RDX
            MUL
            JAE NOT_CIRC:
                PUSH RAX
                PUSH 80
                MUL
                PUSH RBX
                ADD
                POP RCX
                PUSH 56
                POPM RCX
            JMP EXIT_CIRC:

            NOT_CIRC:
                PUSH RAX
                PUSH 80
                MUL
                PUSH RBX
                ADD
                POP RCX
                PUSH 45
                POPM RCX

            EXIT_CIRC:
            PUSH RBX
            PUSH 1
            ADD
            POP RBX
        JMP FOR_BEG_X:
        FOR_END_X:
        
    PUSH RAX
    PUSH 1
    ADD
    POP RAX
JMP FOR_BEG_Y:
FOR_END_Y:
DRAW
HLT

CALC_R:
    POP RBX
    POP RAX
    PUSH RBX
    PUSH 39
    SUB
    PUSH 5
    DIV
    PUSH RBX
    PUSH 39
    SUB
    MUL
    PUSH RAX
    PUSH 14
    SUB
    PUSH RAX
    PUSH 14
    SUB
    MUL
    ADD
RET
