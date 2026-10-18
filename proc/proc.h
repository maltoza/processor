#ifndef PROC_H
#define PROC_H

#include <stdlib.h>
#include <stdio.h>

#include "./stack/stack.h"
#include "code_bin.h"

#define PROC_NUM_ELEMS 100


enum PROC_ERROR {
    PROC_OK = 0,
    PROC_HLT,
    PROC_ERROR_READ_TERM = 10,

    // ошибки работы с файлом
    PROC_ERROR_FILE_OPEN = 20,
    PROC_ERROR_FILE_SCANF,

    // ошибки работы с памятью
    PROC_ERROR_MEM_ALLOC = 30,

    // ошибки выполнения команд .bin
    PROC_ERROR_CMD_UNKNOWN = 40, // команда не определена
    PROC_ERROR_CMD_NO,
    PROC_ERROR_CMD,
};

// структура процессора
struct processor {
    // регистры общего назначения
    int r[5];

    // регистры специального назначения
    int ip;     // указатель на текущую команду

    // буфер программы
    int*   buffer;      // указатель на начало кода
    size_t buf_size;    // размер массива

    // стеки
    stck_t* stack_data;  // стек данных
    stck_t* stck_calls;
};

PROC_ERROR proc_init(processor* proc);
PROC_ERROR proc_destroy(processor* proc);
PROC_ERROR proc_load(processor* proc, const char* const file_name);
PROC_ERROR proc_run(processor* proc);

#endif //PROC_H