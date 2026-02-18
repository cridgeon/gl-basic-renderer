#ifndef CRIDGEON_SHADER_POLYGON_FILLED_HPP
#define CRIDGEON_SHADER_POLYGON_FILLED_HPP

#include <vector>

namespace cridgeon {
namespace Render {
    void polygonFilled(const std::vector<float>& vertices, float r, float g, float b, float a);
    void _destroyPolygonFilled();
} // namespace Render
} // namespace cridgeon

#endif // CRIDGEON_SHADER_POLYGON_FILLED_HPP
