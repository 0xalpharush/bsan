//@run:0
// GNU libstdc++ rewrites the links of `std::set` and `std::map` nodes
// from uninstrumented code. Links that are read afterward must not be
// checked against stale provenance, even if a node was freed and a new
// one was allocated at the same address.
use c_deps as _;

#[link(name = "stdc++")]
unsafe extern "C" {
    fn libstdcxx_rb_tree_churn(rounds: i32) -> i32;
}

fn main() {
    assert_eq!(unsafe { libstdcxx_rb_tree_churn(8) }, 8 * 120);
}
