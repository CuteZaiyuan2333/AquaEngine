# AquaVisual Integration Examples

# AquaEngine Integration Example
add_executable(AquaEngineIntegration
    AquaEngineIntegration.cpp
)

target_link_libraries(AquaEngineIntegration PRIVATE
    AquaVisual
    ${Vulkan_LIBRARIES}
    glfw
)

target_include_directories(AquaEngineIntegration PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Include
    ${Vulkan_INCLUDE_DIRS}
)

target_compile_features(AquaEngineIntegration PRIVATE cxx_std_17)

# Set output directory
set_target_properties(AquaEngineIntegration PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Integration
)

# Third-Party Integration Example
add_executable(ThirdPartyIntegration
    ThirdPartyIntegration.cpp
)

target_link_libraries(ThirdPartyIntegration PRIVATE
    AquaVisual
    ${Vulkan_LIBRARIES}
    glfw
)

target_include_directories(ThirdPartyIntegration PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Include
    ${Vulkan_INCLUDE_DIRS}
)

target_compile_features(ThirdPartyIntegration PRIVATE cxx_std_17)

# Set output directory
set_target_properties(ThirdPartyIntegration PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Integration
)

# Copy assets for integration examples
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../Assets")
    add_custom_command(TARGET AquaEngineIntegration POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/../../Assets
        ${CMAKE_BINARY_DIR}/bin/Integration/Assets
        COMMENT "Copying assets for AquaEngineIntegration"
    )
    
    add_custom_command(TARGET ThirdPartyIntegration POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/../../Assets
        ${CMAKE_BINARY_DIR}/bin/Integration/Assets
        COMMENT "Copying assets for ThirdPartyIntegration"
    )
endif()

# Add to integration examples group
set_target_properties(AquaEngineIntegration ThirdPartyIntegration PROPERTIES
    FOLDER "Integration Examples"
)