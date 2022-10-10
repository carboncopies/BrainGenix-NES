##########################
# Builds Library For ERS #
##########################

ERSBuildLogger(${Green} "Configuring Meshoptimizer Library")
add_subdirectory(${LIB_DIR}/meshoptimizer)
disable_target_warnings(meshoptimizer)
ERSBuildLogger(${BoldGreen} "Finished Configuring Meshoptimizer Library")