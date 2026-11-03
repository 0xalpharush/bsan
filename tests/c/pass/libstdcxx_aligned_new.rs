//@run:0
// Instrumented code trusts `operator new` to return the provenance of the
// new allocation. This must also hold for the aligned form, which libstdc++
// implements with `posix_memalign`, or else the returned pointer would be
// checked against a stale provenance value, like that of `stale` below.
use c_deps as _;

#[link(name = "stdc++")]
unsafe extern "C" {
    fn aligned_new_in_bounds() -> u64;
}

fn main() {
    let stale = Box::new([0u8; 8]);
    assert_eq!(unsafe { aligned_new_in_bounds() }, 5);
    drop(stale);
}
