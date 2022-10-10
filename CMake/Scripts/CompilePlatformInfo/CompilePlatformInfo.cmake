###############################
# ERS Build Versioning System #
###############################

ERSBuildLogger(${Green} "Configuring Platform Info")

message(" -- Detecting Platform Information")
set(TARGET_OS_NAME ${CMAKE_SYSTEM_NAME})
set(TARGET_OS_VERSION ${CMAKE_SYSTEM_VERSION})
set(TARGET_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR})

set(HOST_OS_NAME ${CMAKE_HOST_SYSTEM_NAME})
set(HOST_OS_VERSION ${CMAKE_HOST_SYSTEM_VERSION})
set(HOST_PROCESSOR ${CMAKE_HOST_SYSTEM_PROCESSOR})

message(" -- Configuring File ERS_CompilePlatformInfo.h.in")
configure_File(${CMAKE_SCRIPTS_DIR}/CompilePlatformInfo/ERS_CompilePlatformInfo.h.in ${SRC_DIR}/Interface/Configuration/CompilePlatformInfo/ERS_CompilePlatformInfo.h)
message(" -- Adding CompilePlatformInfo Header Target")
add_subdirectory(${SRC_DIR}/Interface/Configuration/CompilePlatformInfo)

ERSBuildLogger(${BoldGreen} "Finished Configuring Platform Info")