#include <stdlib.h>
#include <stdio.h>

#include "stack.h"
#include "code_bin.h"

#define PRINT_ERROR printf("%s:%d ERROR in function %s", __FILE__, __LINE__, __FUNCTION__)
#define NUM_ELEMS 100

enum ERROR {
    OK = 0,
    ERROR_READ_TERM = 10,

    // ошибки работы с файлом
    ERROR_FILE_OPEN = 20,
    ERROR_FILE_SCANF,

    // ошибки работы с памятью
    ERROR_MEM_ALLOC = 30,

    // ошибки выполнения команд .bin
    ERROR_CMD = 40
};


ERROR read_bin(const char* const file_name, int* buffer, size_t* const buf_len);
ERROR run_bin(const int* const buffer, const int buf_size);



int main(int argc, char* argv[]) {
    if (argc != 2) {
        PRINT_ERROR;
        return ERROR_READ_TERM;
    } 

    // считать данные из .bin файла
    int* buffer = (int*)calloc(NUM_ELEMS, sizeof(int));
    if (buffer == NULL) {
        PRINT_ERROR;
        return ERROR_MEM_ALLOC;
    }

    size_t buf_size = 0;
    read_bin(argv[1], buffer, &buf_size);

    run_bin(buffer, buf_size);

    return 0;
    
}


ERROR read_bin(const char* const file_name, int* buffer, size_t* const buf_len) {
    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        PRINT_ERROR;
        return ERROR_FILE_OPEN;
    } 

    int cmd = 0;
    int param = 0;
    int flag = 0;
    while((flag = fscanf(file, "%d %d\n", &cmd, &param)) != EOF) {
        switch (flag) {
            case 1:
                // команда без параметров
                *buffer = cmd;
                buffer++;
                (*buf_len)++;
                break;
            case 2:
                // команда с 1 параметром
                *buffer = cmd;
                buffer++;
                *buffer = param;
                buffer++;
                (*buf_len) += 2;
                break;
            default:
                PRINT_ERROR;
                return ERROR_FILE_SCANF;
                break;
        }
    }

    // закрытие ресурсов
    fclose(file);
    return OK;
}


// выполнение программы
ERROR run_bin(const int* const buffer, const int buf_size) {
    stack_t stck;
    stack_init(&stck, 1);
    int out = 0;
    for (int i = 0; i < buf_size; i++) {
        switch (buffer[i]) {
            case CMD_PUSH:
                push(&stck, buffer[++i]);
                break;
            case CMD_OUT:
                pop(&stck, &out);
                printf("get from stack: %d\n", out);
                break;
            case CMD_HLT:
                // завершение программы
                return OK;
                break;
            case CMD_ADD:
//              add(&stack);
                break;
            case CMD_SUB:
                break;
            case CMD_DIV:
                break;
            case CMD_MUL:
                break;
            default:
                PRINT_ERROR;
                return ERROR_CMD;
                break;

        }
    }

    return OK;
}