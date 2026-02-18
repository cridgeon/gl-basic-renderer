#include "line.hpp"
#include "lines.hpp"

#include <vector>

namespace cridgeon {
namespace Render {

    void line(float x1, float y1, float x2, float y2, float r, float g, float b, float a) {
        std::vector<float> vertices = {x1, y1, x2, y2};
        lines(vertices, r, g, b, a);
    }

    void _destroyLine()
    {
    }

} // namespace Render
} // namespace cridgeon