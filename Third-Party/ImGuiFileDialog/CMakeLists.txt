# THIS CMAKE IS NOT PART OF THE LIBRARY, AND WAS MADE BY THE ERS DEV TEAM


add_library(ImGuiFileDialog 
    "ImGuiFileDialog.h"
    "ImGuiFileDialogConfig.h"
    "ImGuiFileDialog.cpp"
)



target_link_libraries(ImGuiFileDialog

    IMGUI
    glad
    glfw
)

option(FILESYSTEM_OVER_DIRENT "Enable std::filesystem use for path and ImGuiFileDialog" OFF)

message("Configuring ImGuiFileDialog File Interface System")
if (FILESYSTEM_OVER_DIRENT)
	add_definitions(-DUSE_STD_FILESYSTEM)
	set(CMAKE_CXX_STANDARD 17)
	set_target_properties(ImGuiFileDialog PROPERTIES CXX_STANDARD 17)
	message("ImGuiFileDialog USING C++17 and std::filesystem")
else()
	set(CMAKE_CXX_STANDARD 11)
	set_target_properties(ImGuiFileDialog PROPERTIES CXX_STANDARD 11)
	message("ImGuiFileDialog Using C++11 and DIRENT")
endif()
message("Finished Configuring File Interface System")


target_include_directories(ImGuiFileDialog PUBLIC ./)