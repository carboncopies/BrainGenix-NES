# GetDetailedConnectome API Documentation

## Overview

The `GetDetailedConnectome` API provides detailed neuronal network information extracted from simulations, specifically designed for researchers who work with neuronal network data rather than EM image stacks. This API exports comprehensive connectivity information including neuron types, 3D positions, and detailed synapse information.

## API Endpoint

**Route:** `Simulation/GetDetailedConnectome`

## Request Format

```json
{
    "SimulationID": <simulation-id>,
    "Sparse": <boolean>,
    "NonZero": <boolean>
}
```

### Parameters

- **SimulationID** (integer): The ID of the simulation to export data from
- **Sparse** (boolean): When `true`, returns only synapses with non-zero conductance. When `false`, returns all synapses
- **NonZero** (boolean): When `true`, filters out synapses with zero conductance values

## Response Format

```json
{
    "StatusCode": 0,
    "Neurons": [
        {
            "ID": <neuron-id>,
            "Type": <neuron-type>,
            "Position": [x, y, z],
            "InputSynapses": [
                {
                    "PresynapticNeuronID": <pre-neuron-id>,
                    "Type": <synapse-type>,
                    "Conductance": <conductance-value>,
                    "Position": [x, y, z]
                }
            ]
        }
    ],
    "Regions": {
        "<region-name>": [<neuron-id>, ...]
    }
}
```

### Response Fields

#### Top Level
- **StatusCode** (integer): 0 for success, non-zero for errors
- **Neurons** (array): Array of neuron objects with detailed information
- **Regions** (object): Dictionary mapping region names to arrays of neuron IDs

#### Neuron Object
- **ID** (integer): Unique identifier for the neuron
- **Type** (integer): Neuron type identifier
- **Position** (array): 3D coordinates [x, y, z] of the neuron soma in micrometers
- **InputSynapses** (array): Array of input synapse objects

#### Synapse Object
- **PresynapticNeuronID** (integer): ID of the presynaptic neuron
- **Type** (string/integer): Synapse type (e.g., "AMPA", "GABA", "NMDA" for BS/SC neurons; integer codes for LIFC neurons)
- **Conductance** (float): Synapse strength in nanoSiemens (nS)
- **Position** (array): 3D coordinates [x, y, z] of the synapse location in micrometers

## Differences from GetAbstractConnectome

| Feature | GetAbstractConnectome | GetDetailedConnectome |
|---------|----------------------|----------------------|
| Neuron positions | ✗ | ✓ |
| Synapse positions | ✗ | ✓ |
| Individual synapse data | ✗ (aggregated counts) | ✓ |
| Synapse types | ✗ | ✓ |
| Conductance values | ✗ (counts only) | ✓ |
| Format | Connection matrix | Hierarchical neuron list |

## Neuron Type Support

The API supports both neuron types used in BrainGenix-NES:

### BS/SC Neurons (Ball-and-Stick/Simple Compartmental)
- Uses individual `ReceptorData` objects
- Synapse types as strings ("AMPA", "GABA", "NMDA")
- Direct conductance values in nanoSiemens

### LIFC Neurons (Leaky Integrate-and-Fire Compartmental)
- Uses abstracted `LIFCReceptorData` objects
- Synapse types as integer enums
- Aggregated conductance as `g_peak_sum_nS`

## Example Usage

### Request
```json
{
    "SimulationID": 0,
    "Sparse": true,
    "NonZero": true
}
```

### Response (Example)
```json
{
    "StatusCode": 0,
    "Neurons": [
        {
            "ID": 0,
            "Type": 1,
            "Position": [10.5, 20.3, 15.7],
            "InputSynapses": [
                {
                    "PresynapticNeuronID": 1,
                    "Type": "AMPA",
                    "Conductance": 2.5,
                    "Position": [10.2, 20.1, 15.9]
                },
                {
                    "PresynapticNeuronID": 2,
                    "Type": "GABA",
                    "Conductance": 1.2,
                    "Position": [10.8, 20.5, 15.5]
                }
            ]
        },
        {
            "ID": 1,
            "Type": 2,
            "Position": [25.1, 18.9, 22.3],
            "InputSynapses": []
        }
    ],
    "Regions": {
        "Cortex": [0, 1],
        "Hippocampus": []
    }
}
```

## Error Handling

The API returns standard BrainGenix-NES error codes:
- **StatusCode 0**: Success
- **StatusCode 1**: General error
- **StatusCode 2**: Invalid parameters
- **StatusCode 3**: Simulation not found

## Implementation Notes

- Synapse positions are derived from the center coordinates of the associated geometry objects
- For LIFC neurons, the position represents the first receptor in an abstracted functional group
- The API uses const-correct access to geometry data for read-only operations
- Both sparse and dense output formats are supported through the `Sparse` parameter

## Performance Considerations

- For large networks, consider using `Sparse=true` and `NonZero=true` to reduce output size
- The API scales with the number of neurons and synapses in the simulation
- Memory usage is proportional to the total number of synaptic connections

## Use Cases

1. **Connectome Analysis**: Export detailed network structure for graph analysis
2. **Synaptic Mapping**: Analyze spatial distribution of synapses
3. **Network Visualization**: Create 3D visualizations of neural circuits  
4. **Data Exchange**: Convert BrainGenix simulations to standard neuroscience formats
5. **Connectivity Studies**: Analyze connection patterns and strengths 