# SaveModel and LoadModel API calls

The `SaveModel` and `LoadModel` API calls store and retrieve compact
and fast binary data of a simulation model, both morphological information
and functional parameters.

The function `Simulation::SaveModel()` is called.

This uses a `Saver` object, which is used to prepare a **flat** representation
of the whole model.

Components of the model are prepared in the following order:

1. Shapes: Each shape object in `Simulation::Collection` is parsed and retrieved
   with `GetSphere()`, `GetCylinder()`, `GetBox()`. Each is added to the
   `Saver` object with `AddSphere()`, `AddCylinder()`, `AddBox()`.
2. Compartments: Added to `Saver` with `AddBSSCCompartments()`.
3. Neurons: Added to `Saver` with `AddNeurons()`.
4. Receptors: Added to `Saver` with `AddReceptos()`.
5. Regions: Added to `Saver` with `AddRegions()`.
6. Circuits:  Added to `Saver` with `AddCircuits()`.

Note that the order is such that when loaded using a `Loader` object, each of
the components can be created immediately in a new `Simulation`.

## Compatibility

`SaveModel` and `LoadModel` are not available for simulations composed of
BS Neurons.

`SaveModel` and `LoadModel` use different `Saver` and `Loader` classes for
simulations composed of SC Neurons or LIFC Neurons, because the two types
use overlapping but different parameters.


---
Randal A. Koene, 2025-07-28
