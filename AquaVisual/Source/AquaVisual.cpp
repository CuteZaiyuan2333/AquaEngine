#include "../Include/AquaVisual/AquaVisual.h"
#include <iostream>
#include <memory>

namespace AquaVisual {

static bool g_initialized = false;

bool Initialize() {
    if (g_initialized) {
        std::cout << "AquaVisual already initialized" << std::endl;
        return true;
    }
    
    std::cout << "Initializing AquaVisual..." << std::endl;
    g_initialized = true;
    std::cout << "AquaVisual initialized successfully" << std::endl;
    return true;
}

void Shutdown() {
    if (!g_initialized) {
        return;
    }
    
    std::cout << "Shutting down AquaVisual..." << std::endl;
    g_initialized = false;
    std::cout << "AquaVisual shutdown complete" << std::endl;
}

std::string GetVersion() {
    return "0.1.0-alpha";
}

}