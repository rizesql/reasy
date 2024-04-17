#pragma once

class Priority {
public:
    enum Enum {
        none, urgent, high, medium, low
    };

    constexpr Priority() = default;

    constexpr Priority(Enum e) : enum_(e) {}

    constexpr operator Enum() const { return enum_; }

    explicit operator bool() = delete;

    /// Construct Priority from uint; if it's not matched, then returns `std::nullptr`
    [[nodiscard]]
    static std::optional<Priority> from(uint str) noexcept {
        if (str == 0) return Priority::none;
        if (str == 1) return Priority::urgent;
        if (str == 2) return Priority::high;
        if (str == 3) return Priority::medium;
        if (str == 4) return Priority::low;
        else return {};
    }

    /// Construct Priority from string; if it's not matched, then returns `std::nullptr`
    [[nodiscard]]
    static std::optional<Priority> from(std::string_view str) noexcept {
        if (str == "none") return Priority::none;
        if (str == "urgent") return Priority::urgent;
        if (str == "high") return Priority::high;
        if (str == "medium") return Priority::medium;
        if (str == "low") return Priority::low;
        else return {};
    }

    /// Convert Priority to string
    [[nodiscard]]
    std::string_view to_string() const noexcept {
        if (enum_ == none) return "none";
        if (enum_ == urgent) return "urgent";
        if (enum_ == high) return "high";
        if (enum_ == medium) return "medium";
        if (enum_ == low) return "low";
        else std::unreachable();
    }

    /// Convert Priority to uint
    [[nodiscard]]
    uint to_int() const noexcept {
        if (enum_ == none) return 0;
        if (enum_ == urgent) return 1;
        if (enum_ == high) return 2;
        if (enum_ == medium) return 3;
        if (enum_ == low) return 4;
        else std::unreachable();
    }

private:
    Enum enum_;
};

namespace nlohmann {
    template<>
    struct nlohmann::adl_serializer<Priority> {
        static void from_json(json const &j, Priority p) {
            p = Priority::from(j.template get<std::string>()).value_or(Priority::none);
        }

        static void to_json(json &j, Priority const &p) {
            j = p.to_string();
        }
    };
}
