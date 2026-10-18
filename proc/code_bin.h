#ifndef CODE_BIN_H
#define CODE_BIN_H

// коды команд виртуальной машины
enum CMD {
    CMD_JMP,
    CMD_JA,
    CMD_CALL,
    CMD_RET,
    CMD_PUSH_NUMBER,
    CMD_PUSH,
    CMD_POP,
    CMD_IN,
    CMD_OUT,
    CMD_ADD,
    CMD_SUB,
    CMD_MUL,
    CMD_DIV,
    CMD_HLT
};

#endif // CODE_BIN_H