# AquaVisual API 参考文档

本文档提供了 AquaVisual 渲染引擎的完整 API 参考。

## 目录

- [核心 API](#核心-api)
- [渲染 API](#渲染-api)
- [数学库](#数学库)
- [资源管理](#资源管理)
- [事件系统](#事件系统)

## 核心 API

### 命名空间: AquaVisual

#### 库管理函数

```cpp
bool Initialize();
```
初始化 AquaVisual 库。必须在使用任何其他 API 之前调用。

**返回值**: 成功返回 `true`，失败返回 `false`

---

```cpp
void Shutdown();
```
关闭 AquaVisual 库，清理所有资源。

---

```cpp
const char* GetVersion();
```
获取 AquaVisual 版本字符串。

**返回值**: 版本字符串，格式为 "major.minor.patch"

---

```cpp
bool IsVulkanAvailable();
```
检查系统是否支持 Vulkan。

**返回值**: 支持返回 `true`，不支持返回 `false`

---

```cpp
std::vector<std::string> GetAvailableExtensions();
```
获取可用的 Vulkan 扩展列表。

**返回值**: 扩展名称的字符串向量

#### 渲染器创建

```cpp
struct RendererConfig {
    uint32_t width = 800;           // 窗口宽度
    uint32_t height = 600;          // 窗口高度
    std::string title = "AquaVisual"; // 窗口标题
    bool enableValidation = false;   // 启用 Vulkan 验证层
    bool enableVSync = true;         // 启用垂直同步
    uint32_t msaaSamples = 1;       // MSAA 采样数 (1, 2, 4, 8)
    uint32_t maxFramesInFlight = 2; // 最大飞行帧数
};
```

---

```cpp
std::unique_ptr<RenderAPI> CreateRenderer(const RendererConfig& config);
```
创建渲染器实例。

**参数**:
- `config`: 渲染器配置

**返回值**: 渲染器实例的智能指针，失败返回 `nullptr`

## 渲染 API

### 类: RenderAPI

#### 生命周期管理

```cpp
bool Initialize();
```
初始化渲染器。

**返回值**: 成功返回 `true`，失败返回 `false`

---

```cpp
void Shutdown();
```
关闭渲染器，清理所有资源。

---

```cpp
bool ShouldRender();
```
检查是否应该继续渲染（窗口未关闭）。

**返回值**: 应该继续返回 `true`，应该停止返回 `false`

#### 帧管理

```cpp
bool BeginFrame();
```
开始新的渲染帧。

**返回值**: 成功返回 `true`，失败返回 `false`

---

```cpp
void EndFrame();
```
结束当前渲染帧并呈现到屏幕。

---

```cpp
void SetClearColor(float r, float g, float b, float a);
```
设置清屏颜色。

**参数**:
- `r`, `g`, `b`, `a`: RGBA 颜色值 (0.0-1.0)

#### 基本绘制

```cpp
void DrawTriangle(const Vector3& v1, const Vector3& v2, const Vector3& v3);
```
绘制三角形。

**参数**:
- `v1`, `v2`, `v3`: 三角形的三个顶点

---

```cpp
void DrawTexturedQuad(std::shared_ptr<Texture> texture, 
                      const Vector2& position, 
                      const Vector2& size);
```
绘制带纹理的四边形。

**参数**:
- `texture`: 纹理对象
- `position`: 四边形位置
- `size`: 四边形大小

---

```cpp
void DrawMesh(std::shared_ptr<Mesh> mesh, const Transform& transform);
```
绘制网格。

**参数**:
- `mesh`: 网格对象
- `transform`: 变换信息

#### 相机和视图

```cpp
void SetProjectionMatrix(const Matrix4& projection);
```
设置投影矩阵。

**参数**:
- `projection`: 投影矩阵

---

```cpp
void SetViewMatrix(const Matrix4& view);
```
设置视图矩阵。

**参数**:
- `view`: 视图矩阵

#### 渲染状态

```cpp
void SetDepthTest(bool enable);
```
启用或禁用深度测试。

**参数**:
- `enable`: 是否启用深度测试

---

```cpp
void SetWireframe(bool enable);
```
启用或禁用线框模式。

**参数**:
- `enable`: 是否启用线框模式

---

```cpp
void SetCulling(bool enable);
```
启用或禁用面剔除。

**参数**:
- `enable`: 是否启用面剔除

#### 信息查询

```cpp
const char* GetRendererName();
```
获取渲染器名称。

**返回值**: 渲染器名称字符串

---

```cpp
const char* GetGPUInfo();
```
获取 GPU 信息。

**返回值**: GPU 信息字符串

---

```cpp
float GetFrameTime();
```
获取帧时间（毫秒）。

**返回值**: 当前帧时间

---

```cpp
float GetFPS();
```
获取每秒帧数。

**返回值**: 当前 FPS

#### 事件回调

```cpp
void SetResizeCallback(std::function<void(uint32_t, uint32_t)> callback);
```
设置窗口大小调整回调。

**参数**:
- `callback`: 回调函数，参数为新的宽度和高度

---

```cpp
void SetKeyCallback(std::function<void(int, int, int, int)> callback);
```
设置键盘事件回调。

**参数**:
- `callback`: 回调函数，参数为 key, scancode, action, mods

---

```cpp
void SetMouseCallback(std::function<void(double, double)> callback);
```
设置鼠标移动回调。

**参数**:
- `callback`: 回调函数，参数为鼠标 x, y 坐标

## 数学库

### 结构体: Vector2

```cpp
struct Vector2 {
    float x, y;
    
    Vector2();
    Vector2(float x, float y);
    
    Vector2 operator+(const Vector2& other) const;
    Vector2 operator-(const Vector2& other) const;
    Vector2 operator*(float scalar) const;
    Vector2 operator/(float scalar) const;
    
    float Length() const;
    Vector2 Normalized() const;
    float Dot(const Vector2& other) const;
};
```

### 结构体: Vector3

```cpp
struct Vector3 {
    float x, y, z;
    
    Vector3();
    Vector3(float x, float y, float z);
    
    Vector3 operator+(const Vector3& other) const;
    Vector3 operator-(const Vector3& other) const;
    Vector3 operator*(float scalar) const;
    Vector3 operator/(float scalar) const;
    
    float Length() const;
    Vector3 Normalized() const;
    float Dot(const Vector3& other) const;
    Vector3 Cross(const Vector3& other) const;
};
```

### 结构体: Matrix4

```cpp
struct Matrix4 {
    float data[16];
    
    Matrix4();
    Matrix4(float diagonal);
    
    Matrix4 operator*(const Matrix4& other) const;
    Vector3 operator*(const Vector3& vec) const;
    
    static Matrix4 Identity();
    static Matrix4 Translate(const Vector3& translation);
    static Matrix4 Rotate(const Vector3& rotation);
    static Matrix4 Scale(const Vector3& scale);
    static Matrix4 Perspective(float fov, float aspect, float near, float far);
    static Matrix4 LookAt(const Vector3& eye, const Vector3& center, const Vector3& up);
};
```

### 结构体: Transform

```cpp
struct Transform {
    Vector3 position;
    Vector3 rotation;  // 欧拉角 (弧度)
    Vector3 scale;
    
    Transform();
    Matrix4 GetMatrix() const;
};
```

### 命名空间: Math

```cpp
namespace Math {
    const float PI = 3.14159265359f;
    
    float ToRadians(float degrees);
    float ToDegrees(float radians);
    float Lerp(float a, float b, float t);
    float Clamp(float value, float min, float max);
}
```

## 资源管理

### 类: Texture

```cpp
class Texture {
public:
    uint32_t GetWidth() const;
    uint32_t GetHeight() const;
    uint32_t GetChannels() const;
};
```

### 类: Mesh

```cpp
class Mesh {
public:
    uint32_t GetVertexCount() const;
    uint32_t GetIndexCount() const;
};
```

### 资源加载函数

```cpp
std::shared_ptr<Texture> RenderAPI::LoadTexture(const std::string& path);
```
从文件加载纹理。

**参数**:
- `path`: 纹理文件路径

**返回值**: 纹理对象，失败返回 `nullptr`

---

```cpp
std::shared_ptr<Mesh> RenderAPI::CreateMesh(const std::vector<float>& vertices,
                                           const std::vector<uint32_t>& indices);
```
创建网格对象。

**参数**:
- `vertices`: 顶点数据 (位置 + 纹理坐标)
- `indices`: 索引数据

**返回值**: 网格对象，失败返回 `nullptr`

## 实用工具

### 图像加载

```cpp
struct ImageData {
    unsigned char* data;
    int width;
    int height;
    int channels;
};

ImageData LoadImage(const std::string& path);
void FreeImage(ImageData& image);
```

### 着色器编译

```cpp
std::vector<uint32_t> CompileShader(const std::string& source, 
                                   const std::string& type);
```
编译 GLSL 着色器到 SPIR-V。

**参数**:
- `source`: GLSL 源代码
- `type`: 着色器类型 ("vertex", "fragment", "geometry", "compute")

**返回值**: SPIR-V 字节码

## 错误处理

AquaVisual 使用以下错误处理策略：

1. **返回值检查**: 大多数函数返回 `bool` 或指针来指示成功/失败
2. **异常**: 严重错误可能抛出 `std::runtime_error`
3. **日志**: 错误信息会输出到控制台（调试模式）

### 示例错误处理

```cpp
try {
    if (!AquaVisual::Initialize()) {
        std::cerr << "Failed to initialize AquaVisual" << std::endl;
        return -1;
    }
    
    auto renderer = AquaVisual::CreateRenderer(config);
    if (!renderer) {
        std::cerr << "Failed to create renderer" << std::endl;
        return -1;
    }
    
    auto texture = renderer->LoadTexture("texture.png");
    if (!texture) {
        std::cerr << "Failed to load texture" << std::endl;
        // 继续执行或使用默认纹理
    }
    
} catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    return -1;
}
```

## 性能注意事项

1. **资源管理**: 使用智能指针自动管理资源生命周期
2. **批量渲染**: 尽可能批量绘制相同类型的对象
3. **状态更改**: 最小化渲染状态更改
4. **内存分配**: 避免在渲染循环中频繁分配内存

## 版本兼容性

- **API 版本**: 1.0.0
- **Vulkan 版本**: 1.3+
- **C++ 标准**: C++17+

---

更多详细信息和示例，请参考 [集成指南](INTEGRATION_GUIDE.md) 和 [示例项目](Examples/)。