##########################
# Builds Library For ERS #
##########################

ERSBuildLogger(${Green} "Configuring ThreadNamingUtils Library")
add_subdirectory(${LIB_DIR}/ThreadNamingUtils)
disable_target_warnings(ThreadNamingUtils)
ERSBuildLogger(${BoldGreen} "Finished Configuring ThreadNamingUtils Library")
