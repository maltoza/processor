#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "code_bin.h"

#define PRINT_ERROR printf("%s:%d ERROR in function %s", __FILE__, __LINE__, __FUNCTION__)
#define CMD_LEN_MAX 10  // максимальная длина строки
#define MAX_NUM_LAB 20  // максимальное количество меток

struct label {
    char name[CMD_LEN_MAX]; // имя метки
    int ptr;                // адрес перехода
};

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


ERROR read_asm(FILE* const file_asm, char* const cmd, char* const param, int* const flag);
ERROR convert_cmd(char* const cmd_asm, char* const param_asm, int* const cmd_bin, int* const param_bin);
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
    char param_asm[CMD_LEN_MAX];
    int cmd_bin = 0;
    int param_bin = 0;
    int flag = 0;
    //label* lbs = (label*)calloc(sizeof(label), MAX_NUM_LAB);

    //компиляция
    while(1) {
        read_asm(file_asm, cmd_asm, param_asm, &flag);
        if (flag == EOF) break;
        convert_cmd(cmd_asm, param_asm, &cmd_bin, &param_bin);
        write_bin(file_bin, cmd_bin, param_bin, flag);

    }

    return 0;
}
/*
ERROR read_labels(FILE* const file_asm, label(* lbs)[MAX_NUM_LAB]) {
    int flag = 0;
    char str[CMD_LEN_MAX];
    while(flag != EOF) {
        flag = scanf("%s\n", &str);
        if (str[0] == ':') {
            (*lbs)[0].name = str;

        }
        // запись метки в таблицу

    }
}
*/

ERROR read_asm(FILE* const file_asm, char* const cmd, char* const param, int* const flag) {

    char line[CMD_LEN_MAX];
    if (fgets(line, sizeof(line), file_asm) != 0) {
        *flag = sscanf(line, "%s %s\n", cmd, param);
    } else {
        *flag = EOF;
        return ERROR_FILE_END;
    }

    return OK;
}


ERROR convert_cmd(char* const cmd_asm, char* const param_asm,
                  int*  const cmd_bin, int*  const param_bin) {
    if (!strcmp(cmd_asm, "PUSH")) {
        if ((param_asm[0] >= '0') && (param_asm[0] <= '9'))  {
            // число
            *cmd_bin = CMD_PUSH_NUMBER;
            *param_bin = atoi(param_asm);
        } else {
            // регистр
            *cmd_bin = CMD_PUSH;
            *param_bin = toupper(param_asm[1]) - 'A' + 1;
        }
    }
    else if (!strcmp(cmd_asm, "POP")) {
        // регистр
        *cmd_bin = CMD_POP;
        *param_bin = toupper(param_asm[1]) - 'A' + 1;
    }
    else if (!strcmp(cmd_asm, "OUT")) {
        *cmd_bin = CMD_OUT;
        *param_bin = atoi(param_asm);
    }
    else if (!strcmp(cmd_asm, "HLT")) {
        *cmd_bin = CMD_HLT;
        *param_bin = atoi(param_asm);
    }
    else if (!strcmp(cmd_asm, "ADD")) {
        *cmd_bin = CMD_ADD;
        *param_bin = atoi(param_asm);
    }
    else if (!strcmp(cmd_asm, "SUB")) {
        *cmd_bin = CMD_SUB;
        *param_bin = atoi(param_asm);
    }
    else if (!strcmp(cmd_asm, "DIV")) {
        *cmd_bin = CMD_DIV;
        *param_bin = atoi(param_asm);
    }
    else if (!strcmp(cmd_asm, "MUL")) {
        *cmd_bin = CMD_MUL;
        *param_bin = atoi(param_asm);
    }
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