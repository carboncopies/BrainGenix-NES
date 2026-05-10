# BrainGenix-NES Agent Session Notes

Date: 2026-05-09

## Purpose

Review Apple Silicon support for NES and keep the user-facing workflow aligned with the Linux VM workflow:

```bash
cd BrainGenix-NES/Tools
./Setup.sh
./Build.sh 6 Release
./Run.sh
```

The goal is for macOS-specific setup, Vulkan/MoltenVK configuration, and Netmorph compatibility to be detected automatically while keeping Linux behavior unchanged.

## Branch and state observed

- Branch: `feature/mac_silicon_support`
- Local branch observed ahead of `origin/feature/mac_silicon_support`
- Compared against refreshed `origin/master`, which had advanced to `35a92ef5b`
- Working tree changes observed in this repo:
  - `Source/Core/Simulator/RPC/SimulationRPCInterface.cpp`
  - `ThirdParty/NetmorphCMake`
  - `Tools/Run.sh`
  - `Tools/Setup-MacOS.sh`
  - `Tools/Setup.sh`
  - `agent notes/`

## Work reviewed

- `Tools/Setup.sh`
  - Adds early Darwin detection.
  - Delegates macOS setup to `Tools/Setup-MacOS.sh`.
  - Leaves the existing Linux setup path in place.

- `Tools/Setup-MacOS.sh`
  - Uses recursive pinned submodule initialization.
  - Stops silently pulling `ThirdParty/NetmorphCMake` to `origin/main` during setup.
  - Applies the vcpkg macOS patch idempotently.
  - Keeps macOS dependency setup in the dedicated macOS script while still reachable through normal `./Setup.sh`.

- `Tools/Run.sh`
  - On Darwin, detects the Homebrew prefix.
  - Exports `VK_ICD_FILENAMES` for MoltenVK when the ICD file exists.
  - Prepends Homebrew `lib` to `DYLD_LIBRARY_PATH`.
  - This is what allows the normal run script to enable VSDA rendering on Apple Silicon instead of leaving rendering disabled.

- `Source/Core/Simulator/RPC/SimulationRPCInterface.cpp`
  - Guards Linux-only resource checks behind `__linux__`.
  - Adds a macOS Mach VM statistics path behind `__APPLE__`.
  - Keeps glibc heap accounting Linux-only because `mallinfo2` is not available on macOS.

- `ThirdParty/NetmorphCMake`
  - Points to `5c26c4aa84ad08a7f8f2bba7d650c0581dba7ace`.
  - This was restored because the upstream-main Netmorph pointer `1a71cc3f688b740d65432605859a22cc9186edad` failed to build on macOS when `Source/version` shadowed libc++ `<version>`.
  - Temporary macOS note: this commit is currently available from the personal `johnjose/netmorph-c-make-john` fork. The fork should be treated as a short-term Apple Silicon compatibility source only; the intended follow-up is to upstream the minimal fix into the official Carboncopies Netmorph repository and point NES back to the official submodule source.

## Review outcome

The NES changes are mostly platform isolation and runtime environment setup, which is the right shape for an Apple Silicon support branch. The important integration decision is the Netmorph submodule pointer: NES currently needs the Mac-support Netmorph commit unless Netmorph main receives the same fix.

The upstream merge initially made the RPC/model diff look suspicious, but after refreshing against current master the neuron editing declarations were confirmed as real upstream APIs and were restored. No header cleanup remains as part of this review.

## Verification performed

- Shell syntax check passed for setup/build/run scripts.
- `git diff --check` passed for NES.
- NES rebuilt successfully on Apple Silicon with:

```bash
cd BrainGenix-NES/Tools
./Build.sh 1 Release
```

- `vulkaninfo --summary` saw the Apple GPU through MoltenVK when the same environment variables were set.
- NES logged successful headless Vulkan setup during the integrated run.
- The integrated `xor_scnm` run completed through VSDA EM rendering and reached `OK Freeing Voxel Array`.

## Follow-ups before merge request

- Decide whether the NES MR should include the Netmorph submodule pointer update directly or wait for the same fix to land in Netmorph main.
- If the pointer update is included before Netmorph upstreaming is complete, call out that the personal fork is a temporary macOS-only workaround and should be replaced by the official Netmorph commit later.
- Keep the two preserved Netmorph stashes until there is no need to recover the local README/Readme case-collision notes.
- Confirm whether `agent notes/` should be committed or kept as local review documentation.
