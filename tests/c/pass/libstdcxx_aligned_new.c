#include <stddef.h>
#include <stdint.h>

// The aligned forms of `operator new` and `operator delete`. GNU libstdc++
// implements `operator new` with `posix_memalign`.
void *_ZnwmSt11align_val_t(size_t size, size_t align);
void _ZdlPvSt11align_val_t(void *ptr, size_t align);

uint64_t aligned_new_in_bounds(void) {
    uint64_t *p = _ZnwmSt11align_val_t(64, 64);
    p[7] = 5;
    uint64_t v = p[7];
    _ZdlPvSt11align_val_t(p, 64);
    return v;
}
