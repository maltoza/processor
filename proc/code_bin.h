#ifndef CODE_BIN_H
#define CODE_BIN_H

// коды команд виртуальной машины
enum CMD {
    CMD_JMP,
    CMD_JA,
    CMD_JAE,
    CMD_JB,
    CMD_JBE,
    CMD_JE,
    CMD_JNE,
    CMD_CALL,
    CMD_RET,
    CMD_PUSH_NUMBER,
    CMD_PUSH,
    CMD_PUSHM,
    CMD_POP,
    CMD_POPM,
    CMD_IN,
    CMD_OUT,
    CMD_ADD,
    CMD_SUB,
    CMD_MUL,
    CMD_DIV,
    CMD_SQRT,
    CMD_PRINT,
    CMD_DRAW,
    CMD_HLT
};

#endif // CODE_BIN_H