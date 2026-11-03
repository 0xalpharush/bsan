//@run: 1
use c_deps as _;

#[link(name = "stdc++")]
unsafe extern "C" {
    fn aligned_new_oob() -> u64;
}

fn main() {
    unsafe { aligned_new_oob() };
}
