#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define POISON 63
#define STCK_CANARY_START 123
#define STCK_CANARY_END 124
#define BUF_CANARY_START 125
#define BUF_CANARY_END 126
#define STCK_FILE_DUMP "stck_dump.txt"

#define STCK_PRINT_ERROR(stck_err_code) printf("%s error (%d) %s:%d\n", __FILE__, stck_err_code, __FUNCTION__,  __LINE__)

// тип элементов в стеке
typedef int stck_el;
// тип канарейки
typedef int canary_t;


// ошибки
enum STCK_ERRORS
{
    STCK_OK = 0,        // функция завершена без ошибок
    STCK_ERRORS_MEMALLOC,    // ошибка выделения памяти
    STCK_ERRORS_EMPTY,       // пустой стек
    STCK_ERRORS_INIT,        // ошибка в инициализации стека
    STCK_ERRORS_POP,         // ошибка удаления элемента
    STCK_ERRORS_PUSH,        // ошибка добавления элемента
    STCK_ERRORS_GMEM,        // ошибка изменения размера стека
    STCK_ERRORS_DMEM,         // ошибка уменьшения памяти

    STCK_ERRORS_NULLPTR,    // нулевой указатель на стек
    STCK_ERRORS_CAPACITY,   // ошибка размера стека
    STCK_ERRORS_NUMELEM,    // ошибка количества элементов
    STCK_ERRORS_STRUCT_CANARY1,    // ошибка начальной канарейки стека
    STCK_ERRORS_STRUCT_CANARY2     // ошибка конечной канарейки стека
};

struct stck_err_t
{
    const char* file;   // имя файла
    const char* func;   // имя функции
    int line;           // номер линии
};

struct stck_t
{
    canary_t          canary_stck_start;
    stck_err_t             error;       // ошибки
    size_t            num_elems;        // количество элементов в стеке
    size_t            capacity;         // размер стека (кол-во ячеек)
    canary_t*         canary_buf_start; // указатель на начальную канарейку буфера
    void*             buffer;           // указатель на буфер
    canary_t*         canary_buf_end;   // указатель на конечную канарейку буфера
    canary_t          canary_stck_end;
};

STCK_ERRORS stck_init(stck_t* const stck, const size_t capacity);
void stck_destroy(stck_t* const stck);

STCK_ERRORS stck_pop(stck_t* const stck, stck_el* const out);
STCK_ERRORS stck_push(stck_t* const stck, const stck_el elem);
STCK_ERRORS stck_check_memory(stck_t* const stck);
STCK_ERRORS stck_get_mem(stck_t* const stck);
STCK_ERRORS stck_del_mem(stck_t* const stck);
STCK_ERRORS stck_fill_poison(stck_t* const stck, const int start);

#endif // STACK_H