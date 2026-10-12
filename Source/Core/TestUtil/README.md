# Test kit (`TestUtil`)

Test-only helpers, compiled into `BrainGenix-NES.test` and never into the `braingenix_nes` library.
Include with `#include <TestUtil/TestUtil.h>`. New test files must be added to `UNITTEST_SOURCES`
in `Source/Core/CMakeLists.txt`.

| Helper | Use |
|---|---|
| `MakeSilentLogger()` | `LoggingSystem` with console and file output off |
| `FindFreePort()`, `MakeLoopbackConfig(root)` | `Config` bound to `127.0.0.1` on a free port. Never `0.0.0.0`, never a fixed port |
| `ScopedTempOutputRoot` | unique `mkdtemp` dir, calls `Util::Storage::SetOutputBasePath`, restores and removes it afterwards. Tests must never write under `/var/lib/BrainGenix/NES` |
| `TempOutputRoot` | gtest fixture bundling the three above (`RootPath()`, `Logger_`, `Config_`) |
| `MakeRequest`, `MakeRequestWithId`, `Dump`, `ParseResponse`, `ResponseOk` | JSON request/response helpers (`StatusCode` 0 = ok) |

`EMTestHelpers.h` (dev_frame NT-14, VSDA EM pipeline tests; include it only from a `*.test.cpp`):

| Helper | Use |
|---|---|
| `WaitFor(pred, timeout_ms)` | poll a condition (e.g. `Task::IsDone_`) with a timeout, no fixed sleeps |
| `WriteGreyPng`, `LoadImageFile`, `ReadFileBytes`, `WriteFileBytes` | write source images and read PNG/JPEG output back through stb_image (the stb implementations live in the library) |
| `DecodeCompressedSegmentation`, `ReadSegWords` | independent decoder for the neuroglancer compressed_segmentation format (the library only has the encoder), for round-trip tests |
| `MakeStubVenv(dir, script)` | temp `bin/python` shell script standing in for the virtualenv that `ProcessIgneousPipeline` runs |
| `ArmChildCleanup(root)`, `ChildExit(code)` | inside a death-test statement: remove the child's own temp root on exit, `std::terminate` or SIGABRT/SIGFPE/SIGSEGV (the child re-runs SetUp but never TearDown) |
| `MakePlainParams`, `ConfigurePlainTask` | `MicroscopeParameters` / `ProcessingTask` with noise, blur, interference, contrast, borders and tearing all off |

Pool tests: `ImageProcessorPool`/`ArrayGeneratorPool` with 0 threads is only a queue (`CreateVoxelArrayFromSimulation` hangs on it as soon as
one task exists, and `ExecuteConversionOperation` hangs on a 0-thread `ConversionPool`), so use 1 thread and poll `IsDone_`. Call `srand` before
constructing an `ImageProcessorPool`: its worker takes the seed of its noise generator from `rand()`. For a bug that crashes the process, run
the scenario in a gtest death-test child and expect exit code 0 (`EXPECT_EXIT(..., ExitedWithCode(0), "")` with `death_test_style=threadsafe`).

Seed every RNG, compare floats with `EXPECT_NEAR`.

## Marking a known bug (dev_frame D8)

When a test finds a real bug, the test asserts the **correct** behaviour (never weaken it) and is
named with a suite name ending in `KnownBug`, plus a comment linking the `type::bug` issue:

```cpp
// BUG: https://gitlab.braingenix.org/carboncopies/dev_frame/-/issues/NNN
TEST(TruncNormKnownBug, InstancesAreIndependent) { ... }
```

It is the *suite* name that must end in `KnownBug`; with a fixture, derive one (`struct PoolFixtureKnownBug : PoolFixture {};` then `TEST_F(PoolFixtureKnownBug, ...)`).

`Source/Core/CMakeLists.txt` registers tests whose suite name matches `*KnownBug` with the ctest label `known_bug`
(a second `gtest_discover_tests` call with `TEST_FILTER`), and registers all others without it.

```bash
ctest -LE known_bug     # the gate (Tools/Test.sh, Tools/Coverage.sh): known bugs excluded
ctest -L  known_bug     # lists / runs the known bugs; expected to fail until fixed
```

When the bug is fixed, rename the suite (drop the `KnownBug` suffix) in the same MR.
