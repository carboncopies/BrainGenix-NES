##########################
# Builds Library For ERS #
##########################

ERSBuildLogger(${Green} "Configuring Tracy Library")
add_subdirectory(${LIB_DIR}/tracy)
include_directories(${LIB_DIR}/tracy)
disable_target_warnings(TracyClient)
ERSBuildLogger(${BoldGreen} "Finished Configuring Tracy Library")