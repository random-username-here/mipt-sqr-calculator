/**
 * \file
 * \brief Simple font rendering
 */
#ifndef GLCVS_FONT
#define GLCVS_FONT

#include "glcvs/context.h"

/// Font atlas, as embeded into source code
typedef struct {

  /// Image width & height
  int width, height;

  /// Bitmap data
  const unsigned char data[];

} glcvs_embeded_font_atlas;

/// Data of one glyph inside embeded font
typedef struct {
  /// Rectangle in atlas containing said glyph
  int tex_x, tex_y, tex_w, tex_h;
  
  /// Offset relative to current X position
  int x_off;

  /// Offset from top of the line to the top of the glyph
  int y_off;

  /// Advance of this glyph
  float advance;

} glcvs_embeded_glyph_data;

/// Data of the embeded font
typedef struct {

  /// Height of the line (ascent + descent)
  size_t line_height;

  /// Number of pixels above the baseline (ascent)
  size_t above_baseline;

  /// Function which gets glyph index for given unicode codepoint,
  /// which you may use to index array of glyphs
  unsigned (*index_for_glyph)(unsigned);

  /// Data of the glyphs, like advance and location in atlas
  const glcvs_embeded_glyph_data *glyphs;

  /// Atlas texture data
  const glcvs_embeded_font_atlas *atlas;

} glcvs_embeded_font;

/// \brief Render text in built-in bitmap font.
/// \param ctx Canvas to render onto
/// \param text Text to render
void glcvs_fill_text(glcvs_ctx* ctx, float x, float y, const char* text);

typedef struct {
  float ascent, descent;
  // Ascent of center of `+`. On this height fraction line is placed.
  float fraction_line;
} glcvs_text_mertics;

/// \brief Compute width of given text
/// \param ctx Canvas that text will be drawn onto
/// \param text Text you wish to compute width of
float glcvs_text_width(glcvs_ctx* ctx, const char* text);

/// \brief Get common text metrics for built-in font
glcvs_text_mertics glcvs_get_text_metrics();


#endif
