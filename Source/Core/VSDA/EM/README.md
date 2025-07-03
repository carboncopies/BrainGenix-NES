# EMRenderer Module Documentation

## Overview

The EMRenderer module is a core component of the BrainGenix Neural Emulation System (NES) responsible for rendering three-dimensional brain tissue into electron microscopy (EM) simulation images. This module provides memory-efficient processing of large neural datasets by implementing intelligent region subdivision and parallel processing capabilities.

## Purpose

The EMRenderer addresses the computational challenge of processing large-scale brain simulations by:

- **Memory Management**: Automatically dividing large 3D brain regions into memory-efficient subregions
- **Image Generation**: Creating high-resolution electron microscopy simulation images
- **Resource Optimization**: Monitoring and managing system memory usage during rendering operations
- **Parallel Processing**: Coordinating multiple rendering tasks across available system resources

## Architecture

### Core Components

#### Voxel System
Three-dimensional discrete units (voxels) represent the smallest addressable elements of brain tissue data. Each voxel contains:
- Spatial coordinates in micrometers (μm)
- Tissue properties and neural structure information
- Rendering parameters for image generation

#### Subregion Management
Large brain regions are automatically subdivided into smaller, memory-manageable subregions that:
- Fit within available system memory constraints
- Maintain spatial continuity through overlapping boundaries
- Enable parallel processing across multiple cores

#### Image Stack Generation
The module produces sequences of 2D images that, when combined, represent complete 3D brain tissue volumes with:
- Configurable resolution and sampling parameters
- Overlap regions for seamless reconstruction
- Compatibility with standard neuroimaging formats

## API Reference

### ExecuteSubRenderOperations

```cpp
bool ExecuteSubRenderOperations(
    Config::Config* _Config, 
    BG::Common::Logger::LoggingSystem* _Logger, 
    Simulation* _Simulation, 
    ImageProcessorPool* _ImageProcessorPool, 
    VoxelArrayGenerator::ArrayGeneratorPool* _GeneratorPool
);
```

#### Description
Primary function that orchestrates the complete rendering pipeline for electron microscopy simulation of brain tissue regions.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `_Config` | `Config::Config*` | System configuration containing memory limits and rendering parameters |
| `_Logger` | `BG::Common::Logger::LoggingSystem*` | Logging interface for operation tracking and debugging |
| `_Simulation` | `Simulation*` | Brain simulation data structure containing neural network information |
| `_ImageProcessorPool` | `ImageProcessorPool*` | Thread pool manager for parallel image processing operations |
| `_GeneratorPool` | `VoxelArrayGenerator::ArrayGeneratorPool*` | Resource pool for voxel array generation tasks |

#### Returns
- `true`: Rendering operation completed successfully
- `false`: Operation failed due to insufficient resources or invalid parameters

#### Process Flow

The function implements a three-phase rendering pipeline:

### Phase 0: Memory Assessment and Allocation

**Memory Calculation Algorithm:**
```cpp
uint64_t MaxVoxels = uint64_t(double(getTotalSystemMemory()) * ScalingFactor) / sizeof(VoxelType);
size_t MaxVoxelArraySizeOnAxisInRAM = std::cbrt(MaxVoxels);
```

**Operations:**
1. **System Memory Detection**: Queries available RAM using system calls
2. **Safety Factor Application**: Applies configurable percentage limit (typically 60-80% of total RAM)
3. **Voxel Array Sizing**: Calculates maximum cubic voxel array dimensions that fit in memory
4. **Resource Reservation**: Reserves calculated memory space for rendering operations

### Phase 1: Spatial Subdivision Planning

**Image Step Size Calculation:**
```cpp
double ImageStepSizeX_um = (Params->ImageWidth_px / Params->NumPixelsPerVoxel_px) * 
                          Params->VoxelResolution_um * 
                          (1 - (double(Params->ScanRegionOverlap_percent) / 100.));
```

**Subregion Dimension Calculation:**
```cpp
int NumSubRegionsInXDim = ceil(BaseRegion->SizeX() / SubRegionStepSizeX_um);
int NumSubRegionsInYDim = ceil(BaseRegion->SizeY() / SubRegionStepSizeY_um);
int NumSubRegionsInZDim = ceil(BaseRegion->SizeZ() / SubRegionStepSizeZ_um);
```

**Operations:**
1. **Image Grid Planning**: Calculates optimal image positioning and overlap parameters
2. **Subregion Sizing**: Determines maximum subregion dimensions based on memory constraints
3. **Overlap Management**: Configures boundary overlaps to prevent rendering artifacts
4. **Resource Estimation**: Validates that planned operations fit within memory limits

### Phase 2: Subregion Generation and Coordinate Mapping

**Coordinate Calculation:**
```cpp
double SubRegionStartX_um = (XStep * SubRegionStepSizeX_um) + BaseRegionOffsetX_um;
double SubRegionEndX_um = std::min(
    BaseRegionOffsetX_um + ((XStep + 1) * SubRegionStepSizeX_um) + SubRegionOverlapX_um,
    (double)BaseRegion->Point2X_um
);
```

**Operations:**
1. **Coordinate System Mapping**: Transforms global coordinates to local subregion coordinates
2. **Boundary Enforcement**: Ensures subregions remain within specified brain region limits
3. **Overlap Integration**: Applies calculated overlap regions between adjacent subregions
4. **Data Structure Creation**: Instantiates SubRegion objects with complete spatial information

### Phase 3: Parallel Rendering Execution

**Rendering Loop:**
```cpp
for (size_t i = 0; i < SubRegions.size(); i++) {
    EMRenderSubRegion(_Logger, &SubRegions[i], _ImageProcessorPool, _GeneratorPool);
    _Simulation->VSDAData_.CurrentRegion_ = i + 1;
}
```

**Operations:**
1. **Sequential Processing**: Processes each subregion through the rendering pipeline
2. **Progress Tracking**: Updates completion status for user interface monitoring
3. **Resource Management**: Coordinates memory usage across parallel operations
4. **Memory Cleanup**: Releases allocated resources upon completion

## Configuration Parameters

### Memory Management

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `MaxVoxelArraySize_` | `size_t` | System-dependent | Hard limit on voxel array dimensions |
| `VoxelArrayPercentOfSystemMemory_` | `double` | 60.0 | Percentage of system RAM available for rendering |

### Image Generation

| Parameter | Type | Units | Description |
|-----------|------|-------|-------------|
| `ImageWidth_px` | `int` | pixels | Width of generated microscopy images |
| `ImageHeight_px` | `int` | pixels | Height of generated microscopy images |
| `VoxelResolution_um` | `double` | micrometers | Spatial resolution per voxel |
| `ScanRegionOverlap_percent` | `double` | percentage | Overlap between adjacent image regions |
| `SliceThickness_um` | `double` | micrometers | Thickness of each image slice |
| `NumPixelsPerVoxel_px` | `double` | pixels/voxel | Sampling rate for voxel-to-pixel conversion |

## Error Handling

### Runtime Errors

#### Insufficient Memory Error
```
Error, You Don't Have Enough Memory To Render Images At [width] by [height] Try Reducing The Image Resolution
```

**Cause**: Requested image resolution exceeds available system memory  
**Resolution**: Reduce `ImageWidth_px` and `ImageHeight_px` parameters or increase available system RAM

#### Memory Allocation Waiting
```
Waiting for enough free RAM to become available before starting render
```

**Cause**: Other system processes are consuming memory, preventing render operations  
**Resolution**: Wait for automatic resolution or terminate memory-intensive processes

#### Render Abortion
```
Render Aborted
```

**Cause**: Critical error preventing continuation of rendering operations  
**Resolution**: Check system logs for underlying cause; verify simulation data integrity

### Memory Safety Features

#### Resource Monitoring
- **Continuous RAM tracking**: Real-time monitoring of memory consumption
- **Allocation queuing**: Automatic waiting when memory usage exceeds safe thresholds (90% of system RAM)
- **Cleanup protocols**: Automatic memory deallocation upon operation completion

#### Boundary Validation
- **Region limit enforcement**: Prevents subregions from exceeding defined brain region boundaries
- **Coordinate validation**: Ensures all spatial calculations remain within valid ranges
- **Overlap verification**: Validates that overlap regions maintain spatial continuity

## Performance Optimization

### Memory Efficiency
- **Adaptive subdivision**: Dynamic calculation of optimal subregion sizes based on available resources
- **Lazy loading**: Memory allocation occurs only when needed for active rendering operations
- **Garbage collection**: Immediate cleanup of completed subregion data

### Computational Optimization
- **Parallel processing**: Utilizes multiple CPU cores through thread pool management
- **Cache optimization**: Spatial locality optimization in subregion processing order
- **Resource pooling**: Reuses computational resources across multiple rendering operations

### Monitoring and Diagnostics

#### Progress Tracking
The module provides real-time progress information through the following metrics:

| Metric | Description | Access Method |
|--------|-------------|---------------|
| `CurrentOperation_` | Current phase of rendering operation | `_Simulation->VSDAData_.CurrentOperation_` |
| `CurrentRegion_` | Index of currently processing subregion | `_Simulation->VSDAData_.CurrentRegion_` |
| `TotalRegions_` | Total number of subregions to process | `_Simulation->VSDAData_.TotalRegions_` |
| `TotalImagesX_`, `TotalImagesY_` | Total images in each dimension | `_Simulation->VSDAData_.TotalImagesX_` |

#### Logging Levels
The module uses a hierarchical logging system with the following levels:

| Level | Purpose | Typical Messages |
|-------|---------|------------------|
| 1 | Critical warnings | Memory shortage notifications |
| 3 | General information | Subregion calculations, memory usage |
| 4 | Detailed operations | Image counts, step size calculations |
| 8-9 | Error conditions | Memory errors, operation failures |

## Dependencies

### System Requirements
- **Operating System**: Linux, Windows, or macOS with POSIX compliance
- **Memory**: Minimum 8GB RAM (16GB+ recommended for large datasets)
- **CPU**: Multi-core processor with 64-bit architecture

### External Libraries
- **nlohmann/json**: JSON parsing and configuration management
- **System libraries**: Memory detection and threading support

### Internal Modules
- **VoxelArrayGenerator**: 3D voxel data structure management
- **ImageProcessorPool**: Parallel image processing coordination
- **Logging System**: Centralized logging and debugging infrastructure
- **Configuration System**: Parameter management and validation

## Usage Examples

### Basic Rendering Operation
```cpp
#include <VSDA/EM/EMRenderer.h>

// Initialize required components
Config::Config config;
BG::Common::Logger::LoggingSystem logger;
Simulation simulation;
ImageProcessorPool imagePool;
VoxelArrayGenerator::ArrayGeneratorPool generatorPool;

// Execute rendering
bool success = ExecuteSubRenderOperations(
    &config, 
    &logger, 
    &simulation, 
    &imagePool, 
    &generatorPool
);

if (success) {
    // Process completed successfully
    logger.Log("Rendering completed successfully", 3);
} else {
    // Handle error condition
    logger.Log("Rendering failed - check system resources", 8);
}
```

### Memory Configuration
```cpp
// Configure memory usage parameters
config.MaxVoxelArraySize_ = 1024;  // Maximum voxel array dimension
config.VoxelArrayPercentOfSystemMemory_ = 75.0;  // Use 75% of system RAM

// Set image parameters
simulation.VSDAData_.Params_.ImageWidth_px = 2048;
simulation.VSDAData_.Params_.ImageHeight_px = 2048;
simulation.VSDAData_.Params_.VoxelResolution_um = 0.1;
simulation.VSDAData_.Params_.ScanRegionOverlap_percent = 10.0;
```

## Best Practices

### Memory Management
1. **Monitor system resources** before initiating large rendering operations
2. **Configure appropriate safety margins** (60-80% of total RAM) to maintain system stability
3. **Close unnecessary applications** to maximize available memory for rendering

### Performance Optimization
1. **Use power-of-two image dimensions** when possible for optimal memory alignment
2. **Balance overlap percentage** between quality (higher overlap) and performance (lower overlap)
3. **Monitor CPU usage** to ensure optimal thread pool utilization

### Error Prevention
1. **Validate simulation data** before initiating rendering operations
2. **Test with smaller regions** before processing full-scale datasets
3. **Implement proper exception handling** in calling code

---

## **Common Questions and Answers**

**Q: Why not just process the whole brain region at once?**
A: Large brain simulations can require hundreds of gigabytes of memory. Breaking into subregions allows processing on normal computers.

**Q: Why the complex overlap calculations?**
A: Without proper overlap, you'd see visible seams between subregions in the final images, like a poorly made panoramic photo.

**Q: Why wait for memory instead of failing immediately?**
A: Multiple render jobs might be running. Waiting allows them to complete and free memory rather than failing unnecessarily.

**Q: What happens if there's a bug in the coordinate calculations?**
A: You could get gaps in coverage, overlapping artifacts, or images that don't align properly when stitched together.

---

*This documentation is maintained as part of the BrainGenix Neural Emulation System. For technical support or feature requests, please consult the project repository.*
