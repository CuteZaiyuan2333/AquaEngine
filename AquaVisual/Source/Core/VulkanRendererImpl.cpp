#include "AquaVisual/Core/VulkanRendererImpl.h"
#include "AquaVisual/Core/Window.h"
#include "AquaVisual/Core/Camera.h"
#include "AquaVisual/Resources/Mesh.h"
#include "AquaVisual/Resources/Texture.h"
#include "AquaVisual/Core/ShaderManager.h"
#include "AquaVisual/Core/RenderPipeline.h"
#include "AquaVisual/Core/BufferManager.h"
#include <iostream>
#include <array>

namespace AquaVisual {

VulkanRendererImpl::VulkanRendererImpl(const RendererConfig &config)
    : m_config(config), m_currentCamera(nullptr), m_isInitialized(false) {
}

VulkanRendererImpl::~VulkanRendererImpl() { 
    Shutdown(); 
}

bool VulkanRendererImpl::Initialize() {
    std::cout << "Initializing VulkanRenderer..." << std::endl;

    // Create window
    m_window.reset(new Window(m_config.width, m_config.height, m_config.title));
    if (!m_window->Initialize()) {
        std::cerr << "Failed to create window" << std::endl;
        return false;
    }
    std::cout << "Window created successfully" << std::endl;

    // Initialize Vulkan rendering system
    if (!InitializeVulkan()) {
        std::cerr << "Failed to initialize Vulkan" << std::endl;
        return false;
    }

    std::cout << "VulkanRenderer initialized successfully!" << std::endl;
    return true;
}

void VulkanRendererImpl::Shutdown() {
    std::cout << "Shutting down VulkanRenderer..." << std::endl;

    // Cleanup Vulkan resources
    CleanupVulkan();

    if (m_window) {
        m_window->Shutdown();
        m_window.reset();
    }

    std::cout << "VulkanRenderer shutdown complete" << std::endl;
}

bool VulkanRendererImpl::BeginFrame() {
    if (!m_isInitialized) return false;
    
    // Begin frame rendering
    ClearScreen();
    return true;
}

void VulkanRendererImpl::EndFrame() {
    if (!m_isInitialized) return;
    
    // End frame rendering
    SubmitCommands();
}

void VulkanRendererImpl::SetCamera(const Camera &camera) {
    m_currentCamera = &camera;
}

void VulkanRendererImpl::RenderMesh(const Mesh &mesh, const Texture *texture) {
    if (!m_isInitialized) {
        std::cerr << "Renderer not initialized!" << std::endl;
        return;
    }
    
    std::cout << "Rendering mesh with " << mesh.GetVertexCount()
              << " vertices and " << mesh.GetIndexCount() << " indices";
    if (texture) {
        std::cout << " (with texture)";
    }
    std::cout << std::endl;
    
    // Convert Vertex format to SimpleVertex format
    const auto& vertices = mesh.GetVertices();
    const auto& indices = mesh.GetIndices();
    
    // Safety checks
    if (vertices.empty()) {
        std::cerr << "Error: Mesh has no vertices!" << std::endl;
        return;
    }
    
    if (indices.empty()) {
        std::cerr << "Error: Mesh has no indices!" << std::endl;
        return;
    }
    
    // Validate indices
    for (size_t i = 0; i < indices.size(); ++i) {
        if (indices[i] >= vertices.size()) {
            std::cerr << "Error: Index " << indices[i] << " at position " << i 
                      << " is out of range (vertex count: " << vertices.size() << ")" << std::endl;
            return;
        }
    }
    
    std::vector<SimpleVertex> simpleVertices;
    simpleVertices.reserve(vertices.size());
    
    for (const auto& vertex : vertices) {
        SimpleVertex simpleVertex;
        simpleVertex.position[0] = vertex.position.x;
        simpleVertex.position[1] = vertex.position.y;
        simpleVertex.position[2] = vertex.position.z;
        
        // Generate colors based on position or normal
        // Use normal as color for now (normalized to 0-1 range)
        simpleVertex.color[0] = (vertex.normal.x + 1.0f) * 0.5f;
        simpleVertex.color[1] = (vertex.normal.y + 1.0f) * 0.5f;
        simpleVertex.color[2] = (vertex.normal.z + 1.0f) * 0.5f;
        
        simpleVertices.push_back(simpleVertex);
    }
    
    // Create temporary buffers for this mesh
    auto& bufferManager = BufferManager::Instance();
    
    // Check if data is valid before creating buffers
    if (simpleVertices.empty()) {
        std::cerr << "Error: No vertices to render!" << std::endl;
        return;
    }
    
    auto vertexBuffer = bufferManager.CreateVertexBuffer(
        simpleVertices.data(), 
        simpleVertices.size() * sizeof(SimpleVertex)
    );
    
    if (!vertexBuffer) {
        std::cerr << "Error: Failed to create vertex buffer!" << std::endl;
        return;
    }
    
    auto indexBuffer = bufferManager.CreateIndexBuffer(
        indices.data(), 
        static_cast<uint32_t>(indices.size())
    );
    
    if (!indexBuffer) {
        std::cerr << "Error: Failed to create index buffer!" << std::endl;
        return;
    }
    
    // Bind pipeline
    if (m_currentPipeline) {
        BindPipeline(m_currentPipeline);
    }
    
    // Bind vertex buffer
    if (vertexBuffer) {
        BindVertexBuffer(vertexBuffer);
    }
    
    // Bind index buffer
    if (indexBuffer) {
        BindIndexBuffer(indexBuffer);
    }
    
    // Execute draw commands
    DrawIndexed(mesh.GetIndexCount());
}

void VulkanRendererImpl::Clear(float r, float g, float b, float a) {
    m_clearColor[0] = r;
    m_clearColor[1] = g;
    m_clearColor[2] = b;
    m_clearColor[3] = a;
}

bool VulkanRendererImpl::ShouldClose() const {
    return m_window ? m_window->ShouldClose() : true;
}

void VulkanRendererImpl::PollEvents() {
    if (m_window) {
        m_window->PollEvents();
    }
}

void VulkanRendererImpl::GetWindowSize(uint32_t &width, uint32_t &height) const {
    if (m_window) {
        m_window->GetSize(width, height);
    } else {
        width = m_config.width;
        height = m_config.height;
    }
}

// Vulkan rendering system implementation
bool VulkanRendererImpl::InitializeVulkan() {
    std::cout << "Initializing Vulkan rendering system..." << std::endl;
    
    // Initialize shader manager
    InitializeShaderManager();
    
    // Create basic render pipeline
    CreateBasicPipeline();
    
    // Create test geometry
    CreateTestGeometry();
    
    m_isInitialized = true;
    std::cout << "Vulkan rendering system initialized successfully" << std::endl;
    return true;
}

void VulkanRendererImpl::CleanupVulkan() {
    if (!m_isInitialized) return;
    
    std::cout << "Cleaning up Vulkan rendering system..." << std::endl;
    
    // Cleanup buffers
    m_currentVertexBuffer.reset();
    m_currentIndexBuffer.reset();
    m_currentUniformBuffer.reset();
    
    // Cleanup pipeline
    m_currentPipeline.reset();
    
    // Cleanup managers
    BufferManager::Instance().DestroyAllBuffers();
    PipelineManager::Instance().DestroyAllPipelines();
    
    m_isInitialized = false;
    std::cout << "Vulkan rendering system cleanup complete" << std::endl;
}

void VulkanRendererImpl::InitializeShaderManager() {
    std::cout << "Initializing shader manager..." << std::endl;
    
    // Load basic shaders
    auto& shaderManager = ShaderManager::Instance();
    
    // Load vertex shader
    auto vertexShader = shaderManager.LoadShader("simple_vert", "Shaders/simple.vert", ShaderType::Vertex);
    // Load fragment shader
    auto fragmentShader = shaderManager.LoadShader("simple_frag", "Shaders/simple.frag", ShaderType::Fragment);
    
    // Create basic shader program
    auto shaderProgram = shaderManager.CreateProgram("basic");
    if (shaderProgram && vertexShader && fragmentShader) {
        // Add shaders to program
        shaderProgram->AddShader(vertexShader);
        shaderProgram->AddShader(fragmentShader);
        
        // Link the program
        if (shaderProgram->Link()) {
            m_currentShaderProgram = shaderProgram;
            std::cout << "Basic shader program created and linked successfully" << std::endl;
        } else {
            std::cerr << "Failed to link shader program" << std::endl;
        }
    } else {
        std::cerr << "Failed to load shaders or create program" << std::endl;
    }
}

void VulkanRendererImpl::CreateBasicPipeline() {
    std::cout << "Creating basic render pipeline..." << std::endl;
    
    auto& pipelineManager = PipelineManager::Instance();
    
    // Create basic pipeline configuration
    auto pipelineInfo = pipelineManager.CreateBasicPipelineInfo();
    pipelineInfo.name = "BasicPipeline";
    pipelineInfo.shaderProgram = m_currentShaderProgram;
    
    // Set vertex input description
    pipelineInfo.vertexInput = SimpleVertex::GetInputDescription();
    
    // Create pipeline
    m_currentPipeline = pipelineManager.CreatePipeline(pipelineInfo);
    if (m_currentPipeline) {
        std::cout << "Basic render pipeline created successfully" << std::endl;
    } else {
        std::cerr << "Failed to create basic render pipeline" << std::endl;
    }
}

void VulkanRendererImpl::CreateTestGeometry() {
    std::cout << "Creating test geometry..." << std::endl;
    
    // Create a simple triangle
    std::array<SimpleVertex, 3> vertices = {{
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},  // Red
        {{ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},  // Green
        {{ 0.0f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}   // Blue
    }};
    
    std::array<uint32_t, 3> indices = {0, 1, 2};
    
    // Create vertex buffer
    auto& bufferManager = BufferManager::Instance();
    m_currentVertexBuffer = bufferManager.CreateVertexBuffer(
        vertices.data(), 
        vertices.size() * sizeof(SimpleVertex)
    );
    
    // Create index buffer
    m_currentIndexBuffer = bufferManager.CreateIndexBuffer(
        indices.data(), 
        static_cast<uint32_t>(indices.size())
    );
    
    // Create uniform buffer
    struct UniformBufferObject {
        float model[16];
        float view[16];
        float proj[16];
    };
    
    m_currentUniformBuffer = bufferManager.CreateUniformBuffer(sizeof(UniformBufferObject));
    
    if (m_currentVertexBuffer && m_currentIndexBuffer && m_currentUniformBuffer) {
        std::cout << "Test geometry created successfully" << std::endl;
    } else {
        std::cerr << "Failed to create test geometry" << std::endl;
    }
}

void VulkanRendererImpl::ClearScreen() {
    // Placeholder implementation for clearing screen
    std::cout << "Clearing screen with color (" 
              << m_clearColor[0] << ", " 
              << m_clearColor[1] << ", " 
              << m_clearColor[2] << ", " 
              << m_clearColor[3] << ")" << std::endl;
}

void VulkanRendererImpl::SubmitCommands() {
    // Placeholder implementation for submitting render commands
    std::cout << "Submitting render commands" << std::endl;
}

void VulkanRendererImpl::BindPipeline(std::shared_ptr<RenderPipeline> pipeline) {
    if (pipeline) {
        std::cout << "Binding render pipeline: " << pipeline->GetName() << std::endl;
    }
}

void VulkanRendererImpl::BindVertexBuffer(std::shared_ptr<VertexBuffer> buffer) {
    if (buffer) {
        buffer->Bind();
    }
}

void VulkanRendererImpl::BindIndexBuffer(std::shared_ptr<IndexBuffer> buffer) {
    if (buffer) {
        buffer->Bind();
    }
}

void VulkanRendererImpl::DrawIndexed(uint32_t indexCount) {
    std::cout << "Drawing " << indexCount << " indices" << std::endl;
}

}