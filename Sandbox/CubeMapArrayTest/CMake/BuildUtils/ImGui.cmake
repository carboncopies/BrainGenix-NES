##########################
# Builds Library For ERS #
##########################

ERSBuildLogger(${Green} "Configuring ImGui Library")
add_subdirectory(${LIB_DIR}/imgui)
ERSBuildLogger(${BoldGreen} "Finished Configuring ImGui Library")