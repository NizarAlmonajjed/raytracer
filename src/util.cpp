#include "rt/util.h"

namespace rt {

std::string greeting() {
    return "Hello, ray tracer!";
}

int to_byte(double channel) {
    // TODO(you): implement this. See the comment in include/rt/util.h for the
    // exact rules, and tests/util_test.cpp for the cases it must pass.
    // Hint: <algorithm> has std::clamp, and static_cast<int>(x) truncates.
    (void)channel;  // silences the "unused parameter" warning until you use it
    return 0;
}

}  // namespace rt
