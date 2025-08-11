#include <iostream>
#include <windows.h>

// Simple Windows API window test
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            EndPaint(hwnd, &ps);
        }
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main() {
    std::cout << "Creating simple Windows API window..." << std::endl;

    // Register window class
    const char CLASS_NAME[] = "AquaVisualTestWindow";
    
    WNDCLASSA wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    if (!RegisterClassA(&wc)) {
        std::cerr << "Failed to register window class" << std::endl;
        return 1;
    }

    // Create window
    HWND hwnd = CreateWindowExA(
        0,                              // Optional window styles
        CLASS_NAME,                     // Window class
        "AquaVisual Window Test",       // Window text
        WS_OVERLAPPEDWINDOW,            // Window style
        CW_USEDEFAULT, CW_USEDEFAULT,   // Position
        800, 600,                       // Size
        nullptr,                        // Parent window
        nullptr,                        // Menu
        GetModuleHandle(nullptr),       // Instance handle
        nullptr                         // Additional application data
    );

    if (hwnd == nullptr) {
        std::cerr << "Failed to create window" << std::endl;
        return 1;
    }

    std::cout << "Window created successfully!" << std::endl;
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    // Message loop
    MSG msg = {};
    std::cout << "Entering message loop..." << std::endl;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    std::cout << "Window closed" << std::endl;
    return 0;
}