// Simple library to help manage thread names

#include <SetThreadName.h>



#ifdef _WIN32

void SetThreadName(std::string ThreadName) {
    SetThreadName(ThreadName.c_str());
}
void SetThreadName(const char* ThreadName) {
    //SetThreadName(GetCurrentThreadId(),ThreadName);
}

#elif defined(__linux__)

void SetThreadName(std::string ThreadName) {
    SetThreadName(ThreadName.c_str());
}
void SetThreadName(const char* ThreadName) {
    prctl(PR_SET_NAME,ThreadName,0,0,0);
}

#endif