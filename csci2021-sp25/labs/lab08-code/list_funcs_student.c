#include <stdlib.h>

#include "linked_list.h"

int list_find_student(list_t *list, int value) {
    if (list == NULL) {
        return -1;
    }

    node_t *current = list->head;
    for (int i = 0; i < list->size; i++) {
        if (current->value == value) {
            return i;
        } else {
            current = current->next;
        }
    }

    return -1;
}
