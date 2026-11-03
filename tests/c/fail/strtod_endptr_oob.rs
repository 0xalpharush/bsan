//@run: 1
use c_deps as _;

unsafe extern "C" {
    fn strtod_endptr_oob() -> std::ffi::c_char;
}

fn main() {
    unsafe { strtod_endptr_oob() };
}
