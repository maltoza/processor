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
    ERROR_CMD_UNKNOWN = 40, // команда не определена
    ERROR_CMD_NO
};

// структура процессора
struct processor {
    // регистры общего назначения
    int rax;    //
    int rbx;    //
    int rcx;    //
    int rdx;    //

    // регистры специального назначения
    int ip;     // указатель на текущую команду

    // буфер программы
    int*   buffer;      // указатель на начало кода
    size_t buf_size;    // размер массива

    // стеки
    stack_t* stack_data;  // стек данных
};

processor proc;

ERROR proc_init(void);
ERROR read_bin(const char* const file_name);
ERROR run_bin (void);
ERROR add_stck(stack_t* const stck);
ERROR sub_stck(stack_t* const stck);
ERROR div_stck(stack_t* const stck);
ERROR mul_stck(stack_t* const stck);


int main(int argc, char* argv[]) {
    if (argc != 2) {
        PRINT_ERROR;
        return ERROR_READ_TERM;
    } 
    
    // инициализация процессора
    ERROR result = proc_init();
    if (result != OK) {
        PRINT_ERROR;
        return result;
    } 

    // считать данные из .bin файла
    result = read_bin(argv[1]);
    if (result != OK) {
        PRINT_ERROR;
        return result;
    } 

    // работа программы
    result = run_bin();
    if (result != OK) {
        PRINT_ERROR;
        return result;
    } 

    return 0;
}

// инициализация процессора
ERROR proc_init(void) {
    // инициализация процессора
    proc = {};

    // инициализация буфера программы
    proc.buffer = (int*)calloc(NUM_ELEMS, sizeof(int));
    if (proc.buffer == NULL) {
        PRINT_ERROR;
        //print();
        return ERROR_MEM_ALLOC;
    }

    // инициализация стека данных
    proc.stack_data = (stack_t*)calloc(1, sizeof(stack_t));
    stack_init(proc.stack_data, 1);
    
    return OK;
}


ERROR read_bin(const char* const file_name) {
    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        PRINT_ERROR;
        return ERROR_FILE_OPEN;
    } 

    int cmd = 0;
    int param = 0;
    int flag = 0;
    int count = 0; // счётчик считанных данных
    while((flag = fscanf(file, "%d %d\n", &cmd, &param)) != EOF) {
        switch (flag) {
            case 1:
                // команда без параметров
                proc.buffer[count] = cmd;
                proc.buf_size++;
                count++;
                break;
            case 2:
                // команда с 1 параметром
                proc.buffer[count] = cmd;
                count++;
                proc.buffer[count] = param;
                count++;
                proc.buf_size += 2;
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
ERROR run_bin(void) {
    // программа не загружена
    if (proc.buf_size == 0) {
        PRINT_ERROR;
        return ERROR_CMD_NO;
    }

    proc.ip = 0; // указатель на текущую команду

    while(proc.ip < proc.buf_size) {
        switch (proc.buffer[proc.ip]) {
            case CMD_PUSH_NUMBER:
                // ложит в стек число
                push(proc.stack_data, proc.buffer[++proc.ip]);
                break;
            case CMD_PUSH:
                // ложит в стек из регистра
                switch (proc.buffer[++proc.ip]) {
                    case 1: 
                        push(proc.stack_data, proc.rax);
                        break;
                    case 2:
                        push(proc.stack_data, proc.rbx);
                        break;
                    case 3:
                        push(proc.stack_data, proc.rcx);
                        break;
                    case 4:
                        push(proc.stack_data, proc.rdx);
                        break;
                    default:
                        // неопределенный регистр
                        break;
                }
                break;
            case CMD_POP:
                // извлекает в регистр из стека 
                switch (proc.buffer[++proc.ip]) {
                    case 1: 
                        pop(&proc.rax, proc.stack_data);
                        break;
                    case 2:
                        pop(&proc.rbx, proc.stack_data);
                        break;
                    case 3:
                        pop(&proc.rcx, proc.stack_data);
                        break;
                    case 4:
                        pop(&proc.rdx, proc.stack_data);
                        break;
                    default:
                        // неопределенный регистр
                        break;
                }
                break;
            case CMD_OUT:
                printf("Result: %d\n", proc.rax);
                break;
            case CMD_HLT:
                // завершение программы
                return OK;
                break;
            case CMD_ADD:
                add_stck(proc.stack_data);
                break;
            case CMD_SUB:
                sub_stck(proc.stack_data);
                break;
            case CMD_DIV:
                div_stck(proc.stack_data);
                break;
            case CMD_MUL:
                mul_stck(proc.stack_data);
                break;
            default:
                PRINT_ERROR;
                return ERROR_CMD_UNKNOWN;
                break;
        }
        proc.ip++;
    }

    return OK;
}

ERROR add_stck(stack_t* const stck) {
    stck_el first;
    stck_el second;
    pop(&second, stck);
    pop(&first, stck);
    stck_el result = (stck_el)((int)first + (int)second);
    push(stck, result);

    return OK;
}

ERROR sub_stck(stack_t* const stck) {
    stck_el first;
    stck_el second;
    pop(&second, stck);
    pop(&first, stck);
    stck_el result = first - second;
    push(stck, result);

    return OK;
}

ERROR div_stck(stack_t* const stck) {
    stck_el first;
    stck_el second;
    pop(&second, stck);
    pop(&first, stck);
    stck_el result = first / second;
    push(stck, result);
    
    return OK;
}


ERROR mul_stck(stack_t* const stck) {
    stck_el first;
    stck_el second;
    pop(&second, stck);
    pop(&first, stck);
    stck_el result = first * second;
    push(stck, result);
    
    return OK;
}