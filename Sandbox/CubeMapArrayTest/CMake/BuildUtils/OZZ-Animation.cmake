##########################
# Builds Library For ERS #
##########################

ERSBuildLogger(${Green} "Configuring OZZ-Animation Library")
add_subdirectory(${LIB_DIR}/ozz-animation)
disable_target_warnings(ozz_animation)
ERSBuildLogger(${BoldGreen} "Finished Configuring OZZ-Animation Library")
