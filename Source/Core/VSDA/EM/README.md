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

### Phase 0: Calculating Maximum Number of Voxels which can fit Inside the Memory Requirement 

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
### Functions in Phase 3

1. **PATH: Source/Core/VSDA/EM/VoxelSubsystem/EMSubRegion.cpp**
```cpp
bool EMRenderSubRegion(BG::Common::Logger::LoggingSystem* _Logger, SubRegion* _SubRegion, ImageProcessorPool* _ImageProcessorPool, VoxelArrayGenerator::ArrayGeneratorPool* _GeneratorPool) 
```

2. **PATH: Source/Core/VSDA/EM/VoxelSubsystem/VoxelArrayGenerator.cpp**
```cpp
bool CreateVoxelArrayFromSimulation(BG::Common::Logger::LoggingSystem* _Logger, Simulation* _Sim, MicroscopeParameters* _Params, VoxelArray* _Array, ScanRegion _Region, VoxelArrayGenerator::ArrayGeneratorPool* _GeneratorPool) 
```

3. **PATH: Source/Core/VSDA/EM/VoxelSubsystem/VoxelArrayRenderer.cpp**
```cpp
int RenderSliceFromArray(BG::Common::Logger::LoggingSystem* _Logger, int MaxImagesX, int MaxImagesY, VSDAData* _VSDAData, VoxelArray* _Array, std::string _FilePrefix, int _SliceNumber, int _SliceThickness, ImageProcessorPool* _ImageProcessorPool, double _OffsetX, double _OffsetY, double _RegionOffsetX, double _RegionOffsetY, int _SliceOffset, noise::module::Perlin* _Generator) 
```

---

## EMSubRegion Module Deep Dive

### Overview
The `EMSubRegion` module handles the detailed processing of individual brain tissue subregions created by the main EMRenderer. It orchestrates the conversion of 3D neural simulation data into electron microscopy images through a sophisticated pipeline involving voxel array management, slice processing, and image generation.

### Architecture

#### Process Flow
```
Subregion Input → Voxel Array Creation → Slice Processing → EM Image Generation
```

The module implements a multi-phase approach:

1. **Voxel Array Management**: Intelligent memory allocation and reuse
2. **Simulation Data Rasterization**: Converting neural data into 3D voxel representation
3. **Slice-Based Processing**: Dividing 3D volume into 2D cross-sections
4. **Image Generation**: Creating realistic electron microscopy images

### Key Components

#### SubRegion Structure
```cpp
struct SubRegion {
    Simulation* Sim;                    // Neural simulation reference
    ScanRegion Region;                  // 3D spatial boundaries
    double RegionOffsetX_um;            // X offset within master region
    double RegionOffsetY_um;            // Y offset within master region
    double MasterRegionOffsetX_um;      // Global X coordinate alignment
    double MasterRegionOffsetY_um;      // Global Y coordinate alignment
    int MaxImagesX;                     // Maximum images in X dimension
    int MaxImagesY;                     // Maximum images in Y dimension
    int LayerOffset;                    // Z-layer offset for slice numbering
};
```

#### Coordinate System Hierarchy
The module manages multiple coordinate systems:
- **Global Coordinates**: Absolute positions in full brain simulation
- **Master Region**: Relative to user-defined scan region
- **Subregion**: Relative to current processing subregion
- **Voxel Array**: Discrete 3D array indices

### Core Functions

#### EMRenderSubRegion
```cpp
bool EMRenderSubRegion(
    BG::Common::Logger::LoggingSystem* _Logger, 
    SubRegion* _SubRegion, 
    ImageProcessorPool* _ImageProcessorPool, 
    VoxelArrayGenerator::ArrayGeneratorPool* _GeneratorPool
);
```

**Primary Function**: Renders a single brain tissue subregion into electron microscopy image stack.

**Detailed Process Flow**:

##### Phase 1: Initialization and Setup
```cpp
// Extract local variables and calculate region parameters
ScanRegion RequestedRegion = _SubRegion->Region;
Simulation* Sim = _SubRegion->Sim;
VSDAData* VSDAData_ = &Sim->VSDAData_;

// Calculate total region thickness for progress tracking
float TotalRegionThickness = abs(RequestedRegion.Point1Z_um - RequestedRegion.Point2Z_um);
VSDAData_->TotalSlices_ = TotalRegionThickness / VSDAData_->Params_.VoxelResolution_um;
```

**Operations**:
- Extracts subregion spatial parameters
- Initializes progress tracking metadata
- Calculates total processing workload

##### Phase 2: Voxel Array Management
```cpp
// Intelligent array allocation with reuse optimization
uint64_t TargetArraySize = RequestedRegion.GetVoxelSize(VSDAData_->Params_.VoxelResolution_um);
if (VSDAData_->Array_.get() == nullptr || VSDAData_->Array_->GetSize() <= TargetArraySize) {
    // Create new array for insufficient existing capacity
    VSDAData_->Array_ = std::make_unique<VoxelArray>(_Logger, RequestedRegion, VSDAData_->Params_.VoxelResolution_um);
} else {
    // Reuse existing array with proper sizing and clearing
    VSDAData_->Array_->SetSize(RequestedRegion, VSDAData_->Params_.VoxelResolution_um);
    VSDAData_->Array_->ClearArrayThreaded(std::thread::hardware_concurrency());
    VSDAData_->Array_->SetBB(RequestedRegion);
}
```

**Memory Optimization Features**:
- **Array Reuse**: Avoids allocation/deallocation overhead
- **Threaded Clearing**: Utilizes multiple CPU cores for memory initialization
- **Size Validation**: Ensures arrays match current processing requirements
- **Smart Pointer Management**: Automatic memory cleanup through `std::unique_ptr`

##### Phase 3: Neural Data Rasterization
```cpp
// Convert simulation data into 3D voxel representation
CreateVoxelArrayFromSimulation(_Logger, Sim, &VSDAData_->Params_, VSDAData_->Array_.get(), RequestedRegion, _GeneratorPool);
```

**Rasterization Process**:
- Converts neural network structures into discrete voxel data
- Applies tissue properties and rendering parameters
- Utilizes thread pool for parallel processing

##### Phase 4: Slice Configuration
```cpp
// Calculate slice parameters based on physical requirements
int NumVoxelsPerSlice = VSDAData_->Params_.SliceThickness_um / VSDAData_->Params_.VoxelResolution_um;
if (NumVoxelsPerSlice < 1) {
    NumVoxelsPerSlice = 1; // Ensure minimum slice thickness
}
int NumZSlices = ceil((float)VSDAData_->Array_.get()->GetZ() / (float)NumVoxelsPerSlice);
```

**Slice Calculation Logic**:
- **Physical Accuracy**: Slice thickness matches real EM parameters
- **Voxel Quantization**: Handles discrete voxel boundaries
- **Boundary Conditions**: Ensures complete volume coverage

##### Phase 5: Resource Synchronization
```cpp
// Wait for image processor pool availability
while (_ImageProcessorPool->GetQueueSize() > 0) {
    VSDAData_->CurrentOperation_ = "Image Processing Enqueued";
    _Logger->Log("Waiting for ImageProcessorPool to become available; '" + 
                std::to_string(_ImageProcessorPool->GetQueueSize()) + "' items remaining", 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}
```

**Synchronization Features**:
- **Queue Monitoring**: Prevents resource conflicts
- **Progress Feedback**: Real-time status updates
- **Non-blocking Design**: Polling with sleep intervals

##### Phase 6: Image Generation Loop
```cpp
// Generate electron microscopy images from voxel data
noise::module::Perlin PerlinGenerator;
for (int i = 0; i < NumZSlices; i++) {
    int CurrentSliceIndex = i * NumVoxelsPerSlice;
    std::string FileNamePrefix = "Simulation" + std::to_string(Sim->ID) + "/Region" + std::to_string(VSDAData_->ActiveRegionID_);
    
    VSDAData_->TotalSlices_ += RenderSliceFromArray(_Logger, _SubRegion->MaxImagesX, _SubRegion->MaxImagesY, 
                                                   &Sim->VSDAData_, VSDAData_->Array_.get(), FileNamePrefix, 
                                                   CurrentSliceIndex, NumVoxelsPerSlice, _ImageProcessorPool, 
                                                   XOffset, YOffset, _SubRegion->MasterRegionOffsetX_um, 
                                                   _SubRegion->MasterRegionOffsetY_um, SliceOffset, &PerlinGenerator);
}
```

**Image Generation Features**:
- **Perlin Noise Integration**: Adds realistic EM imaging artifacts
- **Hierarchical File Organization**: Structured output directory system
- **Coordinate Transformation**: Multiple offset parameters for spatial alignment
- **Progress Accumulation**: Tracks total slice generation for monitoring

##### Phase 7: Completion and Cleanup
```cpp
// Ensure all processing tasks complete
while (_ImageProcessorPool->GetQueueSize() > 0) {
    VSDAData_->CurrentSlice_ = VSDAData_->TotalSlices_ - _ImageProcessorPool->GetQueueSize();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

// Wait for all individual tasks to finish
for (unsigned int i = 0; i < VSDAData_->Tasks_.size(); i++) {
    ProcessingTask* Task = VSDAData_->Tasks_[i].get();
    while (Task->IsDone_ != true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}
```

**Cleanup Operations**:
- **Queue Completion**: Ensures all image processing finishes
- **Task Synchronization**: Waits for individual task completion
- **Progress Finalization**: Updates completion status

### Memory Management Strategies

#### Voxel Array Lifecycle
The module implements sophisticated memory management:

**Array Reuse Pattern**:
```cpp
// Decision logic for array allocation vs reuse
if (VSDAData_->Array_.get() == nullptr || VSDAData_->Array_->GetSize() <= TargetArraySize) {
    // Allocate new array
} else {
    // Reuse existing array with clearing
}
```

**Benefits**:
- **Reduced Allocation Overhead**: Minimizes expensive memory operations
- **Memory Fragmentation Prevention**: Reuses large contiguous blocks
- **Performance Optimization**: Eliminates allocation/deallocation cycles

#### Threading and Parallelization

**Multi-level Parallelism**:
1. **Array Clearing**: `ClearArrayThreaded(std::thread::hardware_concurrency())`
2. **Voxel Generation**: Utilizes `VoxelArrayGenerator::ArrayGeneratorPool`
3. **Image Processing**: Coordinates with `ImageProcessorPool`

**Resource Coordination**:
- **Thread Pool Management**: Coordinates multiple processing pools
- **Queue Monitoring**: Prevents resource conflicts
- **Progress Synchronization**: Tracks completion across parallel operations

### Error Handling and Recovery

#### Critical Error Management
```cpp
bool Status = VSDAData_->Array_->SetSize(RequestedRegion, VSDAData_->Params_.VoxelResolution_um);
if (!Status) {
    _Logger->Log("Critical Internal Error, Failed to Set Size Of Voxel Array! This Should NEVER HAPPEN", 10);
    exit(999);
}
```

**Error Handling Strategy**:
- **Fail-Fast Approach**: Critical errors cause immediate termination
- **Comprehensive Logging**: Detailed error messages for debugging
- **State Validation**: Checks for impossible conditions

#### Logging Hierarchy
| Level | Purpose | Usage |
|-------|---------|-------|
| 1 | Warnings | Resource waiting notifications |
| 2 | General Info | Array allocation decisions |
| 4 | Detailed Operations | Subregion processing details |
| 5 | Debug Information | Slice calculation specifics |
| 10 | Critical Errors | Fatal system failures |

### Performance Considerations

#### Memory Efficiency
- **Array Reuse**: Reduces allocation overhead
- **Threaded Operations**: Parallel memory initialization
- **Smart Pointers**: Automatic memory management

#### CPU Utilization
- **Hardware Concurrency**: Uses `std::thread::hardware_concurrency()`
- **Thread Pool Coordination**: Manages multiple processing pools
- **Non-blocking Synchronization**: Prevents CPU waste during waits

#### I/O Optimization
- **Hierarchical File Structure**: Organized output directory system
- **Batch Processing**: Efficient slice-by-slice processing
- **Progress Buffering**: Minimizes status update overhead

### Integration with Pipeline

#### Upstream Dependencies
- **ExecuteSubRenderOperations**: Provides subregion specifications
- **VoxelArrayGenerator**: Handles neural data conversion
- **ImageProcessorPool**: Manages parallel image processing

#### Downstream Products
- **EM Image Stacks**: Sequential 2D images representing 3D volume
- **Progress Metadata**: Real-time processing status
- **File Organization**: Structured output directory system

### Usage Patterns

#### Typical Processing Flow
1. **Subregion Specification**: Receive spatial boundaries from main renderer
2. **Memory Allocation**: Create or reuse voxel arrays
3. **Data Conversion**: Rasterize neural simulation into voxels
4. **Slice Processing**: Generate 2D cross-sections
5. **Image Creation**: Produce electron microscopy images
6. **Cleanup**: Release resources and update status

#### Resource Management
- **Memory Monitoring**: Track voxel array sizes and usage
- **Thread Coordination**: Manage multiple processing pools
- **Progress Tracking**: Provide real-time status updates

### Best Practices for EMSubRegion

#### Memory Management
1. **Monitor Array Sizes**: Ensure voxel arrays fit available memory
2. **Utilize Reuse Patterns**: Take advantage of array reuse optimization
3. **Thread Safety**: Coordinate access to shared resources

#### Performance Optimization
1. **Thread Pool Configuration**: Optimize pool sizes for system capabilities
2. **Slice Thickness Balance**: Trade-off between quality and processing time
3. **Progress Granularity**: Appropriate feedback frequency

#### Error Prevention
1. **Parameter Validation**: Verify slice thickness and resolution settings
2. **Resource Monitoring**: Watch for memory and queue exhaustion
3. **Graceful Degradation**: Handle resource contention appropriately

### Common Issues and Solutions

#### Memory Issues
- **Problem**: Voxel array allocation failures
- **Solution**: Reduce subregion size or increase available memory
- **Prevention**: Monitor system resources before processing

#### Performance Issues
- **Problem**: Slow slice processing
- **Solution**: Optimize thread pool configuration
- **Prevention**: Balance slice thickness with processing requirements

#### Synchronization Issues
- **Problem**: Queue deadlocks or resource conflicts
- **Solution**: Implement proper waiting mechanisms
- **Prevention**: Monitor pool states and implement timeouts

---
