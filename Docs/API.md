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
        RealWorldTimeElapsed:ms: float,
        InSimulationTime_ms:float,
        InSimulationTimeRemaining:float,
        PercentComplete:float
    ]
```