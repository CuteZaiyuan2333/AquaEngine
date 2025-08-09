#include "AquaVisual/AquaVisualMVP.h"
#include "AquaVisual/AquaVisual.h"
#include <iostream>

namespace AquaVisual {

bool InitializeMVP() {
    std::cout << "Initializing AquaVisual MVP..." << std::endl;
    
    // Initialize the base AquaVisual system
    if (!Initialize()) {
        std::cerr << "Failed to initialize base AquaVisual system!" << std::endl;
        return false;
    }
    
    std::cout << "AquaVisual MVP v" << MVPVersion::ToString() << " initialized successfully!" << std::endl;
    return true;
}

void ShutdownMVP() {
    std::cout << "Shutting down AquaVisual MVP..." << std::endl;
    
    // Shutdown the base AquaVisual system
    Shutdown();
    
    std::cout << "AquaVisual MVP shutdown complete!" << std::endl;
}

std::string GetMVPVersion() {
    return MVPVersion::ToString();
}

} // namespace AquaVisual