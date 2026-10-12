# Agent session notes 2026-10-11 (dev_frame NT-24, issue #146)

- Every discovered test now has a ctest `TIMEOUT` (`NES_TEST_TIMEOUT_SECONDS`, default 60, cache variable in `Source/Core/CMakeLists.txt`). Slowest gate test is about 0.7 s (the RPCManager tests, which pay the manager start/stop); 60 s leaves about 85x headroom for a loaded shared box.
- ctest reports a timed-out test as `***Timeout` and exits 8. `Tools/Test.sh` and `Tools/Coverage.sh` propagate that exit code, and also pass `--timeout` (default 60, env `NES_TEST_TIMEOUT_SECONDS`) as a backstop for tests registered without the property.
- `gtest_discover_tests` takes `PROPERTIES LABELS known_bug TIMEOUT 60` as one list; no quoting needed.
- The #105 hang (`~RPCManager` with no `SetupCallback`) listens on 0.0.0.0 while it hangs; a deliberate demo showed ctest killing it at 60 s. The killed test leaves nothing behind (ctest kills the process).
- Only `Source/Core/RPC/RPCManager.test.cpp` constructs `RPCManager` on this branch; `TestUtil/RouteHarness.h` arrives with NT-11. Any new test that constructs a manager must call `SetupCallback` (or use the harness).
