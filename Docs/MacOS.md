# Apple Silicon macOS

BrainGenix-NES supports Apple Silicon macOS through the normal server workflow.

## Prerequisites

- Apple Silicon Mac
- Xcode Command Line Tools
- Homebrew
- MoltenVK installed by the setup script

Install Xcode Command Line Tools if needed:

```bash
xcode-select --install
```

Then run setup from the repo:

```bash
cd Tools
./Setup.sh
```

`Setup.sh` auto-detects macOS and Linux and runs the appropriate setup path.

## Build and run

Use the same commands as the Linux VM workflow:

```bash
cd Tools
./Build.sh 6 Release
./Run.sh
```

`Run.sh` detects the Homebrew prefix and exports the MoltenVK ICD path through `VK_ICD_FILENAMES`. When MoltenVK is available, VSDA rendering can run through the Apple GPU using Vulkan portability support.

## Netmorph submodule

Apple Silicon builds currently require the Netmorph submodule commit recorded by NES. Avoid running a manual `git pull` inside `ThirdParty/NetmorphCMake` during setup, because that can move the submodule away from the commit NES was tested with.

Temporary note: macOS Apple Silicon support currently uses the Netmorph compatibility commit from the personal `johnjose/netmorph-c-make-john` fork while the same fix is being prepared for the official Carboncopies Netmorph repository. This is intended as a short-term Mac-only workaround. The long-term plan is to merge the minimal Apple Silicon fix upstream and return NES to depending only on the official Netmorph submodule source.

## Generated files

Build directories, binaries, virtual environments, and local logs are ignored by git. Keep generated build logs out of merge requests.
