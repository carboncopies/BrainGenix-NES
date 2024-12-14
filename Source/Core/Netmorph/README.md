# Order of ops for a Netmorph run within NES

1. All NetmorphParameters that need to be specified before starting a run
   need to be specified in advance through the API. That includes setting
   which Netmorph Log buffers to maintain for potential retrieval.
2. An API called "Netmorph/StartSimulation" runs the Netmorph simulation.
3. Results stored, including buffered Netmorph logging can be retrieved
   through the API after the Netmorph simulation has completed.

