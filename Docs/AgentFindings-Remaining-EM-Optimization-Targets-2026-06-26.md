# Remaining EM Optimization Targets

Date: 2026-06-26
Repo: `BrainGenix-NES`
Branch: `feature/mac-silicon-acceleration`

## Purpose

This note captures the next likely optimization targets discovered during a code review of the current EM rendering path after the existing macOS Apple Silicon optimization work.

The findings below are based on the current repository state, current benchmark observations, and direct inspection of the relevant code paths.

## Highest-Priority Findings

### 1. `EMRenderer.cpp` has an X/Y planning bug

File:

- `Source/Core/VSDA/EM/EMRenderer.cpp`

Relevant lines:

- `NumImagesInYDimension_img` is computed from `BaseRegion->SizeX()` instead of `BaseRegion->SizeY()`
- `TotalImagesY_` is assigned the X-dimension count instead of the Y-dimension count

Why it matters:

- This is a correctness issue.
- It can distort planning, progress reporting, and any heuristics based on image totals.
- Even if it does not dominate runtime directly, it should be fixed before making deeper performance assumptions from render statistics.

### 2. `FillCylinderPart(...)` remains the strongest likely CPU hotspot

File:

- `Source/Core/VSDA/EM/VoxelSubsystem/ShapeToVoxel/ShapeToVoxel.cpp`

Relevant function:

- `FillCylinderPart(...)`

Why it matters:

- The implementation still performs nested loops over cylinder length, radius, and angle.
- It performs repeated `sin`, `cos`, vector rotation, and world-to-voxel conversion for every sampled point.
- This is materially heavier than the newer sphere path and remains the most likely reason rasterization still dominates end-to-end runtime.

Likely direction:

- Rewrite to work more directly in voxel index space or slice-local coordinates.
- Reduce repeated trigonometric work.
- Reduce duplicate writes caused by dense circular oversampling.

### 3. Shape preprocessing and task creation are still largely serial

File:

- `Source/Core/VSDA/EM/VoxelSubsystem/VoxelArrayGenerator.cpp`

Why it matters:

- The main thread still performs per-compartment region checks, type checks, size estimation, task construction, and queue submission serially.
- On large scenes, this preprocessing can become a front-end bottleneck before worker threads do the actual fill work.
- It also creates a large number of heap allocations for small task objects.

Likely direction:

- Batch task allocation.
- Reduce per-shape allocation churn.
- Consider parallelizing preprocessing and task preparation if memory ownership can be kept safe.

### 4. The EM pipeline still relies on polling-based waiting

Files:

- `Source/Core/VSDA/EM/VoxelSubsystem/EMSubRegion.cpp`
- `Source/Core/VSDA/EM/VoxelSubsystem/VoxelArrayGenerator.cpp`
- `Source/Core/VSDA/EM/VoxelSubsystem/ArrayGeneratorPool/ArrayGeneratorPool.cpp`

Why it matters:

- There are multiple loops that wait on queue size and then sleep.
- There are also per-task waits using `while (!IsDone_) sleep_for(...)`.
- This adds wall-clock latency, scheduler churn, and makes the pipeline less responsive and less precise about true completion state.

Likely direction:

- Add explicit in-flight work accounting.
- Add proper pool-level `wait_until_idle()` or equivalent completion signaling.
- Replace queue-size polling with condition-variable or counter-based completion waits.

### 5. Image processor queue accounting is incomplete

File:

- `Source/Core/VSDA/EM/VoxelSubsystem/ImageProcessorPool/ImageProcessorPool.cpp`

Why it matters:

- `GetQueueSize()` reflects only queued work, not in-flight work currently held by worker threads.
- Callers therefore still need a second phase of per-task `IsDone_` polling to know when work is truly finished.
- This increases synchronization overhead and is a sign the pool interface is not exposing the real completion state cleanly.

Likely direction:

- Track active worker count in addition to queue size.
- Expose a proper idle/wait primitive at the pool layer.

## Secondary Findings

### 6. Memory throttling is coarse and conservative

File:

- `Source/Core/VSDA/EM/EMRenderer.cpp`

Why it matters:

- Render admission uses a coarse `TotalConsumedMemory_MB` estimate and polls once per second.
- The logic assumes a worst-case region memory footprint for the whole render window.
- This can leave usable CPU and memory capacity idle.

Likely direction:

- Improve in-flight memory accounting.
- Shorten or eliminate polling where possible.
- Consider more granular admission around actual active region work.

### 7. Subregions are rendered strictly sequentially

File:

- `Source/Core/VSDA/EM/EMRenderer.cpp`

Why it matters:

- Region execution currently does not overlap later-stage work from one subregion with preprocessing of the next.
- This may underutilize the machine if memory allows partial pipeline overlap.

Likely direction:

- Investigate pipelining region preparation with later image-processing stages.
- Only attempt after better memory and completion accounting is in place.

### 8. Per-image filesystem work may still be a later-stage drag

File:

- `Source/Core/VSDA/EM/VoxelSubsystem/ImageProcessorPool/ImageProcessorPool.cpp`

Why it matters:

- Tasks still ensure directories exist and write files individually.
- Empty images may also copy a placeholder image file.
- This can become I/O-bound even after CPU loops improve.

Likely direction:

- Pre-create directories per slice or region.
- Cache directory creation state.
- Revisit the placeholder-image flow for empty outputs.

### 9. Task granularity remains fine-grained

Files:

- `Source/Core/VSDA/EM/VoxelSubsystem/VoxelArrayRenderer.cpp`
- `Source/Core/VSDA/EM/VoxelSubsystem/VoxelArrayGenerator.cpp`

Why it matters:

- The system still generates many small heap-backed tasks.
- Early heavy regions likely spend non-trivial time in queueing and bookkeeping.

Likely direction:

- Evaluate task pooling.
- Evaluate slice-level or block-level batching.

### 10. Per-voxel Perlin noise remains a possible dense-region hotspot

File:

- `Source/Core/VSDA/EM/VoxelSubsystem/ImageProcessorPool/ImageProcessorPool.cpp`

Why it matters:

- Dense images still compute noise per voxel for interior voxels.
- This may remain expensive, especially when later blur and resize steps reduce visible sensitivity to high-frequency detail.

Likely direction:

- Consider coarser noise sampling or caching if image quality permits.

## Additional Notes

### Duplicate or older image-processing path exists

File:

- `Source/Core/VSDA/EM/VoxelSubsystem/ImageProcessorPool/Working.cpp`

Why it matters:

- The repository still contains what appears to be an older image-processing path.
- Even if inactive, duplicate implementations increase maintenance burden and make performance work harder to reason about.

### Existing TODO in renderer matches current findings

File:

- `Source/Core/VSDA/EM/VoxelSubsystem/VoxelArrayRenderer.cpp`

Why it matters:

- The file already notes that more slice enumeration work should move into the threaded image-processing path.
- That is consistent with the current hotspot review and remains a valid future direction.

## Recommended Next Order Of Attack

1. Fix the X/Y image-count bug in `EMRenderer.cpp`.
2. Rewrite `FillCylinderPart(...)`.
3. Replace queue-size polling with explicit completion tracking and pool idle waits.
4. Reduce task creation and task allocation overhead in `VoxelArrayGenerator.cpp` and `VoxelArrayRenderer.cpp`.
5. Re-evaluate image-processing and Perlin noise costs after the above changes.

## Benchmark Context

This review is consistent with the current measured behavior:

- `Netmorph` is not the dominant contributor.
- The heaviest time is still spent in EM acquisition.
- Early-region rasterization remains the strongest visible bottleneck in live progress output.

## Follow-Up Results

The findings above were partially implemented and benchmarked locally after this note was first written.

### What helped

- Replacing fake cylinder "segmentation" with real geometric sub-cylinders improved the end-to-end benchmark from `1881.55s` to `1735.76s`.
- The biggest visible runtime improvement was better progress through the first large rasterization queue.
- Pool-level outstanding-work accounting and idle waits improved completion tracking and removed some misleading queue-empty states.

### What did not solve the problem fully

- The cylinder rewrite to a voxel-space bounding-box inclusion test did not recover the earlier best benchmark times by itself.
- The benchmark still remained slower than earlier better runs at `1285.36s`, `1450.59s`, and `1481.78s`.
- This suggests that while cylinder task partitioning was a real gain, the overall EM pipeline still has substantial remaining cost in later regions and image-processing stages.

### What appears to be correctness work rather than speed work

- The `EMRenderer.cpp` X/Y image-count fix is still worth keeping, but it should be treated primarily as a correctness fix.
- The improved queue accounting should also be treated mainly as correctness and observability work, with only modest direct speed impact expected.
