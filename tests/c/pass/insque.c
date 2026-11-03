#define _GNU_SOURCE
#include <search.h>
#include <stdlib.h>

struct elem {
    struct elem *next;
    struct elem *prev;
    int value;
};

// `insque` is not intercepted, so it relinks `head` and `tail` from
// uninstrumented code. Their links still have the provenance of the
// pointers that we stored below, but they now point to `mid`.
int insque_relink(void) {
    struct elem *head = calloc(1, sizeof(struct elem));
    struct elem *mid = calloc(1, sizeof(struct elem));
    struct elem *tail = calloc(1, sizeof(struct elem));
    head->next = tail;
    tail->prev = head;
    mid->value = 7;
    insque(mid, head);
    int sum = head->next->value + tail->prev->value;
    free(head);
    free(mid);
    free(tail);
    return sum;
}
