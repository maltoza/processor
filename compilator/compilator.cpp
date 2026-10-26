#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../proc/code_bin.h"
#include "labels.h"

#define RETURN_ERROR(err_code) do {                                                   \
    printf("%s:%d ERROR (%d) %s\n", __FILE__,  __LINE__, err_code, __FUNCTION__);     \
    return err_code;                                                                  \
} while (0)

#define CMPL_LINE_LEN_MAX 10  // максимальная длина строки
#define CMPL_WORD_LEN_MAX 10  // максимальная длина слова
#define CMPL_MAX_NUM_LAB 20   // максимальное количество меток

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
CMPL_ERROR convert_cmd(char* const cmd_asm, char* const param_asm, int* const cmd_bin, int* const param_bin, const label (*lbs)[CMPL_MAX_NUM_LAB], int* const flag);
CMPL_ERROR compile_cmd(const char* const cmd_asm, int* const cmd_bin);
CMPL_ERROR compile_cmd_param(const char* const cmd_asm, const char* const param_asm, int* const cmd_bin, int* const param_bin, const label (*lbs)[CMPL_MAX_NUM_LAB]);
CMPL_ERROR write(FILE* const file_bin, const int cmd_bin, const int param, const int flag);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        RETURN_ERROR(CMPL_ERROR_READ_TERM);
    } 

    printf("Start compiling file %s\n", argv[1]);
    FILE* file_asm = fopen(argv[1], "r");
    if (file_asm == NULL) {
        RETURN_ERROR(CMPL_ERROR_FILE_OPEN);
    } 

    FILE* file_bin = fopen(argv[2], "w");
    if (file_bin == NULL) {
        RETURN_ERROR(CMPL_ERROR_FILE_OPEN);
    } 

    label lbs[CMPL_MAX_NUM_LAB] = {};
    int lbs_count = 0;
    char cmd_asm[CMPL_WORD_LEN_MAX] = {};
    char param_asm[CMPL_WORD_LEN_MAX] = {};
    int cmd_bin = 0;
    int param_bin = 0;
    int flag = 0;
    int line_ptr = 0;

    // компиляция: заполнение массива меток
    while(1) {
        CMPL_ERROR res = read_asm(file_asm, cmd_asm, param_asm, &flag);
        if (flag == EOF) break;
        if (res != CMPL_OK) {
            fclose(file_asm);
            fclose(file_bin);
            RETURN_ERROR(res);
        }
        if (cmd_asm[strlen(cmd_asm) - 1] == ':') {
            LABEL_MESS res = add_labels_name(&lbs, cmd_asm, &line_ptr, &lbs_count);
            if (res != LABEL_MESS_OK) {
                fclose(file_asm);
                fclose(file_bin);
                RETURN_ERROR(res);
            }
        }
        else line_ptr += flag;
    }
    // переход на начало файла .asm
    fseek(file_asm, 0, SEEK_SET);

    // компиляция: преобразование команд
    while(1) {
        CMPL_ERROR res = read_asm(file_asm, cmd_asm, param_asm, &flag);
        if (flag == EOF) break;
        if (res != CMPL_OK) {
            fclose(file_asm);
            fclose(file_bin);
            RETURN_ERROR(res);
        }
        res = convert_cmd(cmd_asm, param_asm, &cmd_bin, &param_bin, &lbs, &flag);
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
    printf("file created %s\n", argv[2]);
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
                  int*  const cmd_bin, int* const param_bin,
                  const label (*lbs)[CMPL_MAX_NUM_LAB], int* const flag) {
    if (find_labels_name((const label (* const)[LABEL_NUM_MAX])lbs, cmd_asm, NULL, NULL) != LABEL_MESS_FIND) {
        if (*flag == 1) compile_cmd(cmd_asm, cmd_bin);
        else if (*flag == 2) compile_cmd_param(cmd_asm, param_asm, cmd_bin, param_bin, lbs);
        else RETURN_ERROR(CMPL_ERROR_CMD_UNKNOWN);
    } else *flag = 0;

    return CMPL_OK;
}

// преобразование команды без параметра
CMPL_ERROR compile_cmd(const char* const cmd_asm, int* const cmd_bin) {
    if (!strcmp(cmd_asm, "ADD")) *cmd_bin = CMD_ADD;
    else if(!strcmp(cmd_asm, "SUB")) *cmd_bin = CMD_SUB;
    else if(!strcmp(cmd_asm, "MUL")) *cmd_bin = CMD_MUL;
    else if(!strcmp(cmd_asm, "DIV")) *cmd_bin = CMD_DIV;
    else if(!strcmp(cmd_asm, "IN")) *cmd_bin = CMD_IN;
    else if(!strcmp(cmd_asm, "OUT")) *cmd_bin = CMD_OUT;
    else if(!strcmp(cmd_asm, "RET")) *cmd_bin = CMD_RET;
    else if(!strcmp(cmd_asm, "HLT")) *cmd_bin = CMD_HLT;
    else RETURN_ERROR(CMPL_ERROR_CMD_UNKNOWN);

    return CMPL_OK;
}

// преобразование команды с параметром
CMPL_ERROR compile_cmd_param(const char* const cmd_asm, const char* const param_asm,
                             int* const cmd_bin, int* const param_bin, const label (*lbs)[CMPL_MAX_NUM_LAB]) {
    if ((cmd_asm[0] == 'J') || (cmd_asm[0] == 'C')) {
        // преобразование команды
        if (!strcmp(cmd_asm, "JMP")) *cmd_bin = CMD_JMP;
        else if (!strcmp(cmd_asm, "JA")) *cmd_bin = CMD_JA;
        else if (!strcmp(cmd_asm, "JAE")) *cmd_bin = CMD_JAE;
        else if (!strcmp(cmd_asm, "JB")) *cmd_bin = CMD_JB;
        else if (!strcmp(cmd_asm, "JBE")) *cmd_bin = CMD_JBE;
        else if (!strcmp(cmd_asm, "JE")) *cmd_bin = CMD_JE;
        else if (!strcmp(cmd_asm, "JNE")) *cmd_bin = CMD_JNE;
        else if (!strcmp(cmd_asm, "CALL")) *cmd_bin = CMD_CALL;  // TODO массив 
        else RETURN_ERROR(CMPL_ERROR_CMD_UNKNOWN);
        //преобразование параметра 
        int lb_ptr;
        int lb_id;
        LABEL_MESS res = find_labels_name(lbs, param_asm, &lb_ptr, &lb_id);
        if (res != LABEL_MESS_FIND)  {
            RETURN_ERROR(CMPL_ERROR_CMD_UNKNOWN);
        }
        *param_bin = lb_ptr;
    } else {
        char* l;
        *param_bin = strtol(param_asm, &l, 10);
        if(*l == '\0') {
            if(!strcmp(cmd_asm, "PUSH")) *cmd_bin = CMD_PUSH_NUMBER;
            else RETURN_ERROR(CMPL_ERROR_CMD_UNKNOWN);
        } else {
            if(!strcmp(cmd_asm, "PUSH")) *cmd_bin = CMD_PUSH;
            else if (!strcmp(cmd_asm, "POP")) *cmd_bin = CMD_POP;
            else RETURN_ERROR(CMPL_ERROR_CMD_UNKNOWN);

            // преобразование регистра из строки в номер
            *param_bin = toupper(param_asm[1]) - 'A' + 1;
        } 

    }

    return CMPL_OK;
}

/*
CMPL_ERROR get_labels()

int find_label(char* const cmd_asm, const label  (* const lbs)[CMPL_MAX_NUM_LAB]) {
    for (int i = 0; i < CMPL_MAX_NUM_LAB; i++) {
        if (!strcmp(cmd_asm, (*lbs)[i].name)) return i;
    }

    return -1;
}
*/

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