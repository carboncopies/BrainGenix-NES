cmake_minimum_required (VERSION 3.1...3.21)


# Set Dirs
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Source")
set(BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Binaries")
set(CMAKE_UTIL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Utils")
set(CMAKE_SCRIPTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/CMake/CompileInfo")
set(PROJECT_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Utils)

set(INSTALL_PREFIX "/etc/BrainGenix/NES")


# Setup vcpkg script with CMake (note: should be placed before project() call)
set(CMAKE_TOOLCHAIN_FILE ${LIB_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")


# Define Project
project(
	"BrainGenix-NES"
	VERSION "2024.02.02"
	LANGUAGES CXX
)

# Set project static library name
set(PROJECT_LIBRARY_NAME "braingenix_nes")


# Include Util CMake Scripts
include(${CMAKE_UTIL_DIR}/ColorizeMessages.cmake)
include(${CMAKE_UTIL_DIR}/SetBinaryDir.cmake)

# Add Libraries
message(STATUS "Finding nlohmann_json Package")
find_package(nlohmann_json CONFIG REQUIRED)

# message(STATUS "Finding boost Package")
# set(Boost_NO_WARN_NEW_VERSIONS 1)
# find_package(Boost REQUIRED COMPONENTS program_options)

message(STATUS "Finding rpclib Package")
find_package(rpclib CONFIG REQUIRED)

message(STATUS "Finding system pthreads lib")
find_package(Threads REQUIRED)

find_path(CPP_BASE64_INCLUDE_DIRS "cpp-base64/base64.cpp")


# Renderer Dependencies
# message(STATUS "Setting up Vulkan Library")
# find_package(Vulkan REQUIRED)

# message(STATUS "Setting Up vk-bootstrap Library")
# # find_package(vk-bootstrap CONFIG REQUIRED) # Will eventually use this when vcpkg sorts out their build issues
# # in the meantime, this works:
# include(FetchContent)
# FetchContent_Declare(
#     fetch_vk_bootstrap
#     GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
#     GIT_TAG        v0.7 #suggest using a tag so the library doesn't update whenever new commits are pushed to a branch
# )
# FetchContent_MakeAvailable(fetch_vk_bootstrap)


message(STATUS "Setting Up yaml-cpp Library")
find_package(yaml-cpp CONFIG REQUIRED)

message(STATUS "Setting Up Vulkan Scene Graph Library")
find_package(vsg CONFIG REQUIRED)

# message(STATUS "Setting Up vsgXchange Library")
# find_package(vsgXchange CONFIG REQUIRED)

message(STATUS "Setting Up STB Library")
find_package(Stb REQUIRED)

message(STATUS "Setting Up LibNoies Library")
find_package(unofficial-noise CONFIG REQUIRED)

# Compiler optimizations
find_package(AVX)
if (${AVX_FOUND})
    message(STATUS "Found AVX Extensions")
else()
    message(STATUS "Did Not Find AVX Extensions")
endif()


# Internal Deps
message(STATUS "Finding bg-common-logger Package")
find_package(bg-common-logger CONFIG REQUIRED)




# Enabling testing and finding GTest
enable_testing()
message(STATUS "Finding gtest Package")
find_package(GTest CONFIG REQUIRED)

# Ensure config is copied to bin
file(COPY "${SRC_DIR}/Data/NES.yaml" DESTINATION "${BIN_DIR}")



# Run Configuration Tools
include(${CMAKE_SCRIPTS_DIR}/VersioningSystem/VersioningSystem.cmake)
include(${CMAKE_SCRIPTS_DIR}/CompilePlatformInfo/CompilePlatformInfo.cmake)
include(${CMAKE_SCRIPTS_DIR}/CompileTimeStamp/CompileTimeStamp.cmake)



# Options
# option(WITH_AVX "Compile With AVX Instructions" ${AVX_FOUND})
option(WITH_DEBUG_SYMBOLS "Enable or disable debug symbols" ON)
set(WITH_AVX ${AVX_FOUND})



# Apply Options
if (WITH_AVX)
    message(STATUS "AVX2 Extensions Will Be Used")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${AVX_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${AVX_FLAGS}")
else()
    message(STATUS "AVX2 Extensions Will Not Be Enabled")
endif()

if (WITH_DEBUG_SYMBOLS)
    message(STATUS "Enabling Debug Symbols")
    SET(CMAKE_CXX_FLAGS "-g")
else()
    message(STATUS "Not Enabling Debug Symbols")
endif()



# Add Source Subdirs
add_subdirectory(${SRC_DIR}/Core)
add_subdirectory(${SRC_DIR}/Renderer)



# Set Install Targets
message(STATUS "Install Prefix Set To '${INSTALL_PREFIX}'")
message(STATUS "Executable Will Be Installed To '/usr/bin'")
install(TARGETS ${PROJECT_NAME}
    COMPONENT Release
    RUNTIME DESTINATION "/usr/bin/"
    LIBRARY DESTINATION ${INSTALL_PREFIX}
    DESTINATION ${INSTALL_PREFIX}
)
install(FILES "${SRC_DIR}/Data/NES.yaml"
    DESTINATION ${INSTALL_PREFIX}
    COMPONENT Release
)
message(STATUS "Installing Unit File To '/etc/systemd/system'")
install(FILES "${SRC_DIR}/Data/BrainGenix-NES.service"
    DESTINATION "/etc/systemd/system/"
    COMPONENT Release
)

# Include CTest for testing
include(CTest)



# TEMPORARY COPIES FOR SHADER CODE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# file(COPY "${SRC_DIR}/Data/vert.spv" DESTINATION "${BIN_DIR}")
# file(COPY "${SRC_DIR}/Data/frag.spv" DESTINATION "${BIN_DIR}")

file(COPY "${SRC_DIR}/Data/" DESTINATION "${BIN_DIR}")



# Include CTest for testing
include(CTest)


# CPack Information
SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Carboncopies Foundation")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${SRC_DIR}/Data/prerm;${SRC_DIR}/Data/postrm;${SRC_DIR}/Data/postinst")
SET(CPACK_OUTPUT_FILE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/Artifacts)
INCLUDE(CPack)
