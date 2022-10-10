# Set CMAKE Version, Define Project
cmake_minimum_required (VERSION 3.1...3.21)
project(
	"BrainGenix-ERS"
	VERSION "0.4.0"
	LANGUAGES CXX
	)

# Set Configuration Options
option(ENABLE_BACKWARD "Build ERS With Backward to provide some stack trace information or not" ON)


# Set Dirs
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Third-Party")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Source")
set(BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Binaries")
set(CMAKE_UTIL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Utils")
set(CMAKE_BUILD_UTILS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/CMake/BuildUtils")
set(CMAKE_FIND_UTILS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/CMake/FindUtils")
set(CMAKE_SCRIPTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Scripts")
set(CMAKE_ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/EditorAssets")
set(PROJECT_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_VISIBILITY_PRESET hidden)


# Include Util CMake Scripts
include(${CMAKE_UTIL_DIR}/ColorizeMessages.cmake)
include(${CMAKE_UTIL_DIR}/SetBinaryDir.cmake)
include(${CMAKE_UTIL_DIR}/DisableTargetWarnings.cmake)

# Run Configuration Tools
include(${CMAKE_SCRIPTS_DIR}/VersioningSystem/VersioningSystem.cmake)
include(${CMAKE_SCRIPTS_DIR}/CompilePlatformInfo/CompilePlatformInfo.cmake)
include(${CMAKE_SCRIPTS_DIR}/CompileTimeStamp/CompileTimeStamp.cmake)

# Include Package Addition Scripts
include(${CMAKE_BUILD_UTILS_DIR}/Lucifer.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/Backward.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/ThreadNamingUtils.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/Python.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/Threads.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/YAML-CPP.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/Glad.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/GLM.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/ImGui.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/ImGuizmo.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/ImPlot.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/ImGuiFileDialog.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/ImGuiColorTextEdit.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/GLFW.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/PCIIDS.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/FreeImage.cmake)
#include(${CMAKE_BUILD_UTILS_DIR}/STB.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/Assimp.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/Tracy.cmake)
#include(${CMAKE_BUILD_UTILS_DIR}/MySQLConnector.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/OpenXR.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/PyBind11.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/OZZ-Animation.cmake)
include(${CMAKE_BUILD_UTILS_DIR}/MeshOptimizer.cmake)


# Add LIBRARY INTERFACE SUBDIRECTORIES
ERSBuildLogger(${Green} "Configuring Library Interfaces")

add_subdirectory(${SRC_DIR}/Interface/Library/MeshOptimizerInterface)
message("-- Configured MeshOptimizer Interface")

ERSBuildLogger(${BoldGreen} "Finished Configuring Library Interfaces")


# ADD STRUCT INTERFACE SUBDIRECTORIES
ERSBuildLogger(${Green} "Configuring Structure Interfaces")

ERSBuildLogger(${BoldGreen} "Finished Configuring Structure Interfaces")


# ADD INTERNAL SUBDIRECTORIES
ERSBuildLogger(${Green} "Configuring Internal Libraries")

add_subdirectory(${SRC_DIR}/Internal/LoggingSystem)
message("-- Configured Internal Library 'LoggingSystem'")
add_subdirectory(${SRC_DIR}/Internal/OpenGLLoggingSystem)
message("-- Configured Internal Library 'OpenGLLoggingSystem'")
add_subdirectory(${SRC_DIR}/Internal/ArgumentParser)
message("-- Configured Internal Library 'ArgumentParser'")
add_subdirectory(${SRC_DIR}/Internal/GPURequest)
message("-- Configured Internal Library 'GPURequest'")
add_subdirectory(${SRC_DIR}/Internal/HardwareInformation)
message("-- Configured Internal Library 'HardwareInformation'")
add_subdirectory(${SRC_DIR}/Internal/InputOutputSubsystem)
message("-- Configured Internal Library 'InputOutputSubsystem'")
add_subdirectory(${SRC_DIR}/Internal/ExternalModelLoader)
message("-- Configured Internal Library 'ExternalModelLoader'")
add_subdirectory(${SRC_DIR}/Internal/ModelImporter)
message("-- Configured Internal Library 'ModelImporter'")


ERSBuildLogger(${BoldGreen} "Finished Configuring Internal Libraries")


# ADD CORE SUBDIRECTORIES
ERSBuildLogger(${Green} "Configuring Core Subsystems")
add_subdirectory(${SRC_DIR}/Core/Manager/ERS_FramerateManager)
add_subdirectory(${SRC_DIR}/Core/Manager/ERS_SceneManager)
add_subdirectory(${SRC_DIR}/Core/Manager/ERS_ProjectManager)
add_subdirectory(${SRC_DIR}/Core/Manager/ERS_ControllerManager)

add_subdirectory(${SRC_DIR}/Core/Editor/)
add_subdirectory(${SRC_DIR}/Core/Editor/Utils/ERS_Editor_ThemeManager)
add_subdirectory(${SRC_DIR}/Core/Editor/Utils/ERS_Editor_FontManager)
add_subdirectory(${SRC_DIR}/Core/Editor/Utils/ERS_Editor_UserProfileManager)
add_subdirectory(${SRC_DIR}/Core/Editor/Utils/ERS_Editor_LayoutManager)
add_subdirectory(${SRC_DIR}/Core/Editor/Utils/ERS_Editor_3DCursor)
add_subdirectory(${SRC_DIR}/Core/Editor/Utils/ERS_Editor_WindowManager)
add_subdirectory(${SRC_DIR}/Core/Editor/Windows)
add_subdirectory(${SRC_DIR}/Core/Editor/Menus)

add_subdirectory(${SRC_DIR}/Core/Renderer/ERS_CLASS_RendererManager)
add_subdirectory(${SRC_DIR}/Core/Renderer/ERS_CLASS_VisualRenderer)

add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_Camera)
add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_SceneCamera)
add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_HumanInputDeviceUtils)
add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_Mesh)
add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_Model)
add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_Texture)
add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_TextureLevel)
add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_Vertex)
add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_Scene)
add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_Shader)
add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_HardwareInfo)
add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_LocRotScale)
add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_IOData)
add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_SystemUtils)
add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_ProjectUtils)
add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_Project)
add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_ControllerSettings)
add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_ModelWriterData)
add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_ShaderProgramAssetIDs)
add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_Viewport)
add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_Light)
add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_OpenGLDefaults)
add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_SceneObject)
add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_Script)
add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_DepthMap)
add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_RendererSettings)
add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_ShaderUniformData)
add_subdirectory(${SRC_DIR}/Core/Structures/ERS_STRUCT_ModelImportOptions)

add_subdirectory(${SRC_DIR}/Core/Script/ERS_CLASS_PythonInterpreterIntegration)

add_subdirectory(${SRC_DIR}/Core/Loader/ERS_SceneLoader)
add_subdirectory(${SRC_DIR}/Core/Loader/ERS_ShaderLoader)
add_subdirectory(${SRC_DIR}/Core/Loader/ERS_ProjectLoader)
add_subdirectory(${SRC_DIR}/Core/Loader/ERS_ModelLoader)
add_subdirectory(${SRC_DIR}/Core/Loader/ERS_ControllerSettingsLoader)

add_subdirectory(${SRC_DIR}/Core/Writers/ERS_SceneWriter)
add_subdirectory(${SRC_DIR}/Core/Writers/ERS_ModelWriter)
add_subdirectory(${SRC_DIR}/Core/Writers/ERS_ProjectWriter)
ERSBuildLogger(${BoldGreen} "Finished Configuring Core Subsystems")


# Add Main.cpp
add_executable(${PROJECT_NAME} ${SRC_DIR}/Main.cpp ${LIB_DIR}/tracy/TracyClient.cpp ${BACKWARD_ENABLE})
target_link_libraries(${PROJECT_NAME}

  # Third Party Libs
  yaml-cpp
  glad
  glfw
  TracyClient
  ozz_animation
  Lucifer

  # Internal Libs
  ERS_GPURequest
  ERS_LoggingSystem
  ERS_ModelImporter


  ERS_HardwareInformation
  ERS_CLASS_PythonInterpreterIntegration
  ERS_ArgumentParser

  ERS_SceneManager
  ERS_ModelLoader
  ERS_SceneLoader  
  ERS_ModelWriter
  ERS_ProjectLoader
  ERS_ProjectWriter
  ERS_InputOutputSubsystem
  ERS_FramerateManager
  ERS_ProjectManager
  ERS_ControllerInputManager
  
  ERS_STRUCT_HumanInputDeviceUtils
  ERS_STRUCT_SystemUtils
  ERS_STRUCT_ProjectUtils
  ERS_STRUCT_RendererSettings

  Renderer
  )

# COPY FILES TO BINARY DIR
file(COPY "${CMAKE_ASSETS_DIR}/" DESTINATION "${BIN_DIR}/EditorAssets")
file(COPY "${CMAKE_ASSETS_DIR}/DefaultLayout/imgui.ini" DESTINATION "${BIN_DIR}")

# Set Configuration Based On Platform
if (WIN32 OR APPLE)
  configure_file("${SRC_DIR}/Config-Windows.yaml" "${BIN_DIR}/Config.yaml" COPYONLY)
else()
  configure_file("${SRC_DIR}/Config-Linux.yaml" "${BIN_DIR}/Config.yaml" COPYONLY)
endif()