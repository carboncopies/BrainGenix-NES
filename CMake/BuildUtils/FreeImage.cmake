##########################
# Builds Library For ERS #
##########################

ERSBuildLogger(${Green} "Configuring FreeImage Library")
add_subdirectory(${LIB_DIR}/FreeImage/FreeImage)
disable_target_warnings(FreeImage)
ERSBuildLogger(${BoldGreen} "Finished Configuring FreeImage Library")
