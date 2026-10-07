#ifndef CODE_BIN_H
#define CODE_BIN_H

// коды команд виртуальной машины
enum CMD {
    CMD_ERROR = 0,
    CMD_PUSH,
    CMD_OUT,
    CMD_HLT,
    CMD_ADD,
    CMD_SUB,
    CMD_DIV,
    CMD_MUL
};

#endif // CODE_BIN_H