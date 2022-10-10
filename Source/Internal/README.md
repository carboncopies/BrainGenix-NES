# Process To Create Internal Lib

## If New
- Create new directory with name of subsystem
- Use other cmake lists from other subsystems as a template to setup yours
- Add a README.md file explaining what your module does
- Add it to the root cmakelists.txt to be built
- use your new internal lib in ERS

## If Existing
- Move existing code over to this system
- Add a README.md file explaining what your module does
- Add it to the root cmakelists.txt to be built
- use your new internal lib in ERS
