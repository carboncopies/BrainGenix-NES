###############################
# BG Build Versioning System #
###############################

BGBuildLogger(${Green} "Configuring Timestamp Info")

message(" -- Detecting Timestamp Information")

string(TIMESTAMP COMPILE_TIME_STAMP "%Y-%m-%d %Hh%m:%s (UTC)")

message(" -- Configuring File CompileTimeStamp.h.in")
configure_File(${CMAKE_SCRIPTS_DIR}/CompileTimeStamp/CompileTimeStamp.h.in ${SRC_DIR}/Info/Static/CompileTimeStamp/CompileTimeStamp.h)

message(" -- Adding Timestamp Header Target")
add_subdirectory(${SRC_DIR}/Info/Static/CompileTimeStamp)

BGBuildLogger(${BoldGreen} "Finished Configuring Timestamp Info")