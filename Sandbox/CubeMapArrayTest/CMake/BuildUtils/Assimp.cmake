##########################
# Builds Library For ERS #
##########################

ERSBuildLogger(${Green} "Configuring Assimp Library")
add_subdirectory(${LIB_DIR}/assimp)
disable_target_warnings(assimp)
ERSBuildLogger(${BoldGreen} "Finished Configuring Assimp Library")