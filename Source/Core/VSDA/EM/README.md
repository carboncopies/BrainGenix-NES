# EMRenderer Module Documentation

## Overview

The EMRenderer module converts 3D brain tissue simulations into electron microscopy (EM) images. It handles large datasets by automatically dividing them into memory-efficient subregions and processing them in parallel.

## Purpose

- **Memory Management**: Automatically subdivides large brain regions to fit available memory
- **Image Generation**: Creates realistic electron microscopy simulation images
- **Parallel Processing**: Coordinates multiple rendering tasks across system resources

## Architecture

The module uses a three-component pipeline:

1. **Voxel System**: Converts neural data into 3D discrete units (voxels)
2. **Subregion Management**: Divides large regions into manageable pieces with overlapping boundaries
3. **Image Generation**: Produces 2D image sequences that reconstruct the full 3D volume

## Core Functions

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

**Purpose**: Orchestrates the complete rendering pipeline for electron microscopy simulation.

**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `_Config` | `Config::Config*` | System configuration containing memory limits and rendering parameters |
| `_Logger` | `BG::Common::Logger::LoggingSystem*` | Logging interface for operation tracking and debugging |
| `_Simulation` | `Simulation*` | Brain simulation data structure containing neural network information |
| `_ImageProcessorPool` | `ImageProcessorPool*` | Thread pool manager for parallel image processing operations |
| `_GeneratorPool` | `VoxelArrayGenerator::ArrayGeneratorPool*` | Resource pool for voxel array generation tasks |

**Process**:
1. **Memory Calculation**: Determines maximum voxel array size that fits in available RAM
2. **Spatial Subdivision**: Divides large regions into memory-efficient subregions with overlap
3. **Parallel Execution**: Processes each subregion using `EMRenderSubRegion`

---

### EMRenderSubRegion

```cpp
bool EMRenderSubRegion(
    BG::Common::Logger::LoggingSystem* _Logger, 
    SubRegion* _SubRegion,
    ImageProcessorPool* _ImageProcessorPool,
    VoxelArrayGenerator::ArrayGeneratorPool* _GeneratorPool
);
```

**Purpose**: Processes a single brain tissue subregion into EM image slices.

**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `_Logger` | `BG::Common::Logger::LoggingSystem*` | Logging system for debugging and progress tracking |
| `_SubRegion` | `SubRegion*` | Subregion definition with spatial boundaries and offsets |
| `_ImageProcessorPool` | `ImageProcessorPool*` | Thread pool for parallel image processing |
| `_GeneratorPool` | `VoxelArrayGenerator::ArrayGeneratorPool*` | Thread pool for voxel array generation |

**Process**:
1. **Voxel Array Management**: Creates or reuses voxel arrays with memory optimization
2. **Neural Data Conversion**: Calls `CreateVoxelArrayFromSimulation` to rasterize neural data
3. **Slice Processing**: Divides 3D volume into 2D slices based on thickness parameters
4. **Image Generation**: Uses `RenderSliceFromArray` to create EM images

---

### CreateVoxelArrayFromSimulation

```cpp
bool CreateVoxelArrayFromSimulation(
    BG::Common::Logger::LoggingSystem* _Logger, 
    Simulation* _Sim, 
    MicroscopeParameters* _Params, 
    VoxelArray* _Array, 
    ScanRegion _Region, 
    VoxelArrayGenerator::ArrayGeneratorPool* _GeneratorPool
);
```

**Purpose**: Converts neural simulation data into 3D voxel array representation.

**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `_Logger` | `BG::Common::Logger::LoggingSystem*` | Logging system for progress tracking |
| `_Sim` | `Simulation*` | Neural simulation containing geometry and connectivity data |
| `_Params` | `MicroscopeParameters*` | Microscope settings including resolution and rendering parameters |
| `_Array` | `VoxelArray*` | Target voxel array to populate |
| `_Region` | `ScanRegion` | 3D region boundaries and transformation parameters |
| `_GeneratorPool` | `VoxelArrayGenerator::ArrayGeneratorPool*` | Thread pool for parallel voxel generation |

**Process**:
1. **Spatial Culling**: Filters neural structures to only those within the target region
2. **Geometry Processing**: Handles spheres (cell bodies), cylinders (axons/dendrites), and boxes (receptors)
3. **Shape Subdivision**: Breaks large structures into smaller pieces for memory efficiency
4. **Parallel Rasterization**: Converts geometry to voxels using thread pool
5. **Optional Artifacts**: Adds realistic tissue tears if enabled

**Key Features**:
- **Memory Optimization**: Subdivides large shapes (>75,000 voxels) to prevent memory issues
- **Spatial Efficiency**: Eliminates 60-90% of processing through spatial culling
- **Parallel Processing**: Each shape becomes an independent task for thread pool

---

### RenderSliceFromArray

```cpp
int RenderSliceFromArray(
    BG::Common::Logger::LoggingSystem* _Logger, 
    int MaxImagesX, 
    int MaxImagesY, 
    VSDAData* _VSDAData, 
    VoxelArray* _Array, 
    std::string _FilePrefix, 
    int _SliceNumber, 
    int _SliceThickness, 
    ImageProcessorPool* _ImageProcessorPool, 
    double _OffsetX, 
    double _OffsetY, 
    double _RegionOffsetX, 
    double _RegionOffsetY, 
    int _SliceOffset, 
    noise::module::Perlin* _Generator
);
```

**Purpose**: Generates 2D EM images from a specific Z-slice of a 3D voxel array.

**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `_Logger` | `BG::Common::Logger::LoggingSystem*` | Logging system for error tracking |
| `MaxImagesX/Y` | `int` | Maximum number of images in each dimension |
| `_VSDAData` | `VSDAData*` | Simulation data and rendering parameters |
| `_Array` | `VoxelArray*` | Source voxel array containing 3D data |
| `_FilePrefix` | `std::string` | Base path for output image files |
| `_SliceNumber` | `int` | Z-slice index to render |
| `_SliceThickness` | `int` | Number of voxel layers to integrate per slice |
| `_ImageProcessorPool` | `ImageProcessorPool*` | Thread pool for parallel image processing |
| `_OffsetX/Y` | `double` | Local coordinate offsets for alignment |
| `_RegionOffsetX/Y` | `double` | Global coordinate offsets for region alignment |
| `_SliceOffset` | `int` | Z-offset for slice numbering |
| `_Generator` | `noise::module::Perlin*` | Noise generator for realistic EM artifacts |

**Process**:
1. **Camera Grid Setup**: Calculates optimal camera positions for complete slice coverage
2. **Voxel Sampling**: Extracts pixel data from voxel array with configurable oversampling
3. **Noise Application**: Adds realistic electron microscopy imaging artifacts
4. **Image Processing**: Applies contrast enhancement and gamma correction
5. **Parallel Output**: Creates image processing tasks for thread pool execution

**Key Features**:
- **Configurable Overlap**: Supports seamless image stitching between adjacent images
- **Anti-aliasing**: Multiple pixels per voxel for smooth image quality
- **Realistic Artifacts**: Perlin noise simulation of electron microscopy imaging effects

## Configuration Parameters

### Memory Settings
- **Memory Scaling Factor**: Percentage of total RAM to use (typically 60-80%)
- **Subdivision Threshold**: Maximum voxels per shape (default: 75,000)

### Image Quality
- **Voxel Resolution**: Physical size per voxel in micrometers
- **Pixels Per Voxel**: Oversampling rate for anti-aliasing
- **Slice Thickness**: Physical thickness of each image slice
- **Overlap Percentage**: Image overlap for seamless stitching

### Noise Parameters
- **Noise Scale X/Y/Z**: Frequency of noise in each dimension
- **Noise Intensity**: Amplitude of noise effects

## Common Issues and Solutions

**Memory Issues**:
- *Problem*: Voxel array allocation failures
- *Solution*: Reduce subregion size or increase available memory

**Performance Issues**:
- *Problem*: Slow processing
- *Solution*: Optimize thread pool configuration and subdivision thresholds

**Image Quality Issues**:
- *Problem*: Visible seams between images
- *Solution*: Increase overlap percentage and verify coordinate calculations

**Coordinate Issues**:
- *Problem*: Misaligned images
- *Solution*: Verify offset calculations and region boundary definitions

## Best Practices

1. **Monitor Memory Usage**: Track voxel array sizes and system memory consumption
2. **Optimize Thread Pools**: Match pool sizes to CPU capabilities
3. **Validate Coordinates**: Ensure proper alignment between coordinate systems
4. **Test Overlap**: Verify seamless image stitching with sample data
5. **Balance Quality vs Performance**: Adjust resolution and noise parameters appropriately

---

## Frequently Asked Questions (FAQ)

### **Q: What should I do if I get a memory allocation error during rendering?**
**A:**  
- Lower the voxel resolution or reduce the image size in your configuration.
- Increase available system memory or close other memory-intensive applications.
- Adjust the `MaxVoxelArraySize_` parameter to fit within your system’s RAM.

---

### **Q: Why do my output images have visible seams or gaps?**
**A:**  
- Ensure the overlap percentage (`ScanRegionOverlap_percent`) is set correctly.
- Double-check that your coordinate calculations and offsets are consistent between subregions.
- Test with sample data and visualize the stitched output to verify seamless alignment.

---

### **Q: How can I speed up the rendering process?**
**A:**  
- Increase the number of threads in your `ImageProcessorPool` and `ArrayGeneratorPool` to match your CPU’s core count.
- Use a lower resolution or reduce the number of slices if possible.
- Make sure your storage device is fast enough for high-throughput image output.

---

### **Q: What configuration parameters most affect image quality?**
**A:**  
- `VoxelResolution_um`: Smaller values increase detail but require more memory.
- `NumPixelsPerVoxel_px`: Higher values improve anti-aliasing and smoothness.
- `Noise` parameters: Adjust these for more or less realistic EM artifacts.

---

### **Q: How do I debug misaligned or missing images?**
**A:**  
- Enable higher log verbosity in the logger to track subregion and slice boundaries.
- Check the calculated start/end coordinates for each subregion in the logs.
- Verify that all subregions are being processed and output files are generated as expected.

---

### **Q: Can I use this module for non-brain tissue simulations?**
**A:**  
- Yes, as long as your simulation data can be converted into the expected voxel and geometry formats.

---

### **Q: Where can I find more implementation details?**
**A:**  
- Refer to the source code in the respective module files for in-depth documentation and comments.
- See the `VSDA/EM/EMRenderer.cpp` and related headers for algorithm specifics.

---

*This documentation explains the core EMRenderer functionality. For implementation details, refer to the source code in the respective module files.*
