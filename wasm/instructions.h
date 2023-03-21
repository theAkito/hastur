// SPDX-FileCopyrightText: 2023 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef WASM_INSTRUCTIONS_H_
#define WASM_INSTRUCTIONS_H_

#include "wasm/wasm.h"

#include <cstdint>
#include <iosfwd>
#include <optional>
#include <variant>
#include <vector>

namespace wasm {

struct BlockType {
    static std::optional<BlockType> parse(std::istream &);

    struct Empty {};
    std::variant<Empty, ValueType, TypeIdx> value;
};

struct MemArg {
    static std::optional<MemArg> parse(std::istream &);

    std::uint32_t align{};
    std::uint32_t offset{};
};

} // namespace wasm

namespace wasm::instructions {

struct Block;
struct Loop;
struct BreakIf;
struct Return;

struct I32Const;
struct I32LessThanSigned;
struct I32Add;

struct LocalGet;
struct LocalSet;
struct LocalTee;

struct I32Load;

// clang-format off
using Instruction = std::variant<Block,
        Loop,
        BreakIf,
        Return,
        I32Const,
        I32LessThanSigned,
        I32Add,
        LocalGet,
        LocalSet,
        LocalTee,
        I32Load>;
// clang-format on

// https://webassembly.github.io/spec/core/binary/instructions.html#control-instructions
struct Block {
    static constexpr std::uint8_t opcode = 0x02;
    BlockType type{};
    std::vector<Instruction> instructions;
};

struct Loop {
    static constexpr std::uint8_t opcode = 0x03;
    BlockType type{};
    std::vector<Instruction> instructions;
};

struct BreakIf {
    static constexpr std::uint8_t opcode = 0x0d;
    std::uint32_t label_idx{};
};

struct Return {
    static constexpr std::uint8_t opcode = 0x0f;
};

struct End {
    static constexpr std::uint8_t opcode = 0x0b;
};

// https://webassembly.github.io/spec/core/binary/instructions.html#numeric-instructions
struct I32Const {
    static constexpr std::uint8_t opcode = 0x41;
    std::int32_t value{};
};

struct I32LessThanSigned {
    static constexpr std::uint8_t opcode = 0x48;
};

struct I32Add {
    static constexpr std::uint8_t opcode = 0x6a;
};

// https://webassembly.github.io/spec/core/binary/instructions.html#variable-instructions
struct LocalGet {
    static constexpr std::uint8_t opcode = 0x20;
    std::uint32_t idx{};
};

struct LocalSet {
    static constexpr std::uint8_t opcode = 0x21;
    std::uint32_t idx{};
};

struct LocalTee {
    static constexpr std::uint8_t opcode = 0x22;
    std::uint32_t idx{};
};

// https://webassembly.github.io/spec/core/binary/instructions.html#memory-instructions
struct I32Load {
    static constexpr std::uint8_t opcode = 0x28;
    MemArg arg{};
};
std::optional<std::vector<Instruction>> parse(std::istream &);

} // namespace wasm::instructions

#endif
