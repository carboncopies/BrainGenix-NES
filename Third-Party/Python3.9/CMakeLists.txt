 
# Automatically copies python binaries to bin dir on windows
ERSBuildLogger(${Green} "Setting Up Python Libraries")
if (WIN32)
    ERSBuildLogger(${Green} "Copying Python Embedded Library")
    file(COPY "${LIB_DIR}/Python3.9/Binaries/" DESTINATION "${BIN_DIR}/")
    ERSBuildLogger(${BoldGreen} "Finished Copying Python Embedded Library")
else ()
    ERSBuildLogger(${BoldGreen} "Python Libraries Will Not Be Copied On Linux, Please Install Python Using Your Package Manager")
endif()
