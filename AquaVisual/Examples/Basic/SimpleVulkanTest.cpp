#include <AquaVisual/AquaVisual.h>
#include <iostream>

int main() {
    std::cout << "=== Simple Vulkan Test ===" << std::endl;
    
    // 初始化 AquaVisual
    if (!AquaVisual::Initialize()) {
        std::cerr << "Failed to initialize AquaVisual!" << std::endl;
        return -1;
    }
    std::cout << "✓ AquaVisual initialized" << std::endl;
    std::cout << "Version: " << AquaVisual::GetVersion() << std::endl;
    
    // 清理
    AquaVisual::Shutdown();
    std::cout << "✓ AquaVisual shutdown" << std::endl;
    
    std::cout << "Test completed successfully!" << std::endl;
    return 0;
}