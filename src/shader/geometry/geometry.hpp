#ifndef CRIDGEON_SHADER_GEOMETRY_HPP
#define CRIDGEON_SHADER_GEOMETRY_HPP

#include "circle.hpp"
#include "circle_filled.hpp"
#include "polygon.hpp"
#include "polygon_filled.hpp"
#include "line.hpp"
#include "lines.hpp"
#include "texture_quad.hpp"

namespace cridgeon {
namespace Render {
    inline void destroyGeometryShaders() {
        _destroyCircle();
        _destroyCircleFilled();
        _destroyPolygon();
        _destroyPolygonFilled();
        _destroyLine();
        _destroyLines();
        _destroyTextureQuad();
    }
} // namespace Render
} // namespace cridgeon

#endif // CRIDGEON_SHADER_GEOMETRY_HPP