// Simple library to help manage thread names

#pragma once

#include <string>



// Based off of: https://stackoverflow.com/questions/10121560/stdthread-naming-your-thread
#ifdef _WIN32
// #define NOMINMAX
// #include <windows.h>

void SetThreadName(std::string ThreadName);
void SetThreadName(const char* ThreadName);

#elif defined(__linux__)
#include <sys/prctl.h>

void SetThreadName(std::string ThreadName);
void SetThreadName(const char* ThreadName);

#endif