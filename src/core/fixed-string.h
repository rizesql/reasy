#pragma once

#include <string_view>

/// Cloned fixed_string implementation from `grpcxx`, because the package does not expose this class
template<class CharT, std::size_t N>
struct basic_fixed_string {
    using value_type = CharT;

    constexpr basic_fixed_string(const value_type (&str)[N + 1]) noexcept {
        std::copy_n(str, N + 1, value);
    }

    constexpr operator std::basic_string_view<value_type>() const noexcept { return view(); }

    template<std::size_t M>
    constexpr auto operator==(const basic_fixed_string<value_type, M> &rhs) const noexcept {
        return (N == M && view() == rhs.view());
    }

    [[nodiscard]] constexpr const value_type *data() const noexcept { return &value[0]; }

    [[nodiscard]] constexpr std::size_t size() const noexcept { return N; }

    [[nodiscard]] constexpr std::basic_string_view<value_type> view() const noexcept {
        return {&value[0], N};
    }

    // extra space for null terminations when constructing from 'const ChatT *'
    value_type value[N + 1];
};

/// Cloned fixed_string implementation from `grpcxx`, because the package does not expose this class
template<std::size_t N>
struct fixed_string : basic_fixed_string<char, N - 1> {
    constexpr fixed_string(const char (&str)[N]) noexcept:
        basic_fixed_string<char, N - 1>(str) {}
};

/// Cloned fixed_string implementation from `grpcxx`, because the package does not expose this class
template<fixed_string T>
struct fixed_string_t {
};