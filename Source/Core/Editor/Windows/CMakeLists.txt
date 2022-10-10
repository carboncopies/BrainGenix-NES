#######################################################################
# Use This CMakeLists.txt To Add Your Source Windows Subfolder Cmakes #
#######################################################################






# Add Window Subdirs
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GUI_Window_ProjectSettings)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GUI_Window_SceneTree)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GUI_Window_ImportProgressBar)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GUI_Window_About)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GUI_Window_SystemLog)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GUI_Window_AssetExplorer)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GUI_Window_ControllerSettings)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GUI_Window_TestEditor)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GUI_Window_ShaderEditor)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GUI_Window_ScriptEditor)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GUI_Window_ImportModel)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GUI_Window_ImportModelDirectory)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GUI_Window_OpenProject)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GUI_Window_NewProject)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GUI_Window_ThemeSelector)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GUI_Window_FontSelector)


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GUI_Window_RAMGraph)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GUI_Window_ObjectProperties)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GUI_Window_RenderingSettings)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GUI_Window_EditorCameraSettings)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GUI_Window_AssetStreamingSettings)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GUI_Window_FramerateGraph)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GUI_Window_FrameLatencyGraph)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GUI_Window_FramerateCounter)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GUI_Window_FramerateHistogram)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GUI_Window_FrameratePlot)