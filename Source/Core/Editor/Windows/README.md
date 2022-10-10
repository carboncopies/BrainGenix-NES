# Instructions To Add New Windows
Adding windows to ERS is a fairly simple process, and can be accomplished as follows:

    - ## Setting Up Window Class/Directory
    - Create a subdirectory here with the format "GUI_Window_{NameOfYourWindow}"
    - Add the cmake and other info, just copy the structure of any existing one
    - Add that subdirectory you created to the CMakeLists.txt file in this directory "add_subdirectory({YourSubDir})"
    
    - ## Add To Windows Struct
    - Now, head over to the WindowManager Class located (../Utils/ERS_Editor_WindowManager)
    - Add your cmake target to the CMakeLists.txt in the window manager directory (instructions in file)
    - In the file "ERS_STRUCT_Windows.h", include the header file for your new window, and add a unique_ptr to the struct containing your window (follow naming conventions there)

    - ## Add To WindowManager Class
    - In the file "ERS_Editor_WindowManager.h", include your window's header file there as well.
    - In "ERS_Editor_WindowManager.cpp", make sure to instantiate your window in the function named "GenerateWindowStruct" (follow what other windows are doing) 
    - Additionally, make sure to add it to the update and other functions in the class - instructions in class file

    - ## Add To Menu Class
    - Open whatever menu you want to control the window (if any)
    - Add to menu using enabled/disabled function in window or any other config param as normal