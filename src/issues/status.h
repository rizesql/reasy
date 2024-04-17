#pragma once

class Status {
public:
    enum Enum {
        backlog, todo, in_progress, done, cancelled
    };

    constexpr Status() = default;

    constexpr Status(Enum e) : enum_(e) {}

    constexpr operator Enum() const { return enum_; }

    explicit operator bool() = delete;

    /// Construct Status from uint; if it's not matched, then returns `std::nullptr`
    [[nodiscard]]
    static std::optional<Status> from(unsigned int str) noexcept {
        if (str == 0) return Status::backlog;
        if (str == 1) return Status::todo;
        if (str == 2) return Status::in_progress;
        if (str == 3) return Status::done;
        if (str == 4) return Status::cancelled;
        else return {};
    }

    /// Construct Status from string; if it's not matched, then returns `std::nullptr`
    [[nodiscard]]
    static std::optional<Status> from(std::string_view str) noexcept {
        if (str == "backlog") return Status::backlog;
        if (str == "todo") return Status::todo;
        if (str == "in_progress") return Status::in_progress;
        if (str == "done") return Status::done;
        if (str == "cancelled") return Status::cancelled;
        else return {};
    }

    /// Convert Status to string
    [[nodiscard]]
    std::string_view to_string() const noexcept {
        if (enum_ == backlog) return "backlog";
        if (enum_ == todo) return "todo";
        if (enum_ == in_progress) return "in_progress";
        if (enum_ == done) return "done";
        if (enum_ == cancelled) return "cancelled";
        else std::unreachable();
    }

    /// Convert Status to uint
    [[nodiscard]]
    uint to_int() const noexcept {
        if (enum_ == backlog) return 0;
        if (enum_ == todo) return 1;
        if (enum_ == in_progress) return 2;
        if (enum_ == done) return 3;
        if (enum_ == cancelled) return 4;
        else std::unreachable();
    }

private:
    Enum enum_;
};

namespace nlohmann {
    template<>
    struct nlohmann::adl_serializer<Status> {
        static void from_json(json const &j, Status s) {
            s = Status::from(j.template get<std::string>()).value_or(Status::todo);
        }

        static void to_json(json &j, Status const &s) {
            j = s.to_string();
        }
    };
}
