##########################
# Builds Library For ERS #
##########################

ERSBuildLogger(${Green} "Finding Threading Package")
find_package(Threads REQUIRED)
ERSBuildLogger(${BoldGreen} "Found Threading Package")