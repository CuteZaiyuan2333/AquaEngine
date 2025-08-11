#include <iostream>

int main() {
    std::cout << "=== DEBUG TEST PROGRAM ===" << std::endl;
    std::cout << "Camera position: (5, 3, 5)" << std::endl;
    std::cout << "Camera target: (0, 0, 0)" << std::endl;
    std::cout << "Camera up: (0, 1, 0)" << std::endl;
    std::cout << "FOV: 45 degrees" << std::endl;
    std::cout << "Aspect ratio: 800/600 = 1.33333" << std::endl;
    std::cout << "Near plane: 0.1" << std::endl;
    std::cout << "Far plane: 100" << std::endl;
    std::cout << "Cube size: 2.0" << std::endl;
    std::cout << "=== END DEBUG INFO ===" << std::endl;
    
    std::cout << "Press any key to continue..." << std::endl;
    std::cin.get();
    
    return 0;
}