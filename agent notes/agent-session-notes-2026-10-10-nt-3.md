# Agent session notes 2026-10-10 (dev_frame NT-3, issue #80)

- `*KnownBug` suites get the ctest label `known_bug` via two `gtest_discover_tests` calls in `Source/Core/CMakeLists.txt` (`TEST_FILTER`, needs CMake >= 3.22). `Tools/Test.sh` runs `ctest -LE known_bug`, so they stay out of the gate.
- `Cylinder::GetRotation_rad` for an axis parallel to +z aborts in Debug (`Vec3D::operator/` asserts a zero divisor, `VecTools.cpp:38`) and yields NaN in Release. `CylinderKnownBug.test_GetRotation_rad_axis_parallel_z_is_finite_zero` therefore shows as "Subprocess aborted" under ctest; run known-bug tests one at a time with `--gtest_filter`.
- `TruncNorm` statics are per process: the fixture tests pass only because every instance is `(0.1, 2.0)`. Any new test with other parameters must go in a `*KnownBug` suite until dev_frame #101 is fixed.
- Rotation sign convention of `Box::Rotations_rad` is not documented, so the Box rotation test uses pi/2 about z where the sign does not matter.
