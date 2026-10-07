#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "code_bin.h"

#define PRINT_ERROR printf("%s:%d ERROR in function %s", __FILE__, __LINE__, __FUNCTION__)
#define CMD_LEN_MAX 10

enum ERROR {
    OK = 0,
    ERROR_READ_TERM = 10,

    // ошибки работы с файлом
    ERROR_FILE_OPEN = 20,
    ERROR_FILE_SCANF,
    ERROR_FILE_END,
    ERROR_FILE_WRITE,

    // ошибки работы с памятью
    ERROR_MEM_ALLOC = 30,

    // ошибки выполнения команд .bin
    ERROR_CMD = 40,
    ERROR_CMD_UNKNOWN,
    ERROR_CMD_SIZE,
};


ERROR read_asm(FILE* const file_asm, char* const cmd, int* const param, int* const flag);
ERROR convert_cmd(char* const cmd_asm, int* const cmd_bin);
ERROR write_bin(FILE* const file_bin, const int cmd_bin, const int param, const int flag);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        PRINT_ERROR;
        return ERROR_READ_TERM;
    } 

    FILE* file_asm = fopen(argv[1], "r");
    if (file_asm == NULL) {
        PRINT_ERROR;
        return ERROR_FILE_OPEN;
    } 

    FILE* file_bin = fopen(argv[2], "w");
    if (file_bin == NULL) {
        PRINT_ERROR;
        return ERROR_FILE_OPEN;
    } 

    char cmd_asm[CMD_LEN_MAX];
    int cmd_bin = 0;
    int param = 0;
    int flag = 0;
    while(1) {
        read_asm(file_asm, cmd_asm, &param, &flag);
        if (flag == EOF) break;
        convert_cmd(cmd_asm, &cmd_bin);
        write_bin(file_bin, cmd_bin, param, flag);

    }

    return 0;
}


ERROR read_asm(FILE* const file_asm, char* const cmd, int* const param, int* const flag) {

    *flag = fscanf(file_asm, "%s %d\n", cmd, param);
    if (*flag == EOF) return ERROR_FILE_END;

    return OK;
}


ERROR convert_cmd(char* const cmd_asm, int* const cmd_bin) {
    if (!strcmp(cmd_asm, "PUSH")) *cmd_bin = CMD_PUSH;
    else if (!strcmp(cmd_asm, "OUT")) *cmd_bin = CMD_OUT;
    else if (!strcmp(cmd_asm, "HLT")) *cmd_bin = CMD_HLT;
    else if (!strcmp(cmd_asm, "ADD")) *cmd_bin = CMD_ADD;
    else if (!strcmp(cmd_asm, "SUB")) *cmd_bin = CMD_SUB;
    else if (!strcmp(cmd_asm, "DIV")) *cmd_bin = CMD_DIV;
    else if (!strcmp(cmd_asm, "MUL")) *cmd_bin = CMD_MUL;
    else {
        PRINT_ERROR;
        return ERROR_CMD_UNKNOWN;
    }

    return OK;
}


ERROR write_bin(FILE* const file_bin, const int cmd_bin, const int param, const int flag) {
    char string[CMD_LEN_MAX];
    if (flag ==  1) sprintf(string, "%d\n", cmd_bin);
    else if (flag == 2) sprintf(string, "%d %d\n", cmd_bin, param);
    else {
        PRINT_ERROR;
        return ERROR_CMD_SIZE;
    }

    size_t result = fwrite(string, strlen(string), 1, file_bin);
    if (result != 1) {
        PRINT_ERROR;
        return ERROR_FILE_WRITE;
    }

    return OK;
}