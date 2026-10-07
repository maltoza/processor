#include "stack.h"

#ifdef STACK_DEBUG
    #define STCK_ASSERT(stck_ptr) stack_assert((stck_ptr), __FILE__, __FUNCTION__, __LINE__)
#else
    #define STCK_ASSERT(stck_ptr)  
#endif


void stack_assert(stack_t* stck, const char* file_name, const char* func_name, int line);
ERRORS_STCK stack_verificate(stack_t* stck, const char* file_name, const char* func_name, int line);
void stack_print_err(stack_t stck, ERRORS_STCK result);

// завершение работы
void stack_assert(stack_t* stck, const char* file_name, const char* func_name, int line)
{
    ERRORS_STCK result = stack_verificate(stck, file_name, func_name, line);
    stack_print_err(*stck, result);
    assert(result == ERRORS_STCK_OK);
}

// проверка стека
ERRORS_STCK stack_verificate(stack_t* stck, const char* file_name, const char* func_name, int line)
{
    if (stck == NULL)
    {
        stck->error.file = file_name;
        stck->error.func = func_name;
        stck->error.line = line;
        return ERRORS_STCK_NULLPTR;
    }
    else if (stck->capacity == 0)
    {
        stck->error.file = file_name;
        stck->error.func = func_name;
        stck->error.line = line;
        return ERRORS_STCK_CAPACITY;
    }
    else if (stck->num_elems > stck->capacity)
    {
        stck->error.file = file_name;
        stck->error.func = func_name;
        stck->error.line = line;  
        return ERRORS_STCK_NUMELEM;
    }
    else if (stck->canary_stck_start != STCK_CANARY_START)
    {
        stck->error.file = file_name;
        stck->error.func = func_name;
        stck->error.line = line;
        return ERRORS_STRUCT_CANARY1;
    }
    else if (stck->canary_stck_end != STCK_CANARY_END)
    {
        stck->error.file = file_name;
        stck->error.func = func_name;
        stck->error.line = line;
        return ERRORS_STRUCT_CANARY2;
    }
    else if (*(stck->canary_buf_start) != BUF_CANARY_START)
    {
        stck->error.file = file_name;
        stck->error.func = func_name;
        stck->error.line = line;
        return ERRORS_STRUCT_CANARY1;
    }
    else if (*(stck->canary_buf_end) != BUF_CANARY_END)
    {
        stck->error.file = file_name;
        stck->error.func = func_name;
        stck->error.line = line;
        return ERRORS_STRUCT_CANARY2;
    }   
    else
    {
        stck->error.file = file_name;
        stck->error.func = func_name;
        stck->error.line = line;
    }

    return ERRORS_STCK_OK;
}

// вывод ошибок в файл
void stack_print_err(stack_t stck, ERRORS_STCK result)
{
    FILE* err = fopen("errors.txt", "a+");

    fprintf (err, "======================================\n");
    if (result == ERRORS_STCK_OK)
    {
        fprintf (err, "            VERIFICATE OK\n");       
    }
    else
    {
        fprintf (err, "            VERIFICATE ERROR\n");
    }
    fprintf (err, "======================================\n");
    fprintf (err, "            INFORMATION:\n");
    fprintf (err, "File name:     %s\n", stck.error.file);
    fprintf (err, "Function name: %s\n", stck.error.func);
    fprintf (err, "Line number:   %d\n", stck.error.line);
    fprintf (err, "---------------------------------------\n");
    fprintf (err, "    num_elems: %d\n", stck.num_elems);
    fprintf (err, "    capacity:  %d\n", stck.capacity);
    fprintf (err, "    struct canary begin:  %d\n", stck.canary_stck_start);
    fprintf (err, "    struct canary finish: %d\n", stck.canary_stck_end);
    fprintf (err, "    buffer canary begin:  %d\n", *stck.canary_buf_start);
    fprintf (err, "    buffer canary finish: %d\n", *stck.canary_buf_end);
    fprintf (err, "    buffer:  %p\n", stck.buffer);
    fprintf (err, "---------------------------------------\n");
    for (size_t i = 0; i < stck.capacity; i++) {
        if (i < stck.num_elems)
        {
            fprintf (err, "       *elem [%d]: %d\n", i, ((stck_el*)stck.buffer)[i]);
        }
        else
        {
            fprintf (err, "        elem [%d]: %d\n", i, ((stck_el*)stck.buffer)[i]);            
        }
    }
    fprintf (err, "======================================\n\n\n");

    fclose(err);
    return;
}


// создание стека
ERRORS stack_init(stack_t* stck, size_t capacity)
{
    FILE* file = fopen("errors.txt", "w");
    fclose(file);

    stck->canary_stck_start = STCK_CANARY_START;
    stck->canary_stck_end = STCK_CANARY_END;
    stck->buffer = malloc(capacity * sizeof(stck_el) + 2 * sizeof(canary_t));
    if (stck->buffer == NULL) return ERRORS_MEMALLOC;

    stck->canary_buf_start = (canary_t*)stck->buffer;
    *stck->canary_buf_start = BUF_CANARY_START;
    stck->buffer = (void*)(((canary_t*)stck->buffer) + 1);
    fill_poison(stck, 0);
    stck->num_elems = 0;
    stck->capacity = capacity;
    stck->canary_buf_end = (canary_t*)(((stck_el*)stck->buffer) + capacity);
    *stck->canary_buf_end = BUF_CANARY_END;

    STCK_ASSERT(stck);

    return FUNC_OK;
}



// удаление стека
void stack_close(stack_t* stck)
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
ERRORS pop(stack_t* stck, stck_el* out)
{
    STCK_ASSERT(stck);
    
    if (stck->num_elems == 0) return ERRORS_EMPTY;

    memcpy(out, &((stck_el*)stck->buffer)[stck->num_elems], sizeof(stck_el));
    ((stck_el*)stck->buffer)[--stck->num_elems] = POISON;
    
    if ((stck->capacity > 1) && (stck->num_elems < (stck->capacity) / 2)) del_mem(stck);
    
    STCK_ASSERT(stck);

    
    return FUNC_OK;
}


// добавление элемента в стек
ERRORS push(stack_t* stck, stck_el elem)
{
    STCK_ASSERT(stck);
    
    if (stck->num_elems == stck->capacity) get_mem(stck); //TODO в функцию выдел памятмт
    ((stck_el*)stck->buffer)[stck->num_elems++] = elem;
    
    STCK_ASSERT(stck);
    
    return FUNC_OK;
}


// увеличение памяти стека
ERRORS get_mem(stack_t* stck)
{
    void* temp = realloc((void*)stck->canary_buf_start, 2 * stck->capacity * sizeof(stck_el) + 2 * sizeof(canary_t));
    if (temp == NULL) return ERRORS_GMEM;
    stck->canary_buf_start = (canary_t*)temp;
    *stck->canary_buf_start = BUF_CANARY_START;
    stck->buffer = ((stck_el*)temp + 1);
    stck->capacity = stck->capacity * 2;
    stck->canary_buf_end = (canary_t*)((stck_el*)stck->buffer + stck->capacity);
    *stck->canary_buf_end = BUF_CANARY_END;
    fill_poison(stck, stck->num_elems);

    return FUNC_OK;
}

// уменьшение памяти стека
ERRORS del_mem(stack_t* stck)
{
    void* temp = realloc((void*)stck->canary_buf_start, stck->capacity / 2 * sizeof(stck_el) + 2 * sizeof(canary_t));
    if (temp == NULL) return ERRORS_GMEM;
    stck->canary_buf_start = (canary_t*)temp;
    *stck->canary_buf_start = BUF_CANARY_START;
    stck->buffer = ((stck_el*)temp + 1);
    stck->capacity = stck->capacity / 2;
    stck->canary_buf_end = (canary_t*)((stck_el*)stck->buffer + stck->capacity);
    *stck->canary_buf_end = BUF_CANARY_END;
    fill_poison(stck, stck->num_elems);
    
    return FUNC_OK;
}


// заполнение ядовитыми значениями
ERRORS fill_poison(stack_t* stck, int start)
{
    //memset(&(((stck_el*)stck->buffer)[start]), POISON, stck->capacity - sizeof(stck_el) * (start - 1));

    for (size_t i = start; i < stck->capacity; i++)
    {
        ((stck_el*)stck->buffer)[i] = POISON;
    }

    return FUNC_OK;
}

