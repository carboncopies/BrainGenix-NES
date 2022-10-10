##########################
# Builds Library For ERS #
##########################

if (WIN32)
    ERSBuildLogger(${Yellow} "Database Support Disabled On Windows Due To SSL Errors, Please Use Linux Instead")
elseif(APPLE)
    ERSBuildLogger(${Yellow} "Database Support Disabled On Darwin Due To SSL Errors, Please Use Linux Instead")
else()
    ERSBuildLogger(${Green} "Configuring MySQL Connector Library")
    add_subdirectory(${LIB_DIR}/mysql-connector-linux)
    ERSBuildLogger(${BoldGreen} "Finished Configuring MySQL Connector Library")
endif()