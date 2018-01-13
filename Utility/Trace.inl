template <typename ... Args>
void Trace(Channel::Enum channel, const char* file, int line, const char* format, Args ... args)
{
    static constexpr char const* s_ChannelStrs[] =
    {
        "DEBUG",
        "NOTICE",
        "WARNING",
        "ERROR",
        "FATAL"
    };

    char formatBuffer[1536];
    std::sprintf(formatBuffer, format, args ...);

    std::uint8_t hour;
    std::uint8_t minute;
    std::uint8_t second;
    std::uint16_t millisecond;
    InternalGetSystemTime(hour, minute, second, millisecond);

    std::uint32_t threadId = InternalGetThreadId();

    char buffer[2048];
    std::sprintf(buffer, "[%02d:%02d:%02d:%04d %s] [0x%x] [%s:%d]: %s\n",
        hour,
        minute,
        second,
        millisecond,
        s_ChannelStrs[channel],
        threadId,
        file,
        line,
        formatBuffer);

    std::printf("%s", buffer);

    InternalOutputDebugString(buffer);
}
