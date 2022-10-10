# ERS Internal Library: OpenGLLoggingSystem

## Description

The OpenGLLoggingSystem Collects user configurable information from the driver and logs it when needed. If there are errors or other issues, this subsystem records them. Other modules can use these systems later to display that imformation in the main log if needed. Modules using this subsystem may get a list containing the log items collected here.