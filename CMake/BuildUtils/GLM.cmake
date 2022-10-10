##########################
# Builds Library For ERS #
##########################

ERSBuildLogger(${Green} "Configuring GLM Library")
add_subdirectory(${LIB_DIR}/glm)
include_directories(${LIB_DIR}/glm)
ERSBuildLogger(${BoldGreen} "Finished Configuring GLM Library")