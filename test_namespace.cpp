#include "Core/Renderer/Renderer.h"
#include <iostream>

namespace Aqua {
namespace Renderer {

void TestFunction() {
    UniformBufferObject ubo;
    std::cout << "UniformBufferObject created successfully" << std::endl;
}

} // namespace Renderer
} // namespace Aqua

int main() {
    Aqua::Renderer::TestFunction();
    return 0;
}