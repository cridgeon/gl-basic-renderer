#ifndef CRIDGEON_SHADER_LINES_HPP
#define CRIDGEON_SHADER_LINES_HPP

#include <vector>

namespace cridgeon {
namespace Render {
    void lines(const std::vector<float>& vertices, float r, float g, float b, float a);
    void _destroyLines();
} // namespace Render
} // namespace cridgeon

#endif // CRIDGEON_SHADER_LINES_HPP
