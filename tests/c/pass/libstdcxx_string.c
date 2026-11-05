#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

// Mirrors `std::__cxx11::basic_string<char>` in the C++11 ABI: a data pointer,
// a length, and a union holding either the 16-byte local buffer or the heap
// capacity.
struct bsan_string {
    char *p;
    size_t len;
    union {
        char buf[16];
        size_t cap;
    } u;
};

// The out-of-line construction routine in GNU libstdc++. Inlined `std::string`
// code calls it and then reads the data pointer it writes. The trailing
// `forward_iterator_tag` is an empty type and takes no argument register.
void _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tag(
    struct bsan_string *self, const char *beg, const char *end);
void _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(
    struct bsan_string *self);

static const char DATA[] =
    "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLM";

// Reproduces the false positive: a concrete, soon-to-be-freed provenance with
// a matching fingerprint is planted in the data pointer's shadow slot. The
// string is then reconstructed out of line; libstdc++'s internal allocation
// reuses the just-freed block, so the data pointer is rewritten with the same
// value. Without refreshing its shadow, the stale (freed) provenance is reused
// and an in-bounds read is wrongly reported as a use-after-free.
int libstdcxx_string_reuse(void) {
    struct bsan_string s;
    s.p = s.u.buf;
    s.len = 0;

    char *stale = (char *)malloc(49);
    s.p = stale; // instrumented store: records {prov(stale), fingerprint(stale)}
    s.len = 48;
    free(stale); // prov(stale) is now freed

    // Out-of-line reconstruct; the internal malloc(49) reuses `stale`.
    _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tag(
        &s, DATA, DATA + 48);

    volatile char c = s.p[40]; // in bounds for the real 48-char buffer
    _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(&s);
    return (int)(unsigned char)c;
}
