# BrainGenix-NES: Overview

BrainGenix-NES (Neuron Emulation System) is a cutting-edge simulation platform developed by the Carboncopies Foundation's BrainGenix initiative. It's designed to support research in whole brain emulation (WBE), aiming to replicate brain function in a digital environment. BrainGenix-NES complements the Virtual Brain Platform (VBP), also developed by BrainGenix, which serves as an in-silico experimentation space for neuroscientists.

## Introducing the Whole Brain Emulation Challenge (WBE Challenge)

To catalyze progress in this field, we've introduced the WBE Challenge — a series of standardized data sets and challenges aimed at improving neural circuit reconstruction methods. Drawing inspiration from successful AI challenges like ImageNet, the WBE Challenge provides a structured framework for researchers to develop and test their algorithms against controlled conditions before applying them to real-world data.


## Virtual Scan Data Acquisition (VSDA) System

A notable feature of BrainGenix-NES is its Virtual Scan Data Acquisition (VSDA) system, which simulates two types of data acquisition:  
 1. **VSDA EM**: Renders Electron Microscopy (EM) stacks, enabling researchers to test their reconstruction methods on known-ground truth models.  
 2. **VSDA CA**: Simulates Calcium imaging, enabling the study of neuronal activity patterns within the virtual brain environment.  

## Visualization with Neuroglancer

To facilitate analysis of VSDA EM stacks, BrainGenix-NES integrates with [Neuroglancer](https://github.com/google/neuroglancer) (a visualization tool developed at Google Inc.) — a powerful visualization tool that allows researchers to navigate and examine the complex structures of simulated brain tissue in three dimensions.


## Important folders

- Further documentation is supplied in the [Docs](https://gitlab.braingenix.org/carboncopies/BrainGenix-NES/-/tree/master/Docs) folder.
- Principal components of BrainGenix-NES reside in the [Core](https://gitlab.braingenix.org/carboncopies/BrainGenix-NES/-/tree/master/Source/Core) and [Renderer](https://gitlab.braingenix.org/carboncopies/BrainGenix-NES/-/tree/master/Source/Renderer) folders within the [Source](https://gitlab.braingenix.org/carboncopies/BrainGenix-NES/-/tree/master/Source) folder. The Renderer offers GPU-accelerated 3D rendering for the purpose of simulation and data display, as well as simulated data acquisition that is needed for the analysis of in-silico ground-truth models in the Virtual Brain Platform.

## Join the BrainGenix Community

By contributing to or participating in BrainGenix-NES and the WBE Challenge, you are part of a global effort to unravel brain function's intricacies. Your support helps us refine tools and methodologies that could lead to transformative breakthroughs in neuroscience, medicine, and technology. For more information or to get involved, please visit our website or contact us at contact@carboncopies.org.


## Supported platforms

BrainGenix-NES compiles and runs on modern Linux distributions.
