[![Codacy Badge](https://app.codacy.com/project/badge/Grade/64207ebc26b34f24b1ad39ad43df315d)](https://www.codacy.com/gh/carboncopies/BrainGenix-NES/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=carboncopies/BrainGenix-NES&amp;utm_campaign=Badge_Grade)

# This project is currently in development


# Roadmap

We'd like to finish the Ball and Stick implementation by the end of January - as this would give us time to make the first iteration of the standardized challenge by end of Febuary.

To do so, the following will need to be done:

[ ] Implement new API routes to finalize simulator integration. (~3-4 hours, assigned to Deb)
[ ] Implement new API routes for calcium imaging. (~1-2 hours, assigned to Deb)
[ ] Add NES RPC handlers for the above routes. (~4-5 hours, assigned to Deb)

[ ] Add backend logic for simulator routes. (~10-20 hours, assigned to Randal)
[ ] Add backend logic for caclium imaging routes. (~5-8 hours, assigned to Thomas)
[ ] Implement VSDA slice->mesh/texture generator for renderer. (~10-20 hours, assigned to Thomas)



# What is BrainGenix-NES?

BrainGenix Neuron Emulation System (NES) aims to be a stable, fault-tolerant, and scalable distributed simulation platform for biologically realistic neuronal networks applicable to the emulation of human brains.

BrainGenix-NES is presently being developed in close coordination with the [Virtual Brain Platform (VBP)](https://gitlab.braingenix.org/carboncopies/virtual-brain/vbp), an in-silico experimentation platform for neuroscientists for which it provides the computational backend.

The primary application of BrainGenix-NES is as the computational backend for simulation efforts involved in the scientific exploration of whole brain emulation (WBE) and research and development of resulting methods for a) system identification and translation, b) validation metrics applied to an emulation, and c) effective implementation of a brain emulation.

The BrainGenix-NES platform integrates and interoperates closely with:

- [BrainGenix-STS](https://gitlab.braingenix.org/carboncopies/BrainGenix-STS): post-processing of acquired brain data.
- [BrainGenix-ERS](https://gitlab.braingenix.org/carboncopies/BrainGenix-ERS): embodiment and environment.
- [Virtual Brain Platform](https://gitlab.braingenix.org/carboncopies/virtual-brain/vbp): scientific exploration of system identification and translation to an emulation from brain data, and the validation of the emulation.


## Distributed processing and multi-resolution models

Distributed processing in BrainGenix-NES includes the distribution of tasks across multiple - potentially spatially separated - CPU architectures, as wells as GPUs. [Apache Zookeeper](https://zookeeper.apache.org/) and other production-grade systems are employed to ensure stability and fault-tolerance. Simulation tasks are expected to range from the simulation of detailed single-neuron models to brain architectures composed of billions of neurons.

A special feature of BrainGenix-NES is that it is designed from the ground-up to enable simulation across multiple resolutions and for model components at those different resolutions to interact sensibly. This is a requirement for efficient brain emulation design, because some aspects of brain function require high resolution modeling (e.g. [dendritic computation](https://pubmed.ncbi.nlm.nih.gov/16033324/)) while others operate well at a much lower resolution of replication (e.g. [ephaptic coupling](https://en.wikipedia.org/wiki/Ephaptic_coupling)).

## Working with BrainGenix-NES through the API

The [BrainGenix-API](https://gitlab.braingenix.org/carboncopies/BrainGenix-API) is the management API through which client systems can request BrainGenix-NES capabilities, can build simulations and can run them. The NES backend is largely written in C++, while client frontends can use any language that supports the necessary API calls and can process results returned. E.g., the VBP is largely written in Python at this time.

Many users or client front-ends can interact with BrainGenix-NES concurrently, running multiple simulations for which state is maintained.

## Getting started

- Source files documentation is processed and made available through [Doxygen in the Docs folder](https://gitlab.braingenix.org/carboncopies/BrainGenix-NES/-/tree/master/Docs/Doxygen).
- To compile, install and run BrainGenix-NES, run cmake on the main folder.

## Important folders

- Further documentation is supplied in the [Docs](https://gitlab.braingenix.org/carboncopies/BrainGenix-NES/-/tree/master/Docs) folder.
- Principal components of BrainGenix-NES reside in the [Core](https://gitlab.braingenix.org/carboncopies/BrainGenix-NES/-/tree/master/Source/Core) and [Renderer](https://gitlab.braingenix.org/carboncopies/BrainGenix-NES/-/tree/master/Source/Renderer) folders within the [Source](https://gitlab.braingenix.org/carboncopies/BrainGenix-NES/-/tree/master/Source) folder. The Renderer offers GPU-accelerated 3D rendering for the purpose of simulation and data display, as well as simulated data acquisition that is needed for the analysis of in-silico ground-truth models in the Virtual Brain Platform.

## Supported platforms

BrainGenix-NES compiles and runs on modern Linux distributions.
