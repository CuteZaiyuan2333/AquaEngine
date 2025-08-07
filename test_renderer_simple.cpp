#include "Core/Renderer/Renderer.h"
#include <iostream>

int main() {
    std::cout << "Testing Renderer header inclusion..." << std::endl;
    
    // Test if we can create UniformBufferObject
    Aqua::Renderer::UniformBufferObject ubo;
    
    std::cout << "UniformBufferObject created successfully" << std::endl;
    return 0;
}