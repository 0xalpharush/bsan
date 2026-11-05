//@run:0
// GNU libstdc++ rewrites a `std::string`'s data pointer from uninstrumented
// code. When a freed heap buffer is reused at the same address, the pointer is
// rewritten with an identical value, so the fingerprint check cannot see the
// overwrite. The stale provenance must be refreshed, or an in-bounds read is
// reported as a use-after-free (the duckdb `StringUtil::EndsWith` shape).
use c_deps as _;

#[link(name = "stdc++")]
unsafe extern "C" {
    fn libstdcxx_string_reuse() -> i32;
}

fn main() {
    assert_eq!(unsafe { libstdcxx_string_reuse() }, 'E' as i32);
}
