// SPDX-FileCopyrightText: 2021 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: BSD-2-Clause

#include "gfx/gfx.h"

#include <GL/glew.h>

namespace gfx {

OpenGLPainter::OpenGLPainter() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGLPainter::set_viewport_size(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1.0, 1.0);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void OpenGLPainter::fill_rect(geom::Rect const &rect, Color color) {
    auto translated{rect.translated(translation_x, translation_y)};
    glColor4ub(color.r, color.g, color.b, color.a);
    glRecti(translated.x, translated.y, translated.x + translated.width, translated.y + translated.height);
}

} // namespace gfx
