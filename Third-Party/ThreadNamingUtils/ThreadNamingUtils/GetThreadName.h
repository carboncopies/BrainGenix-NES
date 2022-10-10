// Simple library to help manage thread names

#pragma once

#include <string>

#if defined(__linux__)
    #include <pthread.h>
#endif

std::string GetThreadName();
void GetThreadName(std::string &ThreadName);
void GetThreadName(std::string *ThreadName);