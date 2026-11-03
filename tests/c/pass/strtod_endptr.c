#include <stdlib.h>
#include <string.h>

// Forces the end pointer to live in memory, so that `strto*` writes it
// from uninstrumented code.
#define ESCAPE(p) __asm__ volatile("" : : "r"(p) : "memory")

// The end pointer's slot holds the provenance of `other` before `strtod`
// overwrites it with a pointer into `s`.
char strtod_after_unrelated(const char *s, const char *other) {
    char *end = (char *)other;
    ESCAPE(&end);
    strtod(s, &end);
    return *end;
}

// The end pointer's slot holds the provenance of a freed allocation.
char strtod_after_freed(const char *s) {
    char *tmp = malloc(10);
    char *end = tmp;
    free(tmp);
    ESCAPE(&end);
    strtod(s, &end);
    return *end;
}

// The end pointer's slot holds a pointer into a freed allocation, and
// `strtol` stores an equal pointer into a new allocation at the same address.
// The stale provenance cannot be detected from the stored value alone.
char strtol_after_reuse(void) {
    char *a = malloc(16);
    memcpy(a, "42  ", 5);
    char *end = a + 2;
    ESCAPE(&end);
    free(a);
    char *b = malloc(16);
    memcpy(b, "42  ", 5);
    strtol(b, &end, 10);
    char c = *end;
    free(b);
    return c;
}
