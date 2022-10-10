# Lucifer Image Library
Wrapper for other various image loading libraries, provides a robust and reliable way to load many differnet image formats from different exporters. Comes with all essentiall and most non-essential dependencies in the repo.


## Documentation
 There isn't any formal documentation as of yet, but you can see the existing ![Example](Example/Example.cpp) file which covers just about everything you'll need to get started. The API is pretty self-explanitory as well, so you really don't need any documentation.


## Integration Instructions
Simply clone the repository and include it as a subdirectory within your own cmake project. Once you've done that, just link your target with Lucifer. (target_link_libraries). Below is a quick example snippit:
```
add_subdirectory(Third-Party/Lucifer) # Change this to fit where you put the Lucifer Directory

add_executable(Example Main.cpp)
target_link_libraries(Example Lucifer)
```
If you need some more examples, please see the ![Tests](Tests) folder. 

## Design Philosophy
This library was created to sastisfy the need to reliably load many different formats from different points in time and different writers. It's primary goal is not to load images as fast as possible but rather allow you to load them and then convert to a format you can use quickly later. I first discovered this issue with my main work project BrainGenix, where we needed to import textures from various models. We would frequently encounter issues when loading images due to problems caused by images failing to load correctly. 

So, with this library, I'm aiming to make it easier to get these to load by stringing together a few different large image libraries to give the best chance of one of those working at decoding the image.

## What's up with the name?
This project started out as a simple extension to DevIL, but eventually turned into something else, so I thought it would be fitting to find some other Devil related name.

## Issues/Bugs
Please feel free to make use of the Issues tab.

