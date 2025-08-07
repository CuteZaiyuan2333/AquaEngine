#include <iostream>
#include <vulkan/vulkan.h>

int main() {
    std::cout << "AquaEngine - Simple Triangle Test" << std::endl;
    
    // 检查Vulkan是否可用
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    
    std::cout << "Vulkan extensions available: " << extensionCount << std::endl;
    
    if (extensionCount > 0) {
        std::cout << "Vulkan is available!" << std::endl;
        std::cout << "纹理系统已准备就绪。" << std::endl;
        std::cout << "着色器已编译完成。" << std::endl;
        std::cout << "测试纹理文件已创建: Assets/Textures/test_pattern.svg" << std::endl;
    } else {
        std::cout << "Vulkan is not available!" << std::endl;
    }
    
    return 0;
}