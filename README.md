![Build](https://github.com/carboncopies/BrainGenix-ERS/actions/workflows/cmake.yml/badge.svg)


![Image](https://user-images.githubusercontent.com/49183477/193433511-1276948e-29e8-43d0-8c5e-8f87e097fec9.png)



# Build Instructions

## Linux:

  It's pretty easy to build on linux and has been tested with Debian/Ubuntu to work. This is the primary development platform used, and such tends to work the best. 
  
  Simply enter the `Tools` directory and run `sudo bash Install-Linux.sh` followed by `bash Build.sh 4`. The build command can be adjusted to fit the number of cpu cores that your system has by changing the argument. If you don't know how many CPU cores your system has, just leave the command as is. If something isn't working quite right for you, you can try cleaning the build with `bash Clean.sh`.

  If some packages are missing on your distro, you'll have to substitute with whatever your distro uses. We try to include as many packages in our codebase as possible, but this isn't always possible or practical.


## Windows:

  Since we don't develop on Windows, builds aren't guarenteed to work. With that said, they *should* work most of the time.   
  
  **Note: Having windows set to a non english language seems to break includes for some reason.** We really wish that this wasn't an issue but it seems to be a problem for whatever reason. Apologies for the inconvenience.

  - Open Visual Stuido 2019 (Has to be this version)
  - Install python 3.9.5 (needed to link with the interpreter, embeddable version already included in project tree, also restart your machine after installing to avoid issues) (If just installing, make sure to restart after to make it show up in visual studio)
  - Clone the repository but note that you'll probably have to put it into a root level directory on your disk. This is because Microsoft has imposed a 260 character file name limit in their compiler. 
  - Open the CMakeLists.txt file, and generate the configuration
  - Put your build config into "Release", currently debug doesn't work for some reason.
  - Set target to BrainGenix-ERS.exe
  - Hit `Build`
  
  If you encounter issues finding the python libraries after you've installed them, try clearing your cmake cache.

## Mac:
  
  - Install one of the following:
    - [Ubuntu](https://ubuntu.com/download)
    - [Debian](https://www.debian.org/distrib/)
    - [Fedora](https://getfedora.org/workstation/download/)
  - Follow Linux build instructions

# Keybinds

  ## Editor Viewport

    "[lmb]" - (When Hovering Gizmo) - Move Gizmo
    "[left shift]" - (When Gizmo Active) - Move Camera While Moving Gizmo
    "[esc]" - (When Gizmo Active) - Abort Current Movement
    "[s]" - Set Gizmo To Scale
    "[g]" - Set Gizmo To Translate
    "[r]" - Set Gizmo To Rotate


    "[lmb]" - (When Not Hovering Gizmo) - Enter Camera Movement Mode (hold button)
    "[lmb] + [w]" - Move Camera Forward
    "[lmb] + [a]" - Move Camera Left
    "[lmb] + [s]" - Move Camera Backward
    "[lmb] + [d]" - Move Camera Right
    "[lmb] + [left shift]" - Move Camera Down
    "[lmb] + [space]" - Move Camera Up

    "[lmb] + [scroll wheel]" - Adjust Camera Movement

    "[`]" - Toggle Viewport Controls On/Off

# Scripting API

  The ERS scripting api uses the Python programming language to provide control over models, lights and more. Scripts can be "attached" to an object to allow for programatic control over it's attributes. Plese see the following API reference for more information.
  
  ## System Variables (provided to all scripts)
    "GameTime" - Number of seconds since the start of the game.
    "SystemTime" - Unix epoch as reported by the system.
    
  ## Model Variables
    "ModelPosX" - X Position of the model
    "ModelPosY" - Y Position of the model
    "ModelPosZ" - Z Position of the model
    "ModelRotX" - X Rotation of the model
    "ModelRotY" - Y Rotation of the model
    "ModelRotZ" - Z Rotation of the model
    "ModelScaleX" - X Scale of the model
    "ModelScaleY" - Y Scale of the model
    "ModelScaleZ" - Z Scale of the model
    "ModelEnabled" - Enable/Disable rendering of the model

  ## Point Light Variables
    "PointLightPosX" - X Position of the light
    "PointLightPosY" - Y Position of the light
    "PointLightPosZ" - Z Position of the light
    "PointLightColorR" - Red component of Color(from scale of 0 to 1)
    "PointLightColorG" - Green component of color (from scale of 0 to 1)
    "PointLightColorB" - Blue component of color (from scale of 0 to 1)
    "PointLightIntensity" - brighness of the light (float)
    "PointLightMaxDistance" - maximum distance of the light after which the scene will not be affected (float)
    "PointLightRolloffConstant" - Constant component of light rolloff (float)
    "PointLightRolloffLinear" - Linear component of light rolloff (float)
    "PointLightRolloffQuadratic" - Quadratic component of light rolloff (float)
    
  ## Directional Light Variables
    "DirectionalLightPosX" - X Position of the light
    "DirectionalLightPosY" - Y Position of the light
    "DirectionalLightPosZ" - Z Position of the light
    "DirectionalLightRotX" - X Rotation of the light
    "DirectionalLightRotY" - Y Rotation of the light
    "DirectionalLightRotZ" - Z Rotation of the light
    "DirectionalLightIntensity" - brighness of the light (float)
    "DirectionalLightMaxDistance" - maximum distance of the light after which the scene will not be affected (float)
    "DirectionalLightColorR" - Red component of color (from scale of 0 to 1)
    "DirectionalLightColorG" - Green component of color (from scale of 0 to 1)
    "DirectionalLightColorB" - Blue component of color (from scale of 0 to 1)

  ## Spot Light Variables
    "SpotLightPosX" - X Position of the light
    "SpotLightPosY" - Y Position of the light
    "SpotLightPosZ" - Z Position of the light
    "SpotLightRotX" - X Rotation of the light
    "SpotLightRotY" - Y Rotation of the light
    "SpotLightRotZ" - Z Rotation of the light
    "SpotLightColorR" - Red component of color (from scale of 0 to 1)
    "SpotLightColorG" - Green component of color (from scale of 0 to 1)
    "SpotLightColorB" - Blue component of color (from scale of 0 to 1)
    "SpotLightIntensity" - brighness of the light (float)
    "SpotLightMaxDistance" - maximum distance of the light after which the scene will not be affected (float)
    "SpotLightCutoff" - Angle of area affected in degrees (float)
    "SpotlightRolloff" - Rolloff angle in degrees (float)
