#include "Utility/Assert.hpp"
#include "Utility/Trace.hpp"

int main()
{
    TRACE("This is a test debug message, %u", 5);

    ASSERT(true);
    ASSERT_MSG(true, "This is an assert message");

    TRACE_CH(Error, "This is a second test debug message");

    ASSERT_FAIL_MSG("This is an intentionally failing assert!");
}
