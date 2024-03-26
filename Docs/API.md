# NES API Documentation

This document outlines the standard that is used to define the NES POST API.

We use JSON requests, so the route and example requests are listed below:


### Simulation - Create
 - Name: `Simulation/Create`  
 - Query: 
```json
    [
        Name: str
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
        SimulationID: int
    ]
```

### Simulation - Reset
 - Name: `Simulation/Reset`  
 - Query: 
```json
    [
        SimulationID: int
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE
    ]
```

### Simulation - RunFor
 - Name: `Simulation/RunFor`  
 - Query: 
```json
    [
        SimulationID: int,
        Runtime_ms: float
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE
    ]
```

### Simulation - RecordAll
 - Name: `Simulation/RecordAll`  
 - Query: 
```json
    [
        SimulationID: int,
        MaxRecordTime_ms: float
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE
    ]
```

### Simulation - GetRecording
 - Name: `Simulation/GetRecording`  
 - Query: 
```json
    [
        SimulationID: int
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
        Recording: json
    ]
```


### Simulation - GetStatus
 - Name: `Simulation/GetStatus`  
 - Query: 
```json
    [
        SimulationID: int
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
        IsSimulating: bool,
        RealWorldTimeRemaining_ms: float,
        RealWorldTimeElapsed_ms: float,
        InSimulationTime_ms: float,
        InSimulationTimeRemaining: float,
        PercentComplete: float
    ]
```


### Simulation - Save
 - Name: `Simulation/Save`  
 - Query: 
```json
    [
        SimulationID: int
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
    ]
```

### Simulation - Get Save
 - Name: `Simulation/GetSave`  
 - Query: 
```json
    [
        SaveHandle: `str`
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
        SaveData: Base64EncodedString
    ]
```

### Simulation - Load
 - Name: `Simulation/Load`  
 - Query: 
```json
    [
        SavedSimName: `str`
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
        TaskID: int
    ]
```


### Simulation - Geometry - Sphere - Create
 - Name: `Simulation/Geometry/Sphere/Create`  
 - Query: 
```json
    [
        SimulationID: int,
        Radius_um: float,
        CenterPosX_um: float,
        CenterPosY_um: float,
        CenterPosZ_um: float,
        Name: str
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
        ShapeID: int
    ]
```

### Simulation - Geometry - Cylinder - Create
 - Name: `Simulation/Geometry/Cylinder/Create`  
 - Query: 
```json
    [
        SimulationID: int,
        Point1Radius_um: float,
        Point1PosX_um: float,
        Point1PosY_um: float,
        Point1PosZ_um: float,
        Point2Radius_um: float,
        Point2PosX_um: float,
        Point2PosY_um: float,
        Point2PosZ_um: float,
        Name: str
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
        ShapeID: int
    ]
```

### Simulation - Geometry - Box - Create
 - Name: `Simulation/Geometry/Box/Create`  
 - Query: 
```json
    [
        SimulationID: int,
        CenterPosX_um: float,
        CenterPosY_um: float,
        CenterPosZ_um: float,
        ScaleX_um: float,
        ScaleY_um: float,
        ScaleZ_um: float,
        RotationX_um: float,
        RotationY_um: float,
        RotationZ_um: float,
        Name: str
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
        ShapeID: int
    ]
```

### Simulation - Compartments - BS - Create
 - Name: `Simulation/Compartments/BS/Create`  
 - Query: 
```json
    [
        SimulationID: int,
        ShapeID: int,
        MembranePotential_mV: float,
        SpikeThreshold_mV: float,
        DecayTime_ms: float,
        RestingPotential_mV: float,
        AfterHyperpolarizationAmplitude_mV: float,
        Name: str
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
        CompartmentID: int
    ]
```

### Simulation - Staple - Create
 - Name: `Simulation/Staple/Create`  
 - Query: 
```json
    [
        SimulationID: int,
        SourceCompartmentID: int,
        DestinationCompartmentID: int,
        Name: str
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
        StapleID: int
    ]
```

### Simulation - Receptor - Create
 - Name: `Simulation/Receptor/Create`  
 - Query: 
```json
    [
        SimulationID: int,
        SourceCompartmentID: int,
        DestinationCompartmentID: int,
        Conductance_nS: float,
        TimeConstantRise_ms: float,
        TimeConstantDecay_ms: float,
        ReceptorPosX_um: float,
        ReceptorPosY_um: float,
        ReceptorPosZ_um: float,
        Name: str
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
        ReceptorID: int
    ]
```

### Simulation - Neuron - BS - Create
 - Name: `Simulation/Neuron/BS/Create`  
 - Query: 
```json
    [
        SimulationID: int,
        SomaID: int,
        AxonID: int,
        MembranePotential_mV: float,
        RestingPotential_mV: float,
        SpikeThreshold_mV: float,
        DecayTime_ms: float,
        AfterHyperpolarizationAmplitude_mV: float,
        PostsynapticPotentialRiseTime_ms: float,
        PostsynapticPotentialDecayTime_ms: float,
        PostsynapticPotentialAmplitude_nA: float,
        Name: str
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
        NeuronID: int
    ]
```
### Simulation - Neuron - SC - Create
 - Name: `Simulation/Neuron/SC/Create`  
 - Query: 
```json
    [
        SimulationID: int,
        SomaIDs: list,
        AxonIDs: list,
        DendriteIDs: list,
        MembranePotential_mV: float,
        RestingPotential_mV: float,
        SpikeThreshold_mV: float,
        DecayTime_ms: float,
        AfterHyperpolarizationAmplitude_mV: float,
        PostsynapticPotentialRiseTime_ms: float,
        PostsynapticPotentialDecayTime_ms: float,
        PostsynapticPotentialAmplitude_nA: float,
        Name: str
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
        NeuronID: int
    ]
```

### Simulation - PatchClampDAC - Create
 - Name: `Simulation/PatchClampDAC/Create`  
 - Query: 
```json
    [
        SimulationID: int,
        DestinationCompartmentID: int,
        ClampPosX_um: float,
        ClampPosY_um: float,
        ClampPosZ_um: float,
        Name: str
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
        PatchClampDACID: int
    ]
```

### Simulation - PatchClampDAC - SetOutputList
 - Name: `Simulation/PatchClampDAC/SetOutputList`  
 - Query: 
```json
    [
        "SimulationID": int,
        "PatchClampDACID": int,
        "ControlData": [
        [ <t_ms> (float), <v_mV> (float) ],
        ...
        (more pairs)
        ]
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
    ]
```


### Simulation - PatchClampADC - Create
 - Name: `Simulation/PatchClampADC/Create`  
 - Query: 
```json
    [
        SimulationID: int,
        SourceCompartmentID: int,
        ClampPosX_um: float,
        ClampPosY_um: float,
        ClampPosZ_um: float,
        Name: str
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
        PatchClampADCID: int
    ]
```



### Simulation - PatchClampADC - SetSampleRate
 - Name: `Simulation/PatchClampADC/Create`  
 - Query: 
```json
    [
        SimulationID: int,
        PatchClampADCID: int,
        Timestep_ms: float
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
    ]
```

### Simulation - PatchClampADC - GetRecordedData
 - Name: `Simulation/PatchClampADC/GetRecordedData`  
 - Query: 
```json
    [
        SimulationID: int,
        PatchClampADCID: int,
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
        Timestep_ms: float,
        RecordedData_mV: list<float>
    ]
```

### Simulation - SetSpecificAPTimes
 - Name: `Simulation/PatchClampADC`  
 - Query: 
```json
    [
        "SimulationID": <SimID>,
        "TimeNeuronPairs": [
            [ <t_ms>, <neuron-id> ],
            (more pairs)
        ]
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
    ]
```

### Simulation - SetSpontaneousActivity
 - Name: `Simulation/SetSpontaneousActivity`  
 - Query: 
```json
    [
        "SimulationID": <SimID>,
        "SpikeIntervalMean_ms": <float>,
        "SpikeIntervalStDev_ms": <float>,
        "NeuronIDs": [
            (neuron-id,)
        ]
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
    ]
```


### Visualizer - GetStatus
 - Name: `Visualizer/GetStatus`  
 - Query: 
```json
    [
        SimulationID: int,
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
        VisualizerStatus: ENUM_VISUALIZER_STATUS
    ]
```

### Visualizer - GetImageHandles
 - Name: `Visualizer/GetImageHandles`  
 - Query: 
```json
    [
        SimulationID: int,
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
        ImageHandles: list<string>
    ]
```

### Visualizer - GetImage
 - Name: `Visualizer/GetImage`  
 - Query: 
```json
    [
        SimulationID: int,
        ImageHandle: string
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
        ImageData: base64 encoded string of binary image data
    ]
```

### Visualizer - GenerateImages
 - Name: `Visualizer/GenerateImages`  
 - Query: 
```json
    [
        "SimulationID": <SimID>,
        "Locations": [
          {
               "CameraPositionX_um": float,
               "CameraPositionY_um": float,
               "CameraPositionz_um": float,
               "CameraLookAtPositionX_um": float,
               "CameraLookAtPositionY_um": float,
               "CameraLookAtPositionz_um": float,
               "CameraFOV_deg": float
           },
           ...
           ]
        "ImageWidth_px": unsigned int,
        "ImageHeight_px": unsigned int
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
    ]
```


### VSDA - EM - Initialize
 - Name: `VSDA/EM/Initialize`  
 - Query: 
```json
    [
        SimulationID: int
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
    ]
```

### VSDA - EM - SetupMicroscope
 - Name: `VSDA/EM/SetupMicroscope`  
 - Query: 
```json
    [
        SimulationID: int,
        PixelResolution_nm: float,
        ImageWidth_px: int,
        ImageHeight_px: int,
        SliceThickness_nm: float,
        ScanRegionOverlap_percent: float,
        MicroscopeFOV_deg: int,
        NumPixelsPerVoxel_px: int
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
    ]
```

### VSDA - EM - DefineScanRegion
 - Name: `VSDA/EM/DefineScanRegion`  
 - Query: 
```json
    [
        SimulationID: int,
        Point1X_um: float,
        Point1Y_um: float,
        Point1Z_um: float,
        Point2X_um: float,
        Point2Y_um: float,
        Point2Z_um: float
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
        ScanRegionID: int
    ]
```

### VSDA - EM - QueueRenderOperation
 - Name: `VSDA/EM/QueueRenderOperation`  
 - Query: 
```json
    [
        SimulationID: int,
        ScanRegionID: int
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
    ]
```

### VSDA - EM - VSDAEMGetRenderStatus
 - Name: `VSDA/EM/VSDAEMGetRenderStatus`  
 - Query: 
```json
    [
        SimulationID: int
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
        RenderStatus: ENUM_VSDA_EM_STATUS_CODE,
        CurrentSlice: int,
        TotalSlices: int,
        CurrentSliceImage: int,
        TotalSliceImages: int,
        CurrentRegion: int,
        TotalRegions: int,
    ]
```

### VSDA - EM - VSDAEMGetImageStack
 - Name: `VSDA/EM/VSDAEMGetImageStack`  
 - Query: 
```json
    [
        SimulationID: int,
        ScanRegionID: int
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
        RenderedImages: list<string>
    ]
```

### VSDA - GetImage
 - Name: `VSDA/GetImage`  
 - Query: 
```json
    [
        SimulationID: int,
        ImageHandle: str
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
        ImageData: base64 encoded binary string of image data
    ]
```





### VSDA - Ca - Initialize
 - Name: `VSDA/Ca/Initialize`  
 - Query: 
```json
    [
        "SimulationID": int
    ]
```
 - Response:
```json
    [
        "StatusCode": ENUM_STATUS_CODE
    ]
```

### VSDA - Ca - SetupMicroscope
 - Name: `VSDA/Ca/SetupMicroscope`  
 - Query: 
```json
    [
        SimulationID: int,
        VoxelResolution_nm: float,
        ImageWidth_px: int,
        ImageHeight_px: int,
        NumVoxelsPerSlice: int,
        ScanRegionOverlap_percent: float,
        NumPixelsPerVoxel_px: int,
        "FlourescingNeuronIDs": list<NeuronID:int>,
        "CalciumIndicator": string,
        "IndicatorRiseTime_ms": float,
        "IndicatorDecayTime_ms": float,
        "IndicatorInterval_ms": float
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
    ]
```

### VSDA - Ca - DefineScanRegion
 - Name: `VSDA/Ca/DefineScanRegion`  
 - Query: 
```json
    [
        SimulationID: int,
        Point1X_um: float,
        Point1Y_um: float,
        Point1Z_um: float,
        Point2X_um: float,
        Point2Y_um: float,
        Point2Z_um: float
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
        ScanRegionID: int
    ]
```

### VSDA - Ca - QueueRenderOperation
 - Name: `VSDA/Ca/QueueRenderOperation`  
 - Query: 
```json
    [
        SimulationID: int,
        ScanRegionID: int
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
    ]
```

### VSDA - Ca - GetRenderStatus
 - Name: `VSDA/Ca/GetRenderStatus`  
 - Query: 
```json
    [
        SimulationID: int
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
        RenderStatus: ENUM_VSDA_CA_STATUS_CODE,
        CurrentSlice: int,
        TotalSlices: int,
        CurrentSliceImage: int,
        TotalSliceImages: int,
        CurrentRegion: int,
        TotalRegions: int,
    ]
```

### VSDA - Ca - GetImageStack
 - Name: `VSDA/Ca/GetImageStack`  
 - Query: 
```json
    [
        SimulationID: int,
        ScanRegionID: int
    ]
```
 - Response:
```json
    [
        StatusCode: ENUM_STATUS_CODE,
        RenderedImages: list<string>
    ]
```






Todo: Add other routes here.

