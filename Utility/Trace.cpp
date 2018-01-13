#include "Trace.hpp"

#if defined(OS_WINDOWS)
    #include "Windows.h"
#elif defined(OS_LINUX)
	#include <pthread.h>
	#include <time.h>
#endif

namespace Trace {

std::uint32_t InternalGetThreadId()
{
#if defined(OS_WINDOWS)
    return GetCurrentThreadId();
#elif defined(OS_LINUX)
	return pthread_self();
#else
    return 0;
#endif
}

void InternalGetSystemTime(std::uint8_t& hour, std::uint8_t& minute,
    std::uint8_t& second, std::uint16_t& millisecond)
{
	// TODO: There's probably a cross platform std::chrono implementation possible here.

#if defined(OS_WINDOWS)
    SYSTEMTIME time;
    GetSystemTime(&time);
    hour = static_cast<std::uint8_t>(time.wHour);
    minute = static_cast<std::uint8_t>(time.wMinute);
    second = static_cast<std::uint8_t>(time.wSecond);
    millisecond = static_cast<std::uint16_t>(time.wMilliseconds);
#elif defined(OS_LINUX)
	time_t theTime = time(NULL);
	tm theTimeThingy = *localtime(&theTime);

	hour = theTimeThingy.tm_hour;
	minute = theTimeThingy.tm_min;
	second = theTimeThingy.tm_sec;
	millisecond = 0;
#else
    hour = 0;
    minute = 0;
    second = 0;
    millisecond = 0;
#endif
}

void InternalOutputDebugString(const char* str)
{
#if defined(OS_WINDOWS)
    OutputDebugStringA(str);
#else
    // TODO - nothing special here?
#endif
}

}