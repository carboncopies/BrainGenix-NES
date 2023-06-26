###############################
# BG Build Versioning System #
###############################

BGBuildLogger(${Green} "Configuring Versioning System")

message(" -- Configuring File Version.h.in")
configure_File(${CMAKE_SCRIPTS_DIR}/VersioningSystem/Version.h.in ${SRC_DIR}/Info/Static/VersioningSystem/Version.h)
message(" -- Adding Version Header Target")
add_subdirectory(${SRC_DIR}/Info/Static/VersioningSystem)

BGBuildLogger(${BoldGreen} "Finished Configuring Versioning System")