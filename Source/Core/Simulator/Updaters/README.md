# A brief note about how simulations are run and how they are created / modified

- The `Engine.cpp` file has a `RunFor()` function, which calls `RunFor()` in
  the simulation object. That is in `Structs/Simulation.cpp`. There, for each
  neural circuit, the `Update()` function is called, which then does the same in
  each cell.
- The circuits and cells for which update functions can be run are kept in lists
  such as `Simualtion->NeuralCircuits`. These lists need to be updated when components
  are created.
- An example of adding components to lists is when an RPC call is received for
  a new geometry component, e.g. a Sphere. The RPC routes are defined in the
  `Manager::Manager()` constructor in the `Manager.cpp` file. The call to
  `Manager::SphereCreate()` populates a new `Geometries::Sphere` struct. The
  next Sphere ID is obtained from the size of the `Simulation->Collection.Geometries`
  list. The new Sphere struct is pushed to the back of `Simulation->Collection.Geometries`.
- Similarly, new neuron compartments need to be created, their parameters set in a
  Manager member function, and added to a list of compartments specific to a
  circuit and neuron that were defined previously.
  The order in which the data is given to the simulation does not matter. We can
  create the components first, because they are then simply put into the `BSCompartments`
  in the `Simulation` object. If we remember the IDs we can use that to add them to a
  specific neuron in a specific circuit.
  This is already implemented in `Manager::BSCreate()`.

NOTES:

- If we are keeping the `NES_Interfaces/BS_Neuron` code in the VBP front-end, then
  it is sufficient to generate the compartments in the backend and connect them
  (with staples etc). We do not need to use the `BallAndStick/BS_Neuron` code in
  the backend - unless we want to be able to use high level calls into the backend
  where we simply request that a neuron is built and it generates the compartments.
  QUESTION: Should we provide high level API calls that build entire neurons or
            even entire circuits all at once? I.e. how much functionality should
            we push into the backend?
            We either need to enable this, or we need to create the compartments,
            then create a Neuron, and assign the compartments to the Neuron.

- To work through this while testing, pull a new version of the backend, build and
  run, and set up the local API to point to localhost:80 while telling it not to
  use http (see its configuration). Then I ought to be able to print what is
  being created and what simulating attempts to run.

--
Randal A. Koene, 2024