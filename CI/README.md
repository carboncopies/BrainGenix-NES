# BrainGenix-NES CI Pipeline Overview

This document explains how the Continuous Integration (CI) pipeline is structured and operates for the BrainGenix-NES project.

---

## Overview

The CI pipeline is managed by GitLab CI/CD and is configured using the `.gitlab-ci.yml` file in the `CI` directory. The pipeline automates building, testing, packaging, and documenting the project across multiple Linux distributions and environments.

---

## Pipeline Structure

The pipeline is modular and organized by operating system and task. Each major task (building, testing, packaging, documentation, etc.) is defined in its own YAML file within subdirectories of the `CI` folder. Within each subdirectory
you will see a consistent OS version which is seen within each directory. 

Build, Images, Package, and Test Directory
They all have a folder Debian12 which is used to represent the Debian 12 OS
Any future designs should follow this structure
---

### Key Folders

- **Images/**: Contains configurations for building Docker images used as environments for CI jobs (e.g., Ubuntu, Debian, Doxygen).
- **Build/**: Contains build job definitions for different OS versions (e.g., Ubuntu 20.04, Ubuntu 22.04, Debian 12).
- **Test/**: Contains test job definitions for different OS versions.
- **Package/**: (If present) Contains packaging job definitions for distributing artifacts.
- **Docs/**: Contains documentation generation jobs (e.g., Doxygen).
- **Deploy/**: (If present) Contains deployment job definitions.

---

## How the Pipeline Runs

1. **Trigger**: The pipeline runs automatically when you push changes or open a merge request.
2. **Stages**: The pipeline executes jobs in the following order:
    - Update CI Containers
    - Build
    - Package
    - Deploy
    - Test
3. **Job Inclusion**: The main `.gitlab-ci.yml` uses the `include` keyword to pull in job definitions from the relevant subfolders.
4. **Artifacts & Caching**: Build outputs and logs are saved as artifacts. Dependencies are cached to speed up subsequent runs.
5. **Parallel & Matrix Builds**: The pipeline can build and test on multiple OS versions in parallel (e.g., Ubuntu 22.04, Debian 12).

---

## How to Use

- **Automatic Run**: Push any commit or open a merge request; the pipeline will start automatically.
- **Manual Run**: Go to your project's **CI/CD > Pipelines** page in GitLab and click "Run pipeline" to trigger a manual run.
- **Monitor**: View pipeline status and logs in the **CI/CD > Pipelines** section.

---

## Customization

- To add or remove support for an OS, edit the `include` section in `.gitlab-ci.yml` and add/remove the corresponding YAML files in the `CI` subfolders.
- To change build or test steps, edit the appropriate `