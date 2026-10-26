#include "labels.h"

#define RETURN_ERROR(err_code) do {                                               \
    printf("%s ERROR (%d) %s:%d\n", __FILE__, err_code, __FUNCTION__,  __LINE__); \
    return err_code;                                                              \
} while (0)

LABEL_MESS find_labels_name(const label (* const lbs)[LABEL_NUM_MAX], const char* const lb_name, int* const lb_ptr, int* const lb_id) {
    for (int i = 0; i < LABEL_NUM_MAX; i++) {
        if (!strcmp(lb_name, (*lbs)[i].name)) {
            if (lb_ptr != NULL) *lb_ptr = (*lbs)[i].ptr;
            if (lb_id != NULL) *lb_id = i;
            return LABEL_MESS_FIND;
        }
    }

    return LABEL_MESS_NOTFIND;
}

LABEL_MESS add_labels_name(label (* const lbs)[LABEL_NUM_MAX], const char* const lb_name, int* const lb_ptr, int* const lb_count) {
    int lb_id = -1;
    LABEL_MESS res = find_labels_name(lbs, lb_name, lb_ptr, &lb_id);
    if (res == LABEL_MESS_FIND) {
        if ((*lbs)[lb_id].ptr != *lb_ptr) RETURN_ERROR(LABEL_MESS_DOUB);
    } else {
        strcpy((*lbs)[*lb_count].name, lb_name);
        (*lbs)[*lb_count].ptr = *lb_ptr;
        (*lb_count)++;
    }

    return LABEL_MESS_OK;
}