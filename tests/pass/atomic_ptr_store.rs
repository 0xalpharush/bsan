//@compile-flags: -Zverify-llvm-ir=yes
//@run:0
// Instrumenting an atomic store reads the previous shadow value of any
// partially-overwritten provenance slot. That read must not inherit the
// store's `Release`/`AcqRel` ordering: a load may only be `NotAtomic`,
// `Unordered`, `Monotonic`, `Acquire` or `SeqCst`. The pass used to emit a
// `release`-ordered load ("Load cannot have Release ordering"), which the LLVM
// verifier rejects. `-Zverify-llvm-ir` runs the verifier, so this test fails
// to compile unless the ordering is downgraded for the shadow read.
use std::sync::atomic::{AtomicPtr, AtomicU32, Ordering};

fn main() {
    // Sub-word stores: their end is not slot-aligned, so instrumentation reads
    // the clobbered slot. This is where the release-ordered load was emitted.
    let a = AtomicU32::new(0);
    a.store(7, Ordering::Release);
    a.store(8, Ordering::SeqCst);
    let _ = a.compare_exchange(8, 9, Ordering::AcqRel, Ordering::Acquire);

    // Pointer stores/exchanges with release and acq_rel orderings.
    let mut x = 0i32;
    let mut y = 1i32;
    let p = AtomicPtr::new(&mut x as *mut i32);
    p.store(&mut y as *mut i32, Ordering::Release);
    let _ = p.swap(&mut x as *mut i32, Ordering::AcqRel);
    let _ = p.compare_exchange(
        &mut x as *mut i32,
        &mut y as *mut i32,
        Ordering::Release,
        Ordering::Relaxed,
    );

    assert_eq!(a.load(Ordering::Acquire), 9);
    let _ = p.load(Ordering::Acquire);
}
