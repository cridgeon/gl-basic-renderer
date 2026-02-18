#ifndef CRIDGEON_SHADER_POLYGON_HPP
#define CRIDGEON_SHADER_POLYGON_HPP

#include <vector>

namespace cridgeon {
namespace Render {
    void polygon(const std::vector<float>& vertices, float r, float g, float b, float a);
    void _destroyPolygon();
} // namespace Render
} // namespace cridgeon

#endif // CRIDGEON_SHADER_POLYGON_HPP
