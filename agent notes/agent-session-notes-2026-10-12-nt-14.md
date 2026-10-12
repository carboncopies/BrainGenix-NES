# NT-14 agent notes (2026-10-12)

- After review (MR !265): gate tests must not pin bugs (D8). A gate test that asserts a buggy number (#143 task count, #138 throw/IsDone_) goes red when the bug is fixed; assert only what holds before and after, and put the correct behaviour in a `*KnownBug` suite.
- Prove bug-independence by patching the production bugs locally, rebuilding and running the gate; never commit the patch.
- Death-test children (`threadsafe`) re-run SetUp and leave by `_exit`/crash, so TearDown never runs: use `ArmChildCleanup`/`ChildExit` from EMTestHelpers.h to remove the child's temp root.
- A KnownBug test that passes both with and without the fix (UB luck, #144) is `DISABLED_`, not left as a "passing" known bug.
- Observed, not NT-14: `--gtest_shuffle --gtest_repeat=3` fails 4 `TruncNormTest.*_default` tests from iteration 1 on (shared TruncNorm state, see TruncNormKnownBug.test_InstanceIndependence); a single shuffled run passes.
