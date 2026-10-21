#include "stack.h"

#ifdef STACK_DEBUG
    #define STCK_ASSERT(stck_ptr) stck_assert((stck_ptr), __FILE__, __FUNCTION__, __LINE__)
#else
    #define STCK_ASSERT(stck_ptr)  
#endif


void stck_assert(stck_t* const stck, const char* file_name, const char* func_name, const int line);
STCK_ERRORS stck_verificate(stck_t* const stck, const char* file_name, const char* func_name, const int line);
void stck_dump(const stck_t* const stck, const STCK_ERRORS result);

// завершение работы
void stck_assert(stck_t* const stck, const char* file_name, const char* func_name, const int line)
{
    STCK_ERRORS result = stck_verificate(stck, file_name, func_name, line);
    stck_dump(stck, result);
    assert(result == STCK_OK);
}

// проверка стека
STCK_ERRORS stck_verificate(stck_t* const stck, const char* file_name, const char* func_name, const int line)
{
    if (stck == NULL)
    {
        STCK_PRINT_ERROR(STCK_ERRORS_NULLPTR);
        return STCK_ERRORS_NULLPTR;
    }
    else if (stck->capacity == 0)
    {
        stck->error.file = file_name;
        stck->error.func = func_name;
        stck->error.line = line;
        STCK_PRINT_ERROR(STCK_ERRORS_CAPACITY);
        return STCK_ERRORS_CAPACITY;
    }
    else if (stck->num_elems > stck->capacity)
    {
        stck->error.file = file_name;
        stck->error.func = func_name;
        stck->error.line = line;  
        STCK_PRINT_ERROR(STCK_ERRORS_NUMELEM);
        return STCK_ERRORS_NUMELEM;
    }
    else if (stck->canary_stck_start != STCK_CANARY_START)
    {
        stck->error.file = file_name;
        stck->error.func = func_name;
        stck->error.line = line;
        STCK_PRINT_ERROR(STCK_ERRORS_STRUCT_CANARY1);
        return STCK_ERRORS_STRUCT_CANARY1;
    }
    else if (stck->canary_stck_end != STCK_CANARY_END)
    {
        stck->error.file = file_name;
        stck->error.func = func_name;
        stck->error.line = line;
        STCK_PRINT_ERROR(STCK_ERRORS_STRUCT_CANARY2);
        return STCK_ERRORS_STRUCT_CANARY2;
    }
    else if (*(stck->canary_buf_start) != BUF_CANARY_START)
    {
        stck->error.file = file_name;
        stck->error.func = func_name;
        stck->error.line = line;
        STCK_PRINT_ERROR(STCK_ERRORS_STRUCT_CANARY1);
        return STCK_ERRORS_STRUCT_CANARY1;
    }
    else if (*(stck->canary_buf_end) != BUF_CANARY_END)
    {
        stck->error.file = file_name;
        stck->error.func = func_name;
        stck->error.line = line;
        STCK_PRINT_ERROR(STCK_ERRORS_STRUCT_CANARY2);
        return STCK_ERRORS_STRUCT_CANARY2;
    }   
    else
    {
        stck->error.file = file_name;
        stck->error.func = func_name;
        stck->error.line = line;
    }

    return STCK_OK;
}

// вывод ошибок в файл
void stck_dump(const stck_t* const stck, const STCK_ERRORS result)
{
    FILE* err = fopen(STCK_FILE_DUMP, "a+");

    fprintf (err, "======================================\n");
    if (result == STCK_OK)
    {
        fprintf (err, "            VERIFICATE OK\n");       
    }
    else
    {
        fprintf (err, "            VERIFICATE ERROR\n");
    }
    fprintf (err, "======================================\n");
    fprintf (err, "            INFORMATION:\n");
    fprintf (err, "File name:     %s\n", (*stck).error.file);
    fprintf (err, "Function name: %s\n", (*stck).error.func);
    fprintf (err, "Line number:   %d\n", (*stck).error.line);
    fprintf (err, "---------------------------------------\n");
    fprintf (err, "    num_elems: %d\n", (*stck).num_elems);
    fprintf (err, "    capacity:  %d\n", (*stck).capacity);
    fprintf (err, "    struct canary begin:  %d\n", (*stck).canary_stck_start);
    fprintf (err, "    struct canary finish: %d\n", (*stck).canary_stck_end);
    fprintf (err, "    buffer:  %p\n", (*stck).buffer);
    fprintf (err, "---------------------------------------\n");
    fprintf (err, "    buffer canary begin:  %d\n", *(*stck).canary_buf_start);
    for (size_t i = 0; i < (*stck).capacity; i++) {
        if (i < (*stck).num_elems)
        {
            fprintf (err, "       *elem [%d]: %c\n", i, ((stck_el*)(*stck).buffer)[i]);
        }
        else
        {
            fprintf (err, "        elem [%d]: %c\n", i, ((stck_el*)(*stck).buffer)[i]);            
        }
    }
    fprintf (err, "    buffer canary finish: %d\n", *(*stck).canary_buf_end);
    fprintf (err, "======================================\n\n\n");

    fclose(err);
    return;
}


// создание стека
STCK_ERRORS stck_init(stck_t* const stck, const size_t capacity)
{
    stck->buffer = malloc(capacity * sizeof(stck_el) + 2 * sizeof(canary_t));
    FILE* file = fopen(STCK_FILE_DUMP, "w");
    fclose(file);


    stck->canary_stck_start = STCK_CANARY_START;
    stck->canary_stck_end = STCK_CANARY_END;
    stck->num_elems = 0;
    stck->capacity = capacity;
    if (stck->buffer == NULL) {
        STCK_PRINT_ERROR(STCK_ERRORS_MEMALLOC);
        return STCK_ERRORS_MEMALLOC;
    }

    stck->canary_buf_start = (canary_t*)stck->buffer;
    *stck->canary_buf_start = BUF_CANARY_START;
    stck->buffer = (void*)(((canary_t*)stck->buffer) + 1);

    STCK_ERRORS res = stck_fill_poison(stck, 0);
    if (res != STCK_OK) {
        STCK_PRINT_ERROR(res);
        return res;
    }
    
    stck->canary_buf_end = (canary_t*)(((stck_el*)stck->buffer) + capacity);
    *stck->canary_buf_end = BUF_CANARY_END;

    STCK_ASSERT(stck);

    return STCK_OK;
}



// удаление стека
void stck_destroy(stck_t* const stck)
{
    free(stck->canary_buf_start);
    stck->buffer = NULL;
    stck->capacity = 0;
    stck->num_elems = 0;
    stck->canary_stck_start = 0;
    stck->canary_stck_end = 0;
    stck->canary_buf_start = NULL;
    stck->canary_buf_end = NULL;
    
    return;
}



// удаление элемента из стека
STCK_ERRORS stck_pop(stck_t* const stck, stck_el* const out)
{
    STCK_ASSERT(stck);
    
    if (stck->num_elems == 0) {
        STCK_PRINT_ERROR(STCK_ERRORS_EMPTY);
        return STCK_ERRORS_EMPTY;
    }

    memcpy(out, &((stck_el*)stck->buffer)[stck->num_elems - 1], sizeof(stck_el));
    ((stck_el*)stck->buffer)[--stck->num_elems] = POISON;
    
    STCK_ERRORS res = stck_check_memory(stck);
    if (res != STCK_OK) {
        STCK_PRINT_ERROR(res);
        return res;
    }
    
    STCK_ASSERT(stck);

    return STCK_OK;
}


// добавление элемента в стек
STCK_ERRORS stck_push(stck_t* const stck, const stck_el elem)
{
    STCK_ASSERT(stck);
    
    STCK_ERRORS res = stck_check_memory(stck);
    if (res != STCK_OK) {
        STCK_PRINT_ERROR(res);
        return res;
    }
    
    ((stck_el*)stck->buffer)[stck->num_elems++] = elem;
    
    STCK_ASSERT(stck);
    
    return STCK_OK;
}


STCK_ERRORS stck_check_memory(stck_t* const stck) {
    if (stck->num_elems == stck->capacity) stck_get_mem(stck);
    else if ((stck->capacity > 1) && (stck->num_elems < (stck->capacity) / 2)) stck_del_mem(stck);

    return STCK_OK;
}

// увеличение памяти стека
STCK_ERRORS stck_get_mem(stck_t* const stck)
{
    void* temp = realloc((void*)(stck->canary_buf_start), 2 * stck->capacity * sizeof(stck_el) + 2 * sizeof(canary_t));
    if (temp == NULL) {
        STCK_PRINT_ERROR(STCK_ERRORS_GMEM);
        return STCK_ERRORS_GMEM;
    }
    stck->canary_buf_start = (canary_t*)temp;
    *stck->canary_buf_start = BUF_CANARY_START;
    stck->buffer = (stck_el*)((int*)temp + 1);
    stck->capacity = stck->capacity * 2;
    stck->canary_buf_end = (canary_t*)((int*)stck->buffer + stck->capacity);
    *stck->canary_buf_end = BUF_CANARY_END;
    STCK_ERRORS res = stck_fill_poison(stck, stck->num_elems);
    if (res != STCK_OK) {
        STCK_PRINT_ERROR(res);
        return res;
    }

    return STCK_OK;
}

// уменьшение памяти стека
STCK_ERRORS stck_del_mem(stck_t* const stck)
{
    void* temp = realloc((void*)stck->canary_buf_start, stck->capacity / 2 * sizeof(stck_el) + 2 * sizeof(canary_t));
    if (temp == NULL) {
        STCK_PRINT_ERROR(STCK_ERRORS_GMEM);
        return STCK_ERRORS_GMEM;
    }

    stck->canary_buf_start = (canary_t*)temp;
    *stck->canary_buf_start = BUF_CANARY_START;
    stck->buffer = (stck_el*)((int*)temp + 1);
    stck->capacity = stck->capacity / 2;
    stck->canary_buf_end = (canary_t*)((int*)stck->buffer + stck->capacity);
    *stck->canary_buf_end = BUF_CANARY_END;

    STCK_ERRORS res = stck_fill_poison(stck, stck->num_elems);
    if (res != STCK_OK) {
        STCK_PRINT_ERROR(res);
        return res;
    }
    
    return STCK_OK;
}


// заполнение ядовитыми значениями
STCK_ERRORS stck_fill_poison(stck_t* const stck, const int start)
{
    for (size_t i = start; i < stck->capacity; i++)
    {
        ((stck_el*)stck->buffer)[i] = POISON;
    }

    return STCK_OK;
}

