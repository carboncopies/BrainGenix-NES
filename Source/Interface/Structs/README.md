# Interface Structs

The interface structs are designed to be used as the name says - to provide interfaces between different modules. For example, the ModelLoader subsystem should link to the model class. If another system uses that model, it can get it from there without having to include any other struct. This will help to keep all the different modules *modular*.