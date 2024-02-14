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
Broken right now.

### Simulation - Load
Broken right now.



### Geometry - Sphere - Create
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

### Geometry - Cylinder - Create
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

### Geometry - Box - Create
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
