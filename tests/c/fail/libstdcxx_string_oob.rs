//@run: 1
use c_deps as _;

#[link(name = "stdc++")]
unsafe extern "C" {
    fn libstdcxx_string_oob() -> i32;
}

fn main() {
    unsafe { libstdcxx_string_oob() };
}
