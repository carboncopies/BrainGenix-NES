# Agent session notes 2026-10-10 (dev_frame P0-4, issue #34)

- The test target needs `NetmorphCMake` and `ZLIB::ZLIB` on its link line (Simulation.h pulls in Netmorph headers).
- `Main.cpp` is excluded from the `braingenix_nes` library (`LIB_SOURCES`); the executable still compiles `MAIN_SOURCES` itself. Do not link with `--allow-multiple-definition`.
- `Tools/Test.sh` now propagates ctest's exit code and runs `ctest -LE known_bug`.
- Production changes that made old tests stale: `DoubleExponentExpr` is `amp*(-exp(-t/tr)+exp(-t/td))`; `BSAlignedNC` no longer appends to `ReceptorDataVec` (`BSAlignedNC.cpp:86`); `BSNeuron::UpdateConvolvedFIFO` no longer reverses or offsets (`BSNeuron.cpp:344` reads `ConvolvedFIFO[size-10]`, a hardcoded 10).
- BSAlignedNC, BSAlignedNCRandomUniform, BSMorphology and Simulator/Receptors/* are dead code (no runtime callers); their tests are kept but should be excluded from coverage numbers.
