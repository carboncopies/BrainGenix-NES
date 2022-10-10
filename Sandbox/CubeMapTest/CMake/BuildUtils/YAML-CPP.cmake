##########################
# Builds Library For ERS #
##########################

ERSBuildLogger(${Green} "Configuring YAML-CPP Library")
add_subdirectory("${LIB_DIR}/yaml-cpp")
disable_target_warnings(yaml-cpp)
ERSBuildLogger(${BoldGreen} "Finished Configuring YAML-CPP Library")