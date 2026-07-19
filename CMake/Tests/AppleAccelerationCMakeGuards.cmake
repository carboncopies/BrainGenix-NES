if(NOT DEFINED CORE_CMAKE_FILE)
    message(FATAL_ERROR "CORE_CMAKE_FILE must point to Source/Core/CMakeLists.txt")
endif()

file(READ "${CORE_CMAKE_FILE}" CoreCMake)

string(FIND "${CoreCMake}" "if(APPLE AND CMAKE_SYSTEM_PROCESSOR MATCHES \"^(arm64|aarch64)$\")" GuardStart)
if(GuardStart EQUAL -1)
    message(FATAL_ERROR "Apple acceleration must be gated to Apple ARM64 builds.")
endif()

foreach(AppleSource
    "AppleGaussianBlur.mm"
    "MetalVoxelFill.mm")
    string(FIND "${CoreCMake}" "${AppleSource}" SourcePosition)
    if(SourcePosition EQUAL -1 OR SourcePosition LESS GuardStart)
        message(FATAL_ERROR "${AppleSource} is not contained by the Apple ARM64 CMake guard.")
    endif()
endforeach()

foreach(AppleFramework
    "-framework Foundation"
    "-framework Accelerate"
    "-framework Metal"
    "-framework MetalPerformanceShaders")
    string(FIND "${CoreCMake}" "${AppleFramework}" FrameworkPosition)
    if(FrameworkPosition EQUAL -1 OR FrameworkPosition LESS GuardStart)
        message(FATAL_ERROR "${AppleFramework} is not contained by the Apple ARM64 CMake guard.")
    endif()
endforeach()
