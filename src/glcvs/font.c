#include <GL/glew.h>
#include <GL/gl.h>
#include <assert.h>
#include "glcvs/font.h"
#include "glcvs/buffers.h"
#include "glcvs/builtin-font.h"
#include "glcvs/geometry.h"

#define STRING_END 0
#define BAD_CHARACTER 1

// Code written by me for an older project

// Return CNT last bytes of C shifted by OFFSET to the left.
//
//  pack(0b1011, 2, 3) -> 0b11000
//           ^^             ^^
static unsigned pack(char c, size_t cnt, size_t offset) {
  unsigned mask = (1 << cnt) - 1;
  return (unsigned)((unsigned)(c) & mask) << offset;
}

static const char* extract_utf8_codepoint(const char* text, unsigned* output) {

  assert(text);

  unsigned cp_length;
  unsigned char ch = text[0];

  if ((ch >> 7) == 0) cp_length = 1;
  else if ((ch >> 5) == 0b110) cp_length = 2;
  else if ((ch >> 4) == 0b1110) cp_length = 3;
  else if ((ch >> 3) == 0b11110) cp_length = 4;
  else { // Bad byte
    if (output)
      *output = BAD_CHARACTER;
    return text + 1;
  }

  if (output) {
    if (cp_length == 1)
      *output = *text;
    else if (cp_length == 2)
      *output = pack(*(text+1), 6, 0)
                + pack(*(text  ), 5, 6);
    else if (cp_length == 3)
      *output = pack(*(text+2), 6, 0)
                + pack(*(text+1), 6, 6)
                + pack(*(text  ), 4, 12);
    else if (cp_length == 4)
      *output = pack(*(text+3), 6, 0)
                + pack(*(text+2), 6, 6)
                + pack(*(text+1), 6, 12)
                + pack(*(text  ), 3, 18);
  }

  return text + cp_length;
}

void glcvs_fill_text(glcvs_ctx* ctx, float x, float y, const char* text) {

  assert(ctx);

  glcvs_buf_begin_new_shape(ctx);
  
  glBindTexture(GL_TEXTURE_2D, ctx->_backend.font_atlas_tex);
  glUniform1i(ctx->_backend.shader.attr_use_texture, 1);

  const glcvs_embeded_font fnt = glcvs_embeded_font_builtin_13();

  while(1) {

    unsigned cp;
    text = extract_utf8_codepoint(text, &cp);

    if (cp == STRING_END)
      break;
    else if (cp == BAD_CHARACTER)
      cp = 0; // character not in font, so it will fallback


    // a --- b
    // |     |
    // d --- c

    const glcvs_embeded_glyph_data glyph = fnt.glyphs[fnt.index_for_glyph(cp)];

    if (glyph.tex_h && glyph.tex_w) {
      float top = y - fnt.above_baseline + glyph.y_off,
            left = x + glyph.x_off;

      float tex_x = glyph.tex_x * 1.0 / fnt.atlas->width,
            tex_y = glyph.tex_y * 1.0 / fnt.atlas->height,
            tex_w = glyph.tex_w * 1.0 / fnt.atlas->width,
            tex_h = glyph.tex_h * 1.0 / fnt.atlas->height;


      unsigned a = glcvs_buf_add_vertex(ctx, (_glcvs_vertex) {
                      .pos = (glcvs_point){left, top},
                      .uv = (glcvs_point){tex_x, tex_y}
                   }),
               b = glcvs_buf_add_vertex(ctx, (_glcvs_vertex) {
                      .pos = (glcvs_point){left+glyph.tex_w, top},
                      .uv = (glcvs_point){ tex_x + tex_w, tex_y }
                   }),
               c = glcvs_buf_add_vertex(ctx, (_glcvs_vertex) {
                      .pos = (glcvs_point){left+glyph.tex_w, top+glyph.tex_h},
                      .uv = (glcvs_point){ tex_x + tex_w, tex_y + tex_h }
                   }),
               d = glcvs_buf_add_vertex(ctx, (_glcvs_vertex) {
                      .pos = (glcvs_point){left, top+glyph.tex_h},
                      .uv = (glcvs_point){ tex_x, tex_y + tex_h }
                   });

      glcvs_buf_add_triangle(ctx, a, b, c);
      glcvs_buf_add_triangle(ctx, c, d, a);
    }

    x += glyph.advance;
  }

  glcvs_buf_draw(ctx);
}


float glcvs_text_width(glcvs_ctx* ctx, const char* text) {

  assert(ctx);

  // This could use font style from ctx, but that will be later
  // For now this requires not needed ctx argument.

  const glcvs_embeded_font fnt = glcvs_embeded_font_builtin_13();

  float w = 0;

  while(1) {
    unsigned cp;
    text = extract_utf8_codepoint(text, &cp);

    if (cp == STRING_END)
      break;
    else if (cp == BAD_CHARACTER)
      cp = 0; // character not in font, so it will fallback

    w += fnt.glyphs[fnt.index_for_glyph(cp)].advance;
  }

  return w;
}

glcvs_text_mertics glcvs_get_text_metrics() {
  const glcvs_embeded_font fnt = glcvs_embeded_font_builtin_13();
  const glcvs_embeded_glyph_data plus_sign = fnt.glyphs[fnt.index_for_glyph('+')];

  return (glcvs_text_mertics) {
    .ascent = (float) fnt.above_baseline,
    .descent = (float) (fnt.line_height - fnt.above_baseline),
    .fraction_line = (float) fnt.above_baseline - (float) plus_sign.y_off - (float) plus_sign.tex_h / 2.0f
  };
}
