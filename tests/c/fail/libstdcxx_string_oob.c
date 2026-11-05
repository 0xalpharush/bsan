#include <stddef.h>
#include <stdint.h>

struct bsan_string {
    char *p;
    size_t len;
    union {
        char buf[16];
        size_t cap;
    } u;
};

void _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tag(
    struct bsan_string *self, const char *beg, const char *end);
void _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(
    struct bsan_string *self);

static const char DATA[] =
    "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLM";

// The refreshed provenance describes the string's actual heap buffer, so a
// read past its end is still reported instead of being silently accepted.
int libstdcxx_string_oob(void) {
    struct bsan_string s;
    s.p = s.u.buf;
    s.len = 0;
    _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tag(
        &s, DATA, DATA + 48);
    size_t cap = s.u.cap;
    volatile char c = s.p[cap + 8];
    _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(&s);
    return (int)(unsigned char)c;
}
