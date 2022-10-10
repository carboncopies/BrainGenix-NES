##########################
# Builds Library For ERS #
##########################

ERSBuildLogger(${Green} "Configuring Python Library")
add_subdirectory(${LIB_DIR}/Python3.9/)
ERSBuildLogger(${BoldGreen} "Finished Configuring Python Library")