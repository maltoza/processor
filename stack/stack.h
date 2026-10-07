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


// тип элементов в стеке
typedef int stck_el;
// тип канарейки
typedef int canary_t;


// ошибки
enum ERRORS
{
    FUNC_OK = 0,        // функция завершена без ошибок
    ERRORS_MEMALLOC,    // ошибка выделения памяти
    ERRORS_EMPTY,       // пустой стек
    ERRORS_INIT,        // ошибка в инициализации стека
    ERRORS_POP,         // ошибка удаления элемента
    ERRORS_PUSH,        // ошибка добавления элемента
    ERRORS_GMEM,        // ошибка изменения размера стека
    ERRORS_DMEM         // ошибка уменьшения памяти
};

// ошибки стека
enum ERRORS_STCK // TODO ошибки + const
{
    ERRORS_STCK_OK = 0,     // нет ошибок
    ERRORS_STCK_NULLPTR,    // нулевой указатель на стек
    ERRORS_STCK_CAPACITY,   // ошибка размера стека
    ERRORS_STCK_NUMELEM,    // ошибка количества элементов
    ERRORS_STRUCT_CANARY1,    // ошибка начальной канарейки стека
    ERRORS_STRUCT_CANARY2     // ошибка конечной канарейки стека
};

struct err_t
{
    const char* file;   // имя файла
    const char* func;   // имя функции
    int line;           // номер линии
};

struct stack_t
{
    canary_t          canary_stck_start;
    err_t             error;            // ошибки
    canary_t*         canary_buf_start; // указатель на начальную канарейку буфера
    void*             buffer;           // указатель на буфер
    canary_t*         canary_buf_end;   // указатель на конечную канарейку буфера
    size_t            num_elems;        // количество элементов в стеке
    size_t            capacity;         // размер стека (кол-во ячеек)
    canary_t          canary_stck_end;
};

ERRORS stack_init(stack_t* stck, size_t capacity);
void stack_close(stack_t* stck);

ERRORS pop(stack_t* stck, stck_el* out);
ERRORS push(stack_t* stck, stck_el elem);
ERRORS get_mem(stack_t* stck);
ERRORS del_mem(stack_t* stck);
ERRORS fill_poison(stack_t* stck, int start);

#endif // STACK_H