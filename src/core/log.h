#pragma once

#include "spdlog/spdlog.h"
#include "fmt/color.h"

class log {
public:
    static void init() {
        spdlog::set_pattern("[%Y-%m-%dT%X.%eZ] %^%l%$ on %P: %v");
        spdlog::set_error_handler([](std::string const &msg) {
            spdlog::get("console")->error("ERROR: {}", msg);
        });
    }

    template<typename S>
    static std::basic_string<char> label(S str) {
        return fmt::format(
            fmt::fg(fmt::terminal_color::bright_blue) |
            fmt::emphasis::bold,
            str);
    }
};
