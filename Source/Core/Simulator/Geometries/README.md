# 3D geometric tools

Lots of useful things in here. This file contains some useful reminders / notes.

## About getting from a definition of 2 end points to a 3D rotation in x,y,z

This is something that is needed to specify a rotated box, or a cylinder.
The way we did this was to covert to spherical coordinates and to then use
the angles in there for rotations. To see the details, see the function
`Cylinder::WriteToVoxelArray` in `Cylinder.cpp`.
This refers to the use of rot_y = .theta() and rot_z = .theta(), applied in
that order with rot_z = 0.0 for an xyz rotation. (This does not refer to
the two rotate_around_xyz() calls at the top of the function.)
