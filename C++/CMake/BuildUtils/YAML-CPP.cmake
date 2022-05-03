##########################
# Builds Library For NES #
##########################

NESBuildLogger(${Green} "Configuring YAML-CPP Library")
add_subdirectory("${LIB_DIR}/yaml-cpp")
disable_target_warnings(yaml-cpp)
NESBuildLogger(${BoldGreen} "Finished Configuring YAML-CPP Library")