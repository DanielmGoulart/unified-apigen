#include "Utility/Assert.hpp"
#include "Utility/Trace.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#if defined(OS_WINDOWS)
    #include "Windows.h"
#endif

namespace Assert {

static bool g_AssertTestMode = false;
static bool g_AssertHit = true;

void Fail(const char* condition, const char* file, int line, const char* message)
{
    char buffer[2048];

    if (condition)
    {
        std::sprintf(buffer, "ASSERTION FAILURE\n  Summary: (%s) failed at (%s:%d)", condition, file, line);
    }
    else
    {
        std::sprintf(buffer, "ASSERTION FAILURE\n  Summary: Failed at (%s:%d)", file, line);
    }

    if (message)
    {
        std::strcat(buffer, "\n  Message: ");
        std::strcat(buffer, message);
    }

#if defined(OS_WINDOWS)
    void* stackTrace[20];
    int numCapturedFrames = CaptureStackBackTrace(0, 20, stackTrace, NULL);

    if (numCapturedFrames)
    {
        std::strcat(buffer, "\n  Backtrace:\n");
        for (int i = 0; i < numCapturedFrames; ++i)
        {
            char backtraceBuffer[32];
            std::sprintf(backtraceBuffer, "    0x%p\n", stackTrace[i]);
            std::strcat(buffer, backtraceBuffer);
        }
    }
#endif // OS_WINDOWS

    TRACE_CH(Fatal, buffer);

    g_AssertHit = true;

    bool skipCrash = false;
    bool skipBreak = false;

    if (!g_AssertTestMode)
    {
#if defined(OS_WINDOWS)
        int response = MessageBox(GetActiveWindow(), buffer, "ASSERTION FAILURE", MB_ABORTRETRYIGNORE);

        switch (response)
        {
        case IDRETRY: // No crash, but break.
            skipCrash = true;
            break;

        case IDIGNORE: // No crash or break.
            skipCrash = true;
            skipBreak = true;
            break;

        case IDABORT:
        default: // Crash and break
            break;
        }
#endif // OS_WINDOWS

        if (!skipBreak)
        {
#if defined(CMP_MSVC)
            __debugbreak();
#endif
        }

        if (!skipCrash)
        {
            std::abort();
        }
    }
}

void TestBegin()
{
    g_AssertTestMode = true;
    g_AssertHit = false;
}

bool TestSuccess()
{
    bool hit = g_AssertHit;
    g_AssertHit = false;
    return !hit;
}

}
