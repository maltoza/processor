#include "proc.h"

#define LINE_LEN 20

#define RETURN_ERROR(err_code) do {                                               \
    printf("%s error (%d) %s:%d\n", __FILE__, err_code, __FUNCTION__,  __LINE__); \
    return err_code;                                                              \
} while (0)


static PROC_ERROR jmp(processor* proc);
static PROC_ERROR ja(processor* proc);
static PROC_ERROR call(processor* proc);
static PROC_ERROR ret(processor* proc);
static PROC_ERROR push_num(processor* proc);
static PROC_ERROR push(processor* proc);
static PROC_ERROR pop(processor* proc);
static PROC_ERROR in(processor* proc);
static PROC_ERROR out(processor* proc);
static PROC_ERROR add(processor* proc);
static PROC_ERROR sub(processor* proc);
static PROC_ERROR mul(processor* proc);
static PROC_ERROR div(processor* proc);
static PROC_ERROR hlt(processor* proc);

static PROC_ERROR (*(cmd[]))(processor* proc) =
{jmp, ja, call, ret, push_num, push, pop, in, out, add, sub, mul, div, hlt};

// инициализация процессора
PROC_ERROR proc_init(processor* proc) {
    // инициализация буфера программы
    proc->buffer = (int*)calloc(PROC_NUM_ELEMS, sizeof(int));
    if (proc->buffer == NULL) RETURN_ERROR(PROC_ERROR_MEM_ALLOC);

    // инициализация стека данных
    proc->stack_data = (stck_t*)calloc(1, sizeof(stck_t));
    STCK_ERRORS res = stck_init(proc->stack_data, 1);
    if(res != STCK_OK) RETURN_ERROR(PROC_ERROR_MEM_ALLOC);

    proc->stck_calls = (stck_t*)calloc(1, sizeof(int));
    res = stck_init(proc->stck_calls, 1);
    if(res != STCK_OK) RETURN_ERROR(PROC_ERROR_MEM_ALLOC);
    
    return PROC_OK;
}

PROC_ERROR proc_destroy(processor* proc) {
    stck_destroy(proc->stck_calls);
    free(proc->stck_calls);
    stck_destroy(proc->stack_data);
    free(proc->stack_data);
    free(proc->buffer);

    return PROC_OK;
}


PROC_ERROR proc_load(processor* proc, const char* const file_name) {
    FILE* file = fopen(file_name, "r");
    if (file == NULL) RETURN_ERROR(PROC_ERROR_FILE_OPEN);

    int cmd = 0;
    int param = 0;
    int flag = 0;
    int count = 0; // счётчик считанных данных
    char ln[LINE_LEN] = {};
    while((fgets(ln, LINE_LEN - 1, file))) {
        flag = sscanf(ln, "%d %d\n", &cmd, &param);
        switch (flag) {
            case 1:
                // команда без параметров
                proc->buffer[count] = cmd;
                proc->buf_size++;
                count++;
                break;
            case 2:
                // команда с 1 параметром
                proc->buffer[count] = cmd;
                count++;
                proc->buffer[count] = param;
                count++;
                proc->buf_size += 2;
                break;
            default:
                RETURN_ERROR(PROC_ERROR_FILE_SCANF);
                break;
        }
    }

    // закрытие ресурсов
    fclose(file);
    return PROC_OK;
}


// выполнение программы
PROC_ERROR proc_run(processor* proc) {
    // программа не загружена
    if (proc->buf_size == 0) RETURN_ERROR(PROC_ERROR_CMD_NO);

    proc->ip = 0; // указатель на текущую команду

    while(proc->ip < proc->buf_size) {
        PROC_ERROR res = cmd[proc->buffer[proc->ip]](proc);
        if (res != PROC_OK) {
            if (res == PROC_HLT) return res;
            else RETURN_ERROR(res);
        }
    }

    return PROC_OK;
}

// безусловный переход
static PROC_ERROR jmp(processor* proc) {
    int param = proc->buffer[proc->ip + 1];
    proc->ip = param;
    return PROC_OK;
}


static PROC_ERROR ja(processor* proc) {
    int param;
    int first_el, second_el;
    STCK_ERRORS res = stck_pop(proc->stack_data, &first_el);
    if (res != STCK_OK) RETURN_ERROR(PROC_ERROR_CMD);
    res = stck_pop(proc->stack_data, &second_el);
    if (res != STCK_OK) RETURN_ERROR(PROC_ERROR_CMD);

    if (first_el > second_el) {
        param = proc->buffer[proc->ip + 1];
        proc->ip = param;
    } else {
        proc->ip += 2;
    }
    
    return PROC_OK;
}


static PROC_ERROR call(processor* proc) {
    return PROC_OK;
}


static PROC_ERROR ret(processor* proc) {
    return PROC_OK;
}


static PROC_ERROR push_num(processor* proc) {
    int param = proc->buffer[proc->ip + 1];
    STCK_ERRORS res = stck_push(proc->stack_data, param);
    if (res != STCK_OK) RETURN_ERROR(PROC_ERROR_CMD);
    proc->ip += 2;
    return PROC_OK;
}


static PROC_ERROR push(processor* proc) {
    int param = proc->buffer[proc->ip + 1];
    STCK_ERRORS res = stck_push(proc->stack_data, proc->r[param]);
    if (res != STCK_OK) RETURN_ERROR(PROC_ERROR_CMD);
    proc->ip += 2;
    return PROC_OK;
}


static PROC_ERROR pop(processor* proc) {
    int param = proc->buffer[proc->ip + 1];
    STCK_ERRORS res = stck_pop(proc->stack_data, &proc->r[param]);
    if (res != STCK_OK) RETURN_ERROR(PROC_ERROR_CMD);
    proc->ip += 2;
    return PROC_OK;
}


static PROC_ERROR in(processor* proc) {
    int num;
    printf("Enter the number: ");
    int res1 = scanf("%d\n", &num);
    if (res1 != 1) {
        printf("Error, try again: ");
        return PROC_OK;
    } 
    STCK_ERRORS res2 = stck_push(proc->stack_data, num);
    if (res2 != STCK_OK) RETURN_ERROR(PROC_ERROR_CMD);
    proc->ip++;

    return PROC_OK;
}


static PROC_ERROR out(processor* proc) {
    stck_el out;
    STCK_ERRORS res = stck_pop(proc->stack_data, &out);
    if (res != STCK_OK) RETURN_ERROR(PROC_ERROR_CMD);
    printf("Result: %d\n", out);
    proc->ip++;

    return PROC_OK;
}


PROC_ERROR add(processor* proc) {

    stck_el first;
    stck_el second;
    STCK_ERRORS res = stck_pop(proc->stack_data, &second);
    if (res != STCK_OK) RETURN_ERROR(PROC_ERROR_CMD);
    res = stck_pop(proc->stack_data, &first);
    if (res != STCK_OK) RETURN_ERROR(PROC_ERROR_CMD);
    stck_el result = (stck_el)((int)first + (int)second);
    res = stck_push(proc->stack_data, result);
    if (res != STCK_OK) RETURN_ERROR(PROC_ERROR_CMD);
    proc->ip++;

    return PROC_OK;
}

PROC_ERROR sub(processor* proc) {
    stck_el first;
    stck_el second;
    STCK_ERRORS res = stck_pop(proc->stack_data, &second);
    if (res != STCK_OK) RETURN_ERROR(PROC_ERROR_CMD);
    res = stck_pop(proc->stack_data, &first);
    if (res != STCK_OK) RETURN_ERROR(PROC_ERROR_CMD);
    stck_el result = (stck_el)((int)first - (int)second);
    res = stck_push(proc->stack_data, result);
    if (res != STCK_OK) RETURN_ERROR(PROC_ERROR_CMD);
    proc->ip++;

    return PROC_OK;
}

PROC_ERROR mul(processor* proc) {
    stck_el first;
    stck_el second;
    STCK_ERRORS res = stck_pop(proc->stack_data, &second);
    if (res != STCK_OK) RETURN_ERROR(PROC_ERROR_CMD);
    res = stck_pop(proc->stack_data, &first);
    if (res != STCK_OK) RETURN_ERROR(PROC_ERROR_CMD);
    stck_el result = (stck_el)((int)first * (int)second);
    res = stck_push(proc->stack_data, result);
    if (res != STCK_OK) RETURN_ERROR(PROC_ERROR_CMD);
    proc->ip++;

    return PROC_OK;
}


PROC_ERROR div(processor* proc) {
    stck_el first;
    stck_el second;
    STCK_ERRORS res = stck_pop(proc->stack_data, &second);
    if (res != STCK_OK) RETURN_ERROR(PROC_ERROR_CMD);
    res = stck_pop(proc->stack_data, &first);
    if (res != STCK_OK) RETURN_ERROR(PROC_ERROR_CMD);
    stck_el result = (stck_el)((int)first / (int)second);
    res = stck_push(proc->stack_data, result);
    if (res != STCK_OK) RETURN_ERROR(PROC_ERROR_CMD);
    proc->ip++;

    return PROC_OK;
}


static PROC_ERROR hlt(processor* proc) {
    return PROC_HLT;
}
