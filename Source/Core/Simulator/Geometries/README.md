# 3D geometric tools

Lots of useful things in here. This file contains some useful reminders / notes.

## About getting from a definition of 2 end points to a 3D rotation in x,y,z

This is something that is needed to specify a rotated box, or a cylinder.
The way we did this was to covert to spherical coordinates and to then use
the angles in there for rotations. To see the details, see the function
`Cylinder::WriteToVoxelArray` in `Cylinder.cpp`.
