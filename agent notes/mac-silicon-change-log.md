# BrainGenix-NES Mac Silicon Change Log

Reviewed on 2026-05-09 against refreshed `origin/master`.

## Current branch

- Branch: `feature/mac_silicon_support`
- Compared base: `origin/master` at `35a92ef5b`
- Upstream integration: merged refreshed `origin/master` into `feature/mac_silicon_support`.
- Local branch is ahead of `origin/feature/mac_silicon_support` because it contains the upstream master merge plus local Mac cleanup commits.
- Local status during review includes Mac setup/run script edits and this `agent notes` folder.

## Intent

Allow the same server workflow to work on both Linux VM and Apple Silicon macOS:

```bash
cd BrainGenix-NES/Tools
./Setup.sh
./Build.sh 6 Release
./Run.sh
```

Linux should continue to use the existing distro package manager path. macOS should route through Homebrew/MoltenVK setup without requiring the user to know a separate command.

## Changes made

- `Tools/Setup.sh`
  - Added early Darwin detection.
  - On macOS, delegates to `Tools/Setup-MacOS.sh`.
  - Linux behavior is otherwise left intact.

- `Tools/Setup-MacOS.sh`
  - Installs compiler, vcpkg, Vulkan/MoltenVK, and Python dependencies with Homebrew.
  - Creates the project `venv` and installs `igneous-pipeline`.
  - Updates submodules recursively using the pinned submodule commits.
  - No longer runs `git pull origin main` inside `ThirdParty/NetmorphCMake`, because setup should not silently move a submodule away from the commit recorded by NES.
  - Applies the vcpkg macOS patch idempotently: already-applied patches are accepted, unapplied patches are applied, invalid patch state exits.

- `Tools/Run.sh`
  - On macOS, detects the Homebrew prefix and exports the MoltenVK ICD path through `VK_ICD_FILENAMES`.
  - Prepends Homebrew `lib` to `DYLD_LIBRARY_PATH`.
  - This lets the normal `./Run.sh` path find MoltenVK and create a headless Vulkan device for VSDA rendering.

- `CMakeLists.txt`
  - Requires C++17.
  - Appends debug flags instead of replacing all existing C/C++ flags.

- Thread naming updates across simulator/VSDA worker pools
  - Uses the macOS `pthread_setname_np(const char*)` form under `__APPLE__`.
  - Keeps the Linux `pthread_setname_np(pthread_t, const char*)` form elsewhere.

- STB resize updates
  - Moves image resize users from `stb_image_resize.h`/`stbir_resize_uint8` to `stb_image_resize2.h`/`stbir_resize_uint8_linear`.
  - This aligns with the available newer STB API pulled through the current dependency set.

- VSG/Vulkan portability updates
  - Uses current VSG spelling `validateInstanceLayerNames`.
  - Adds `VK_KHR_portability_subset` on Apple.
  - Catches VSG instance creation exceptions so headless Vulkan failure disables VSDA rendering rather than aborting the NES server.
  - Guards viewer compilation when viewer setup fails.

- `Source/Core/Simulator/RPC/SimulationRPCInterface.cpp`
  - Upstream resource-status code uses Linux-only `<malloc.h>`, `<sys/sysinfo.h>`, `sysinfo`, and `mallinfo2`.
  - The Mac branch now guards that path under `__linux__` and uses Mach VM statistics under `__APPLE__`.
  - macOS reports system free/inactive/speculative memory; the optional glibc heap contribution remains Linux-only.

- `Tools/Patches/vcpkg/vcpkg-macos.patch`
  - Adds CMake policy compatibility for older ports.
  - Adds zlib macOS compile fixes needed by the current vcpkg state.

- `ThirdParty/NetmorphCMake`
  - Working tree points at `5c26c4aa84ad08a7f8f2bba7d650c0581dba7ace`, the Mac Silicon Netmorph support commit.
  - This pointer is needed on Apple Silicon: rebuilding with the upstream-main Netmorph commit `1a71cc3f...` failed because `ThirdParty/NetmorphCMake/Source/version` shadows libc++ `<version>` during C++ compilation.
  - Fresh Apple Silicon clones should record this submodule commit in NES unless the same fix lands upstream in Netmorph main.
  - Temporary fork note: this commit is presently on the personal `johnjose/netmorph-c-make-john` fork. That fork should be used only as an interim macOS compatibility source; the future target is to merge the minimal Apple Silicon fix into the official Carboncopies Netmorph repo and remove the need for any fork-specific dependency.

## Cleanup performed during review

- Initially removed two header-only RPC declarations from `Source/Core/Simulator/RPC/ModelRPCInterface.h` before upstream was refreshed:
  - `SCNeuronEdit`
  - `LIFCNeuronEdit`
- After merging current `origin/master`, these are real upstream APIs with implementations, so the declarations were restored and no RPC header cleanup remains in the working diff.

## Review notes

- The Mac setup flow is now discoverable through the normal `./Setup.sh` command.
- The refreshed upstream merge removed the misleading raw diff deletions in neuron-editing/model code.
- The Netmorph submodule pointer remains part of the Mac support surface, because NES does not build on macOS with the current main Netmorph pointer.
- The vcpkg submodule is updated relative to older branches; keep this only if the macOS patch and dependency versions require it.
- `Source/Core/VSDA/EM/VoxelSubsystem/ImageProcessorPool/Working.cpp` appears to duplicate the main image processor implementation. Keep the matching resize/thread changes only if this file is still compiled or used by local workflows.

## Verification

- `bash -n` passes for `Tools/Setup.sh`, `Tools/Setup-MacOS.sh`, `Tools/Build.sh`, and `Tools/Run.sh`.
- NES rebuild with Netmorph `1a71cc3f...` failed on macOS due `Source/version` shadowing libc++ `<version>`.
- NES rebuild with Netmorph `5c26c4aa...` progressed past Netmorph and exposed an upstream Linux-only `malloc.h` include, which was fixed with platform guards.
- After that fix, `./Build.sh 1 Release` completed and produced `Binaries/BrainGenix-NES`.
- Previous integrated run on Apple Silicon succeeded through API + NES + BrainEmulationChallenge `xor_scnm`.
- After adding the macOS MoltenVK run environment, `vulkaninfo --summary` outside the sandbox sees `Apple M1 Max` through MoltenVK, NES logs `Headless Vulkan setup successful.`, and the integrated `xor_scnm` VSDA EM render completed through `OK Freeing Voxel Array`.
- `diff --check` passes for NES.
- After the user's successful terminal pull, local `origin/master` was refreshed and merged into the feature branch.
- Netmorph has a macOS case-insensitive filesystem wrinkle: some commits expose both `Source/README` and `Source/Readme`. During integration those paths were locally marked assume-unchanged inside the submodule while testing the upstream-main pointer to avoid a false README/Readme rewrite on this machine.
- After the refreshed branch update and rebuild, the local API + NES + BrainEmulationChallenge `xor_scnm` integrated run completed through EM rendering and `OK Freeing Voxel Array`.
- The challenge venv needed `psutil` installed locally before the integrated script could start.
