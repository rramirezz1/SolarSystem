#include FT_FREETYPE_H

#include <iostream>

FT_Library ft;
if (FT_Init_FreeType(&ft)) {
    std::cerr << "Erro ao inicializar o FreeType\n";
    return -1;
}

// Carregamento da fonte
FT_Face face;
if (FT_New_Face(ft, "caminho/para/sua/fonte.ttf", 0, &face)) {
    std::cerr << "Erro ao carregar a fonte\n";
    return -1;
}

FT_Set_Pixel_Sizes(face, 0, 48);

struct TextRenderingData {
    GLuint programID;
    GLuint MatrixID;
    glm::mat4 MVP;
    GLuint whiteTextureID;
    GLuint VertexArrayID;
    GLuint vertexbuffer;
};

// Fun��o para renderizar texto
void renderText(const char* text, float x, float y, float scale) {
    glUseProgram(programID);  // Certifique-se de usar o programa correto

    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    setTexture(whiteTextureID, programID);  // Supondo que voc� tenha uma textura branca

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    FT_GlyphSlot g = face->glyph;

    for (const char* p = text; *p; p++) {
        if (FT_Load_Char(face, *p, FT_LOAD_RENDER))
            continue;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, g->bitmap.width, g->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);

        float xpos = x + g->bitmap_left * scale;
        float ypos = y - (g->bitmap.rows - g->bitmap_top) * scale;

        float w = g->bitmap.width * scale;
        float h = g->bitmap.rows * scale;

        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }
        };

        // Renderizar o texto
        glBindVertexArray(VertexArrayID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (g->advance.x >> 6) * scale;
        y += (g->advance.y >> 6) * scale;
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}
