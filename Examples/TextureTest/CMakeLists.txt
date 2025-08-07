# TextureTest示例程序

# 收集源文件
set(TEXTURE_TEST_SOURCES
    main.cpp
)

# 创建可执行文件
add_executable(TextureTest ${TEXTURE_TEST_SOURCES})

# 设置目标属性
set_target_properties(TextureTest PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# 链接依赖
target_link_libraries(TextureTest 
    PRIVATE 
        AquaCore
)

# 包含目录
target_include_directories(TextureTest 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

# 设置输出目录
set_target_properties(TextureTest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 复制着色器文件到输出目录
if(EXISTS ${CMAKE_SOURCE_DIR}/Assets/Shaders)
    add_custom_command(TARGET TextureTest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Assets/Shaders
        $<TARGET_FILE_DIR:TextureTest>/Shaders
        COMMENT "Copying shaders to output directory"
    )
endif()

# 在Windows上，确保DLL在正确的位置
if(WIN32)
    # 如果使用动态链接的Vulkan，可能需要复制DLL
    # 这里暂时不需要，因为我们使用静态链接
endif()