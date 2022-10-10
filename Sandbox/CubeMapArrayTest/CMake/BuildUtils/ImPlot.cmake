##########################
# Builds Library For ERS #
##########################

ERSBuildLogger(${Green} "Configuring ImPlot Library")
add_subdirectory(${LIB_DIR}/implot)
ERSBuildLogger(${BoldGreen} "Finished Configuring ImPlot Library")