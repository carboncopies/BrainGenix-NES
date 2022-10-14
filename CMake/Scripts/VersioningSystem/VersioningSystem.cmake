###############################
# ERS Build Versioning System #
###############################

ERSBuildLogger(${Green} "Configuring Versioning System")

message(" -- Configuring File Version.h.in")
configure_File(${CMAKE_SCRIPTS_DIR}/VersioningSystem/Version.h.in ${SRC_DIR}/Interface/Configuration/VersioningSystem/Version.h)
message(" -- Adding Version Header Target")
add_subdirectory(${SRC_DIR}/Interface/Configuration/VersioningSystem)

ERSBuildLogger(${BoldGreen} "Finished Configuring Versioning System")