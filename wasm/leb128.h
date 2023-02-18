// SPDX-FileCopyrightText: 2023 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef WASM_LEB128_H_
#define WASM_LEB128_H_

#include <cmath>
#include <concepts>
#include <cstdint>
#include <istream>
#include <optional>

namespace wasm {

// https://webassembly.github.io/spec/core/bikeshed/#binary-int
template<typename T>
requires std::integral<T>
struct Leb128 {
    // clang-format-14 adds a newline between the braces and clang-format-15
    // removes it. This comment keeps the formatting stable until we update to
    // clang-format-15.
};

// https://en.wikipedia.org/wiki/LEB128#Decode_unsigned_integer
template<std::unsigned_integral T>
struct Leb128<T> {
    static std::optional<T> decode_from(std::istream &&is) { return decode_from(is); }
    static std::optional<T> decode_from(std::istream &is) {
        T result{};
        std::uint8_t shift{};
        auto const max_bytes = static_cast<int>(std::ceil(sizeof(T) * 8 / 7.f));
        for (int i = 0; i < max_bytes; ++i) {
            std::uint8_t byte{};
            if (!is.read(reinterpret_cast<char *>(&byte), sizeof(byte))) {
                return std::nullopt;
            }

            if (i == max_bytes - 1) {
                // This is the last byte we'll read. Check that any extra bits are all 0.
                auto remaining_value_bits = sizeof(T) * 8 - (max_bytes - 1) * 7;
                auto extra_bits_mask = (0xff << remaining_value_bits) & 0b0111'1111;
                auto extra_bits = byte & extra_bits_mask;
                if (extra_bits != 0) {
                    return std::nullopt;
                }
            }

            result |= static_cast<T>(byte & 0b0111'1111) << shift;
            if (!(byte & 0b1000'0000)) {
                return result;
            }

            shift += 7;
        }

        return std::nullopt;
    }
};

} // namespace wasm

#endif
