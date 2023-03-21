// SPDX-FileCopyrightText: 2023 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: BSD-2-Clause

#include "wasm/instructions.h"

#include "wasm/leb128.h"

#include <iomanip>
#include <iostream>
#include <istream>
#include <sstream>

namespace wasm {

std::optional<BlockType> BlockType::parse(std::istream &is) {
    std::uint8_t type{};
    if (!is.read(reinterpret_cast<char *>(&type), sizeof(type))) {
        return std::nullopt;
    }

    constexpr std::uint8_t kEmptyTag = 0x40;
    if (type == kEmptyTag) {
        return BlockType{{BlockType::Empty{}}};
    }

    std::stringstream ss{std::string{1, static_cast<char>(type)}};
    auto value_type = ValueType::parse(ss);
    if (value_type) {
        return BlockType{{*std::move(value_type)}};
    }

    std::cerr << "Unhandled BlockType\n";
    return std::nullopt;
}

std::optional<MemArg> MemArg::parse(std::istream &is) {
    auto a = wasm::Leb128<std::uint32_t>::decode_from(is);
    if (!a) {
        return std::nullopt;
    }

    auto o = wasm::Leb128<std::uint32_t>::decode_from(is);
    if (!o) {
        return std::nullopt;
    }

    return MemArg{.align = *std::move(a), .offset = *std::move(o)};
}

namespace instructions {

std::optional<std::vector<Instruction>> parse(std::istream &is) {
    std::vector<Instruction> instructions{};

    while (true) {
        std::uint8_t opcode{};
        if (!is.read(reinterpret_cast<char *>(&opcode), sizeof(opcode))) {
            return std::nullopt;
        }

        switch (opcode) {
            case Block::opcode: {
                auto type = BlockType::parse(is);
                if (!type) {
                    return std::nullopt;
                }

                auto block_instructions = parse(is);
                if (!block_instructions) {
                    return std::nullopt;
                }

                instructions.emplace_back(Block{*std::move(type), *std::move(block_instructions)});
                break;
            }
            case Loop::opcode: {
                auto type = BlockType::parse(is);
                if (!type) {
                    return std::nullopt;
                }

                auto block_instructions = parse(is);
                if (!block_instructions) {
                    return std::nullopt;
                }

                instructions.emplace_back(Block{*std::move(type), *std::move(block_instructions)});
                break;
            }
            case BreakIf::opcode: {
                auto value = wasm::Leb128<std::uint32_t>::decode_from(is);
                if (!value) {
                    return std::nullopt;
                }
                instructions.emplace_back(BreakIf{*value});
                break;
            }
            case Return::opcode:
                instructions.emplace_back(Return{});
                break;
            case End::opcode:
                return instructions;
            case I32Const::opcode: {
                auto value = wasm::Leb128<std::int32_t>::decode_from(is);
                if (!value) {
                    return std::nullopt;
                }
                instructions.emplace_back(I32Const{*value});
                break;
            }
            case I32LessThanSigned::opcode:
                instructions.emplace_back(I32LessThanSigned{});
                break;
            case I32Add::opcode:
                instructions.emplace_back(I32Add{});
                break;
            case LocalGet::opcode: {
                auto value = wasm::Leb128<std::uint32_t>::decode_from(is);
                if (!value) {
                    return std::nullopt;
                }
                instructions.emplace_back(LocalGet{*value});
                break;
            }
            case LocalSet::opcode: {
                auto value = wasm::Leb128<std::uint32_t>::decode_from(is);
                if (!value) {
                    return std::nullopt;
                }
                instructions.emplace_back(LocalSet{*value});
                break;
            }
            case LocalTee::opcode: {
                auto value = wasm::Leb128<std::uint32_t>::decode_from(is);
                if (!value) {
                    return std::nullopt;
                }
                instructions.emplace_back(LocalTee{*value});
                break;
            }
            case I32Load::opcode: {
                auto arg = MemArg::parse(is);
                if (!arg) {
                    return std::nullopt;
                }

                instructions.emplace_back(I32Load{*std::move(arg)});
                break;
            }
            default:
                std::cerr << "Unhandled opcode 0x" << std::setw(2) << std::setfill('0') << std::hex << +opcode;
                return std::nullopt;
        }
    }
}

} // namespace instructions
} // namespace wasm
