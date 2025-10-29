#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

extern std::ofstream g_log;

#define LOG(msg) { \
    std::ostringstream oss; oss << "[" << __TIME__ << "] " << msg; \
    g_log << oss.str() << std::endl; \
    g_log.flush(); \
}