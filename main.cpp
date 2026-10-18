#include "proc.h"

#define RETURN_ERROR(err_code) printf("%s error (%d) %s:%d\n", __FILE__, err_code, __FUNCTION__,  __LINE__)

enum MAIN_ERROR {
    ERROR_ARG = 0,

};

int main(int argc, char* argv[]) {
    processor proc1;
    
    if (argc != 2) {
        RETURN_ERROR(ERROR_ARG);
    } 
    
    // инициализация процессора
    PROC_ERROR result = proc_init(&proc1);
    if (result != PROC_OK) {
        RETURN_ERROR(result);
    } 

    // считать данные из .bin файла
    result = proc_load(&proc1, argv[1]);
    if (result != PROC_OK) {
        RETURN_ERROR(result);
    } 

    // работа программы
    result = proc_run(&proc1);
    if (result != PROC_OK) {
        RETURN_ERROR(result);
    } 

    return 0;
}