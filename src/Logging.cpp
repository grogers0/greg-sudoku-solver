#include "Logging.hpp"
#include <cstdio>
#include <cstdarg>

namespace {
    LogLevel level = Info;
    bool shouldPrintLogLevel = false;
    bool shouldQuietlyBifurcate = false;
}

void SetLogLevel(LogLevel newLevel)
{
    level = newLevel;
}

LogLevel QuietlyBifurcate()
{
    if (!shouldQuietlyBifurcate)
        return level;

    LogLevel oldLevel = level;
    level = Fatal;
    return oldLevel;
}

void SetShouldPrintLogLevel(bool x)
{
    shouldPrintLogLevel = x;
}

void SetShouldQuietlyBifurcate(bool x)
{
    shouldQuietlyBifurcate = x;
}

const char *GetLogLevelName(LogLevel level)
{
    switch (level)
    {
        case Fatal: return "Fatal";
        case Error: return "Error";
        case Warning: return "Warning";
        case Info: return "Info";
        case Debug: return "Debug";
        case Trace: return "Trace";
        default: return "Unknown";
    }
}

int Log(LogLevel lvl, const char *fmt, ...)
{
    if (level < lvl)
        return 0;

    va_list va;

    va_start(va, fmt);
    if (shouldPrintLogLevel)
        printf("%s: ", GetLogLevelName(lvl));
    int ret = vprintf(fmt, va);
    va_end(va);

    return ret;
}
