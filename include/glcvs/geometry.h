/**
 * \file
 * \brief Geometry needed for canvas-like drawing. 
 */
#ifndef GLCVS_GEOMETRY
#define GLCVS_GEOMETRY

#include <stddef.h>
#include <stdbool.h>

/// A point in 2D space.
typedef struct {
  float x, y;
} glcvs_point;

/// \brief Color defenition.
/// Uses GL color format (components from 0 to 1).
typedef struct {
  float r, g, b;
} glcvs_color;

/*
 * TODO: clipping and path drawing
 *
/// \brief Compute how many trianges will be in triangulation
///        of polygon with given number of points.
///
/// \param num_points Number of points in polygon to be triangulated
size_t glcvs_triags_in_triangulation(size_t num_points);

/// \brief Triangulate given polygon with ear-clipping algorithm.
///
/// Polygon must be non self-intersecting.
/// If you want to allocate array for indices, use `triags_in_triangulation()`
/// function.
///
/// \param points Array of `num_points` points, forming given polygon.
/// \param num_points Number of points in given polygon.
/// \param [out] indices Output for resulting triangles. They are given in packs of
///                      3 indices for each triangle.
bool glcvs_triangulate(glcvs_point* points, size_t num_points, unsigned* indices);

*/

#endif // GLCVS_GEOMETRY
