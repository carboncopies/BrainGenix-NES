# Agent Session Notes — 2026-06-27

## Session goal

Understand today's work on `feature/mac-silicon-acceleration`, identify the EM rasterization bottleneck, establish a reproducible self-test benchmark, and get a baseline end-to-end timing number.

---

## Code review findings

### Bottleneck analysis

The dominant cost in EM acquisition is `FillCylinderPart` in `Source/Core/VSDA/EM/VoxelSubsystem/ShapeToVoxel/ShapeToVoxel.cpp`. It iterates the full axis-aligned bounding box (AABB) of each cylinder and tests each voxel with a projection + distance check. For diagonal axons the AABB volume is much larger than the cylinder volume, so most voxels are rejected after iteration. This is the single biggest opportunity for speedup.

Secondary: `Metal_FillSphere` (GPU accelerated sphere fill) is guarded by `_TotalThreads == 1` (lines 153–159). Large soma spheres are subdivided into `NumSegments > 1` sub-tasks, so the GPU path is never reached for the shapes that matter most.

Full ranked list is in `agent notes/mac-silicon-change-log.md` and the persistent memory file `memory/project_em_bottlenecks.md`.

### Bug fixed: dangling ShapePtr crash

`Source/Core/VSDA/EM/VoxelSubsystem/VoxelArrayGenerator.cpp` line 127 read `ShapePtr->ID` to get the shape ID for each compartment. `ShapePtr` is a raw pointer into `GeometryCollection::Geometries` (a `std::vector`). Any `push_back` that triggers reallocation invalidates all existing pointers, making `ShapePtr` dangling. Reading through it yields a garbage ID, which then throws `std::out_of_range` inside `GeometryCollection::GetShapeType`.

Fix: cast the `Compartment*` from `GetCompartmentByIdx` to `BS*` and read `ShapeID` directly. `ShapeID` is stored by value in `BSBaseData` and is always valid.

```cpp
// Before (dangling pointer dereference):
int ShapeID = _Sim->GetCompartmentByIdx(i)->ShapePtr->ID;

// After:
Compartments::BS* ThisCompartment = static_cast<Compartments::BS*>(_Sim->GetCompartmentByIdx(i));
if (!ThisCompartment) continue;
int ShapeID = ThisCompartment->ShapeID;
```

Committed: `632fac05e` on `feature/mac-silicon-acceleration`.

### Benchmark test API compatibility

All `Benchmarking/Renderer/Test*.py` files were missing two fields required by the updated Python client:
- `SimulationCfg.Seed = 0` — new required field on SimulationCfg
- `[0, 0, 0]` rotation argument to `DefineScanRegion()` — new required positional param

Both patched in the same commit `632fac05e`.

---

## Benchmark infrastructure discovery

### Build mode was Debug (-O0)

All prior work on this branch ran on a Debug build (`Build/BuildType = "Debug"`, `CMAKE_CXX_FLAGS_DEBUG = -g` only). Rasterization at -O0: ~3.5 it/s → 5+ hour EM acquisition. Switching to `RelWithDebInfo` (-O2 + symbols): 70–115 it/s → 237s acquisition.

### Memory config caused swap thrash

`Binaries/NES.yaml` `VSDA_EM_PercentOfSysteMemoryLimit: 70` allocates 70% of total RAM (23 GB on a 64 GB machine) as a single VoxelArray per subregion. With only ~4 GB actually free at session time, this causes massive swap I/O — shapes that should take milliseconds took 3 seconds each.

Fix: set `VSDA_EM_PercentOfSysteMemoryLimit: 6` (~3.8 GB). More, smaller subregions, each fitting in available RAM. Net result is much faster despite more subregion passes.

**This setting should be tuned to available free RAM, not total RAM.** A value of 6–10% is appropriate for a 64 GB machine with other processes running.

### Fine-resolution tests (Test4.py, 0.05 μm/vox) OOM on large-RAM machines

The EMRenderer sizes subregions based on total system RAM. On a 64 GB machine at 70%, the subregion at 0.05 μm/vox is 1280×1280×1443 voxels × 16 bytes = 37 GB — larger than total free RAM. The `posix_memalign` call succeeds (virtual memory), but `ClearArrayThreaded` triggers page faults that exhaust physical RAM → segfault or OOM kill. This is a pre-existing issue unrelated to today's changes. Not fixed; tracked as a known limitation.

---

## Baseline benchmark result

**Branch:** `feature/mac-silicon-acceleration`  
**Build:** RelWithDebInfo (`./Build.sh N RelWithDebInfo`)  
**Config:** `VSDA_EM_PercentOfSysteMemoryLimit: 6`  
**Scene:** xor_scnm, `-Resolution_um 0.1`, `-SubdivideSize 10`  
**Machine:** Apple Silicon, 64 GB RAM  
**Date:** 2026-06-27

| Phase | Time |
|---|---|
| Reservoir (Netmorph via NES API) | 712s |
| Connectome tuning | ~0s |
| **EM Acquisition** | **237s** |
| **Total E2E** | **949s** |

EM acquisition is the target metric for optimization. Baseline is **237s**.

---

## Follow-up regression check

Date: 2026-06-27

After pulling additional work from another agent, the benchmark was rerun from:

```bash
cd /Users/apple/fun_project/mac_silicon/optimisations/BrainEmulationChallenge/src/models/xor_scnm
/usr/bin/time -p ./Run.sh -H localhost -P 8000
```

Services used:

- NES: `/Users/apple/fun_project/mac_silicon/optimisations/BrainGenix-NES`
- API: `/Users/apple/fun_project/mac_silicon/optimisations/BrainGenix-API`
- Challenge: `/Users/apple/fun_project/mac_silicon/optimisations/BrainEmulationChallenge`

Result:

- `real 2780.62s` (`46m 21s`)
- `user 17.68s`
- `sys 4.89s`

Critical finding: this run used `Binaries/NES.yaml` with `VSDA_EM_PercentOfSysteMemoryLimit: 70`, not the tuned value `6` used by the faster baseline above. That makes the run non-comparable to the earlier metric. The 70% setting creates much larger voxel subregions and was already identified as a cause of poor Apple Silicon behavior because it increases memory-bandwidth pressure and can trigger swap depending on free RAM.

Live monitoring during the 46-minute run showed:

- No swap activity at the OS level (`Swapins: 0`, `Swapouts: 0`).
- NES RSS around 24 GB.
- Early first-region rasterization around 9-10 tasks/sec.
- Later stages using high multicore CPU, so the slowdown is not simply single-threaded execution.
- Region 0 had a large raster queue (`5852` tasks), and post-raster/image-processing work added significant hidden wall time after the visible queue ended.

Conclusion:

- The pulled ShapeID change was correctness-motivated but unsafe for LIFC simulations because it cast every compartment to `BS*`; this was fixed by reading `ShapeID` from the simulation's concrete compartment storage.
- The immediate regression versus the fast metric is most likely config drift from `6` back to `70`, not the ShapeID fix itself.
- Future comparisons should explicitly record `Build/BuildType`, `VSDA_EM_PercentOfSysteMemoryLimit`, repo paths, and whether the timing is EM acquisition-only or full end-to-end.

Action taken:

- Restored `VSDA_EM_PercentOfSysteMemoryLimit: 6` in tracked source config `Source/Data/NES.yaml`.
- Rebuilt/restarted NES so the runtime `Binaries/NES.yaml` was regenerated with the tuned value.

Corrected acquisition-only rerun:

```bash
cd /Users/apple/fun_project/mac_silicon/optimisations/BrainEmulationChallenge/src/models/xor_scnm
/usr/bin/time -p ./Run.sh -x a -H localhost -P 8000
```

Result with `RelWithDebInfo` NES, `VSDA_EM_PercentOfSysteMemoryLimit: 6`, API on localhost:8000:

- `real 312.82s` (`5m 13s`)
- `user 4.91s`
- `sys 1.25s`

Interpretation:

- This recovered the run from the bad `46m 21s` result back into the same order of magnitude as the earlier `237s` EM-acquisition metric.
- It is still slower than the best recorded `237s`, so the config drift explains the major regression but not all remaining delta.
- The remaining likely drag is still inside EM rasterization/image processing, especially the cylinder AABB scan path and hidden per-region image-processing/finalization time.

Failed follow-up experiment:

- Tried reducing rasterization and image-processing completion polling sleeps from `1000ms` to `100ms` while throttling logs to once per second.
- Rebuilt NES in `RelWithDebInfo`, restarted NES, and reran the same acquisition-only command.
- Result: `real 456.74s`, `user 3.93s`, `sys 0.96s`.
- Decision: reverted the code change and did not keep it. The increased polling cadence likely added progress/status churn or contention without addressing the dominant rasterization/image-processing work.

---

## How to run the benchmark

Use `nohup` or a detached script — acquisition runs 4–20 min and will be killed by terminal timeouts if run inline.

```bash
# Build
cd Tools && ./Build.sh $(sysctl -n hw.ncpu) RelWithDebInfo

# Confirm Source/Data/NES.yaml: VSDA_EM_PercentOfSysteMemoryLimit: 6

# Start servers
cd Tools && ./Run.sh >/tmp/nes.log 2>&1 &
cd ../BrainGenix-API/Tools && ./Run.sh >/tmp/api.log 2>&1 &
until nc -z localhost 8000; do sleep 1; done

# Run (use nohup or a script file)
XOR=../BrainEmulationChallenge/src/models/xor_scnm
VENV=../BrainEmulationChallenge/venv/bin/python3
PYPATH=../BrainEmulationChallenge/PythonClient
export PYTHONPATH=$PYPATH
cd $XOR
$VENV xor_scnm_groundtruth_reservoir.py   -Host localhost -Port 8000 -modelfile nesvbp-xor-res-sep-targets -modelname apple-xor_scnm
$VENV xor_scnm_groundtruth_connectome.py  -Host localhost -Port 8000 -modelname apple-xor_scnm
time $VENV xor_scnm_acquisition_direct.py -Host localhost -Port 8000 -modelname apple-xor_scnm-tuned -RenderEM -SubdivideSize 10 -Resolution_um 0.1
```

---

## Next optimization targets (in priority order)

1. **Cylinder AABB** — axis-walk along cylinder axis, compute per-slice circle bounds, skip empty AABB region. Expected: largest single speedup.
2. **Metal sphere path** — remove `_TotalThreads == 1` guard so GPU fills large subdivided soma spheres.
3. **CompositeVoxelAtIndex** — inline index computation, single bounds check, direct write.
4. **Polling loops** — replace `while (queue != 0) sleep_for(1000ms)` with `WaitUntilIdle()` in VoxelArrayGenerator and EMSubRegion.
