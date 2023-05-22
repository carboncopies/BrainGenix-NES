# About
The client allows the user to actually talk to a backend and run their code (even though for now the backend is on their own machine). The client API will be designed to model that of graphics rendering libraries - that is to say, users should create their neurons, buffer them, and then can simulate them in groups. (similar to draw calls).

We will implement a paradigm in which users create models (by instancing the Frontend models), set the parameters for them, and then pass those objects to the client to be added to the simulation. They are given back and ID which may be used to destroy them later.
