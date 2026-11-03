//@run:0
// `strtod` and friends write the end pointer from uninstrumented code. These
// tests would fail if the end pointer kept the provenance that was previously
// stored in its slot.
use c_deps as _;
use std::ffi::{c_char, CString};

unsafe extern "C" {
    fn strtod_after_unrelated(s: *const c_char, other: *const c_char) -> c_char;
    fn strtod_after_freed(s: *const c_char) -> c_char;
    fn strtol_after_reuse() -> c_char;
}

fn main() {
    let s = CString::new("1.5   ").unwrap();
    let other = Box::new([0u8; 10]);
    unsafe {
        assert_eq!(strtod_after_unrelated(s.as_ptr(), other.as_ptr().cast()) as u8, b' ');
        assert_eq!(strtod_after_freed(s.as_ptr()) as u8, b' ');
        assert_eq!(strtol_after_reuse() as u8, b' ');
    }
}
