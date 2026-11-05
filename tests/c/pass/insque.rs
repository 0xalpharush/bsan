//@run:0
// Pointers that are written by uninstrumented code must not be checked
// against the provenance that was previously stored in the same location.
use c_deps as _;

unsafe extern "C" {
    fn insque_relink() -> i32;
}

fn main() {
    assert_eq!(unsafe { insque_relink() }, 14);
}
