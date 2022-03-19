##########################
# Builds Library For NES #
##########################

NESBuildLogger(${Green} "Configuring Backward Library")
add_subdirectory(${LIB_DIR}/backward)
NESBuildLogger(${BoldGreen} "Finished Configuring Backward Library")