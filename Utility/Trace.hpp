#pragma once

#include <cstdint>
#include <cstdio>

namespace Trace {

#define TRACE(format, ...) \
    ::Trace::Trace(::Trace::Channel::Debug, __FILE__, __LINE__, (format), __VA_ARGS__)

#define TRACE_CH(channel, format, ...) \
    ::Trace::Trace((::Trace::Channel::channel), __FILE__, __LINE__, (format), __VA_ARGS__)

#define TRACE_CH_VAR(channel, format, ...) \
    ::Trace::Trace(channel, __FILE__, __LINE__, (format), __VA_ARGS__)

struct Channel
{
    enum Enum
    {
        Debug,
        Notice,
        Warning,
        Error,
        Fatal
    };
};

template <typename ... Args>
void Trace(Channel::Enum channel, const char* file, int line, const char* format, Args ... args);

std::uint32_t InternalGetThreadId();
void InternalGetSystemTime(std::uint8_t& hour, std::uint8_t& minute,
    std::uint8_t& second, std::uint16_t& millisecond);
void InternalOutputDebugString(const char* str);

#include "Utility/Trace.inl"

}
