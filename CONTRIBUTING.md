# Contribution Guide
First of all, thank you for taking the time to contribute, we really appreciate it.
With that said, we would also appreciate it if you would make sure that your code conforms to the standards below:

- Please use UpperCamelCase for naming varaibles, and add a trailing Underscore_ for class member variables.
- When creating functions/classes/structs, please add ERS_CLASS_, ERS_STRUCT_, or ERS_FUCTION_ to the start of your name.
- When creating directories, please also add ERS_[name of function] to the directory you put your files under.
- For file names, please also add ERS_[CLASS/STRUCT/FUNCTION]_ to the start of the name to indicate what is contained within the file.
- Every dir should have it's own CMakeLists.txt file which tells cmake how to build your code. These can be adapted from the example below.
- Make sure to call add_subdirectory with that dir in the root CMakeLists.txt file, or else it won't build.
- Please also document your code with doxygen formatting. We use doxygen to generate the online docs page, so all code should use that format.
- Finally, please also add the following boilerplate code below to your the start of your code.

## Templates

Template `CMakeLists.txt` File:

```
########################################################################
# This file is part of the BrainGenix-ERS Environment Rendering System #
########################################################################

# Create Library (Name Should Be Parent Dir Name)
add_library([NAME OF PARENT DIR HERE]

            # Add Source Files (.cpp)
            "[Name of your .cpp file, (keep quotes) add more below if more than one]"

            # Add Header Files (.h)
            "[Name of your .h file, (keep quotes) also add more below if needed]"

            ${BACKWARD_ENABLE}
            )

# Link 3rd Party Libs
target_link_libraries([NAME OF PARENT DIR HERE]
    # whatever thrid party libs your code needs
    )

# Link Internal Libs
target_link_libraries([NAME OF PARENT DIR HERE]
    # Whatever other ers cmake targets your code needs
    )

target_include_directories([NAME OF PARENT DIR HERE] PUBLIC ./)
```

Template Header `.cpp` files:
```
//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//
```

Template Header `.h` files:

```
//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once


// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
```
