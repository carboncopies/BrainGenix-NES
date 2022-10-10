#############################
# Configures The OpenXR SDK #
#############################

# OpenXR SDK
ERSBuildLogger(${Green} "Configuring OpenXR SDK")
add_subdirectory(${LIB_DIR}/OpenXR-SDK)
ERSBuildLogger(${BoldGreen} "Finished Configuring OpenXR SDK")

