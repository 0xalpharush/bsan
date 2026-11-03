#include <stdlib.h>
#include <string.h>

#define ESCAPE(p) __asm__ volatile("" : : "r"(p) : "memory")

// The end pointer inherits the provenance of `s`, so reading past the end
// of `s` through it is still reported as out-of-bounds.
char strtod_endptr_oob(void) {
    char *s = malloc(4);
    memcpy(s, "1.5", 4);
    char *end = NULL;
    ESCAPE(&end);
    strtod(s, &end);
    char c = end[4];
    free(s);
    return c;
}
