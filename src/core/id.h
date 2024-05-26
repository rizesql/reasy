#pragma once

#include "xid/xid.h"
#include "fixed-string.h"

/// Strongly typed id to avoid primitive obsession; internally uses xid::id
template<fixed_string T>
class tid {
private:
    xid::id val_;

public:
    explicit tid() : val_{xid::next()} {}

    explicit tid(xid::id val) : val_{val} {}

    [[nodiscard]]
    xid::id val() const noexcept {
        return val_;
    }

    bool operator==(const tid<T> &rhs) const {
        return val_ == rhs.val_;
    }

    bool operator!=(const tid<T> &rhs) const {
        return rhs != *this;
    }
};
