// SPDX-FileCopyrightText: 2021 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef GFX_SHADER_H_
#define GFX_SHADER_H_

#include <cstdint>
#include <optional>
#include <string_view>
#include <utility>

namespace gfx {

class OpenGLShader {
public:
    static std::optional<OpenGLShader> create(std::string_view vertex_src, std::string_view fragment_src);

    OpenGLShader(OpenGLShader const &) = delete;
    OpenGLShader &operator=(OpenGLShader const &) = delete;

    OpenGLShader(OpenGLShader &&o) : program_{std::exchange(o.program_, 0)} {}
    OpenGLShader &operator=(OpenGLShader &&o) {
        program_ = std::exchange(o.program_, 0);
        return *this;
    }

    ~OpenGLShader();

    void use();

    std::uint32_t id() const { return program_; }

private:
    OpenGLShader(std::uint32_t program) : program_{program} {}

    // Really a GLuint, but https://www.khronos.org/opengl/wiki/OpenGL_Type says
    // it's always exactly 32 bits wide, so let's hide the GL header properly.
    std::uint32_t program_{0};
};

} // namespace gfx

#endif
