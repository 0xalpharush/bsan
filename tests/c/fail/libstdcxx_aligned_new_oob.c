#include <stddef.h>
#include <stdint.h>

void *_ZnwmSt11align_val_t(size_t size, size_t align);
void _ZdlPvSt11align_val_t(void *ptr, size_t align);

// The allocation keeps its own provenance, so accesses past its end are
// still reported.
uint64_t aligned_new_oob(void) {
    uint64_t *p = _ZnwmSt11align_val_t(64, 64);
    uint64_t v = p[8];
    _ZdlPvSt11align_val_t(p, 64);
    return v;
}
