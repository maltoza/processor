#ifndef LABELS_H
#define LABELS_H

#include <string.h>
#include <stdio.h>

#define LABEL_LEN_MAX 10
#define LABEL_NUM_MAX 20

#define RETURN_ERROR(err_code) do {                                               \
    printf("%s ERROR (%d) %s:%d\n", __FILE__, err_code, __FUNCTION__,  __LINE__); \
    return err_code;                                                              \
} while (0)

enum LABEL_MESS {
    LABEL_MESS_OK = 0,
    LABEL_MESS_FIND,
    LABEL_MESS_NOTFIND,
    LABEL_MESS_DOUB,
};

struct label {
    char name[LABEL_LEN_MAX]; // имя метки
    int ptr;                // адрес перехода
};

LABEL_MESS find_labels_name(const label (* const lbs)[LABEL_NUM_MAX], const char* const lb_name, int* const lb_ptr, int* const lb_id);
LABEL_MESS add_labels_name(label (* const lbs)[LABEL_NUM_MAX], const char* const lb_name, int* const lb_ptr, int* const lb_count);

#endif //LABELS_H