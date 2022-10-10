##########################
# Builds Library For ERS #
##########################

ERSBuildLogger(${Green} "Configuring Lucifer Library")
add_subdirectory(${LIB_DIR}/LuciferIL/LuciferIL)
disable_target_warnings(Lucifer)
ERSBuildLogger(${BoldGreen} "Finished Configuring Lucifer Library")
