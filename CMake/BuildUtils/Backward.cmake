##########################
# Builds Library For ERS #
##########################

ERSBuildLogger(${Green} "Configuring Backward Library")
if (${ENABLE_BACKWARD})
    add_subdirectory(${LIB_DIR}/backward)
    ERSBuildLogger(${Green} "Backwards Library Enabled, Configuring")
else()
    set(BACKWARD_ENABLE "")
    ERSBuildLogger(${Green} "Backwards Library Disabled, Skipping")
endif()
ERSBuildLogger(${BoldGreen} "Finished Configuring Backward Library")