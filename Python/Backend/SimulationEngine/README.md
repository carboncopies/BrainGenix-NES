# About
As the name suggests, the simulation engine actually runs the simulation. It also contains the code needed to save and load simulations as requested by the client.

It will own the simulation object, and all models within, and run the updater classes for each type of model that is in the simulation and is requested to be updated.