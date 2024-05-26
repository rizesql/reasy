#pragma once

#include <nlohmann/json.hpp>
#include "id.h"

template<typename T, typename... Ts>
void variant_from_json(const nlohmann::json &j, std::variant<Ts...> &data) {
    try {
        data = j.get<T>();
    } catch (...) {
    }
}

namespace nlohmann {
    template<fixed_string T>
    struct nlohmann::adl_serializer<tid<T>> {
        static void from_json(json const &j, tid<T> &id) {
            id = tid<T>(j.template get<std::string>());
        }

        static void to_json(json &j, tid<T> const &id) {
            j = id.val().string();
        }
    };

    template<typename... Ts>
    struct nlohmann::adl_serializer<std::variant<Ts...>> {
        static void to_json(json &j, std::variant<Ts...> const &var) {
            using nlohmann::to_json;
            std::visit(
                [&](auto &v) { j = v; },
                var);
        }

        static void from_json(json const &j, std::variant<Ts...> &var) {
            (variant_from_json<Ts>(j, var), ...);
        }
    };

    template<typename T>
    struct nlohmann::adl_serializer<std::optional<T>> {
        static void from_json(const json &j, std::optional<T> &opt) {
            if (j.is_null()) {
                opt = std::nullopt;
            } else {
                opt = j.template get<T>();
            }
        }

        static void to_json(json &json, std::optional<T> const t) {
            if (t) {
                json = *t;
            } else {
                json = nullptr;
            }
        }
    };
}
