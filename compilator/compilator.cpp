#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../proc/code_bin.h"

#define RETURN_ERROR(err_code) do {                                                   \
    printf("%s CMPL_ERROR (%d) %s:%d\n", __FILE__, err_code, __FUNCTION__,  __LINE__); \
    return err_code;                                                                    \
} while (0)

#define CMPL_LINE_LEN_MAX 10  // максимальная длина строки
#define CMPL_WORD_LEN_MAX 10  // максимальная длина слова
#define CMPL_MAX_NUM_LAB 20  // максимальное количество меток

struct label {
    char name[CMPL_WORD_LEN_MAX]; // имя метки
    int ptr;                // адрес перехода
};

enum CMPL_ERROR {
    CMPL_OK = 0,
    CMPL_ERROR_READ_TERM = 10,

    // ошибки работы с файлом
    CMPL_ERROR_FILE_OPEN = 20,
    CMPL_ERROR_FILE_SCANF,
    CMPL_ERROR_FILE_END,
    CMPL_ERROR_FILE_WRITE,

    // ошибки работы с памятью
    CMPL_ERROR_MEM_ALLOC = 30,

    // ошибки выполнения команд .bin
    CMPL_ERROR_CMD = 40,
    CMPL_ERROR_CMD_UNKNOWN,
    CMPL_ERROR_CMD_SIZE,
};


CMPL_ERROR read_asm(FILE* const file_asm, char* const cmd, char* const param, int* const flag);
CMPL_ERROR convert_cmd(char* const cmd_asm, char* const param_asm, int* const cmd_bin, int* const param_bin);
CMPL_ERROR write(FILE* const file_bin, const int cmd_bin, const int param, const int flag);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        RETURN_ERROR(CMPL_ERROR_READ_TERM);
        return CMPL_ERROR_READ_TERM;
    } 

    printf("Start compiling file %s", argv[0]);
    FILE* file_asm = fopen(argv[1], "r");
    if (file_asm == NULL) {
        RETURN_ERROR(CMPL_ERROR_FILE_OPEN);
        return CMPL_ERROR_FILE_OPEN;
    } 

    FILE* file_bin = fopen(argv[2], "w");
    if (file_bin == NULL) {
        RETURN_ERROR(CMPL_ERROR_FILE_OPEN);
        return CMPL_ERROR_FILE_OPEN;
    } 

    label lbs[CMPL_MAX_NUM_LAB];
    char cmd_asm[CMPL_WORD_LEN_MAX];
    char param_asm[CMPL_WORD_LEN_MAX];
    int cmd_bin = 0;
    int param_bin = 0;
    int flag = 0;

    //компиляция
    while(1) {
        CMPL_ERROR res = read_asm(file_asm, cmd_asm, param_asm, &flag);
        if (res != CMPL_OK) {
            fclose(file_asm);
            fclose(file_bin);
            RETURN_ERROR(res);
        }
        if (flag == EOF) break;
        res = convert_cmd(cmd_asm, param_asm, &cmd_bin, &param_bin);
        if (res != CMPL_OK) {
            fclose(file_asm);
            fclose(file_bin);
            RETURN_ERROR(res);
        }
        res = write(file_bin, cmd_bin, param_bin, flag);
        if (res != CMPL_OK) {
            fclose(file_asm);
            fclose(file_bin);
            RETURN_ERROR(res);
        }
    }

    fclose(file_asm);
    fclose(file_bin);
    printf("file created %s", argv[2]);
    return 0;
}

CMPL_ERROR read_asm(FILE* const file_asm, char* const cmd, char* const param, int* const flag) {

    char line[CMPL_LINE_LEN_MAX];
    if (fgets(line, sizeof(line), file_asm) != 0) {
        *flag = sscanf(line, "%s %s\n", cmd, param);
    } else {
        *flag = EOF;
        return CMPL_ERROR_FILE_END;
    }

    return CMPL_OK;
}


CMPL_ERROR convert_cmd(char* const cmd_asm, char* const param_asm,
                  int*  const cmd_bin, int*  const param_bin,
                  const label lbs[CMPL_MAX_NUM_LAB], int* const flag) {
    if (find_label(cmd_asm, (const label (*)[20])lbs) < 0) {
        if (*flag == 1) compile_cmd(cmd_asm, cmd_bin);
        else if (*flag == 2) compile_cmd_param(cmd_asm, param_asm, cmd_bin, param_bin);
        else RETURN_ERROR(CMPL_ERROR_CMD_UNKNOWN);
    } else *flag = 0;

    return CMPL_OK;
}

CMPL_ERROR get_labels()

int find_label(char* const cmd_asm, const label  (* const lbs)[CMPL_MAX_NUM_LAB]) {
    for (int i = 0; i < CMPL_MAX_NUM_LAB; i++) {
        if (!strcmp(cmd_asm, (*lbs)[i].name)) return i;
    }

    return -1;
}


CMPL_ERROR write(FILE* const file_bin, const int cmd_bin, const int param, const int flag) {
    char ln[CMPL_LINE_LEN_MAX];
    if (flag == 0) return CMPL_OK;
    else if (flag ==  1) sprintf(ln, "%d\n", cmd_bin);
    else if (flag == 2) sprintf(ln, "%d %d\n", cmd_bin, param);
    else {
        RETURN_ERROR(CMPL_ERROR_CMD_SIZE);
        return CMPL_ERROR_CMD_SIZE;
    }

    size_t result = fwrite(ln, strlen(ln), 1, file_bin);
    if (result != 1) {
        RETURN_ERROR(CMPL_ERROR_FILE_WRITE);
        return CMPL_ERROR_FILE_WRITE;
    }

    return CMPL_OK;
}