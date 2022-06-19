// SPDX-FileCopyrightText: 2021 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: BSD-2-Clause

#include "gfx/shader.h"

#include <GL/glew.h>

#include <cassert>
#include <limits>

namespace gfx {

std::optional<OpenGLShader> OpenGLShader::create(std::string_view vertex_src, std::string_view fragment_src) {
    GLint success{0};
    GLint shader_length{0};
    GLchar const *shader_src{nullptr};

    shader_length = static_cast<GLint>(vertex_src.length());
    shader_src = vertex_src.data();
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &shader_src, &shader_length);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glDeleteShader(vertex_shader);
        return std::nullopt;
    }

    shader_length = static_cast<GLint>(fragment_src.length());
    shader_src = fragment_src.data();
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &shader_src, &shader_length);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glDeleteShader(fragment_shader);
        glDeleteShader(vertex_shader);
        return std::nullopt;
    }

    GLuint program = glCreateProgram();
    if (program == 0) {
        glDeleteShader(fragment_shader);
        glDeleteShader(vertex_shader);
        return std::nullopt;
    }

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        glDeleteProgram(program);
        glDeleteShader(fragment_shader);
        glDeleteShader(vertex_shader);
        return std::nullopt;
    }

    glDetachShader(program, fragment_shader);
    glDetachShader(program, vertex_shader);
    glDeleteShader(fragment_shader);
    glDeleteShader(vertex_shader);

    return OpenGLShader{program};
}

OpenGLShader::~OpenGLShader() {
    if (program_ != 0) {
        glDeleteProgram(program_);
    }
}

void OpenGLShader::use() {
    glUseProgram(program_);
}

} // namespace gfx
