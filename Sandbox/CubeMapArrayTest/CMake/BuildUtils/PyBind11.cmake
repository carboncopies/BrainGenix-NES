##########################
# Builds Library For ERS #
##########################

ERSBuildLogger(${Green} "Configuring PyBind11 Library")
add_subdirectory(${LIB_DIR}/pybind11)
ERSBuildLogger(${BoldGreen} "Finished Configuring PyBind11 Library")