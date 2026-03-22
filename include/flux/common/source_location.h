#pragma once
#include <cstdint>
#include <string>

namespace flux {
    struct SourceLocation {
        std::string filepath = "";
        uint32_t    line     = 0;
        uint32_t    col      = 0;
    };
}
