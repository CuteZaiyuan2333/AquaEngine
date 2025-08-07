#include <iostream>
#include <vulkan/vulkan.h>

int main() {
    std::cout << "Testing Vulkan..." << std::endl;
    
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    
    std::cout << "Vulkan extensions available: " << extensionCount << std::endl;
    
    return 0;
}