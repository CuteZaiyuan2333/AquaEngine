#pragma once

#include <string>
#include <unordered_map>
#include <variant>

namespace Aqua {
namespace Utils {

using ConfigValue = std::variant<int, float, bool, std::string>;

class Config {
public:
    static Config& GetInstance() {
        static Config instance;
        return instance;
    }

    template<typename T>
    void Set(const std::string& key, const T& value) {
        m_config[key] = value;
    }

    template<typename T>
    T Get(const std::string& key, const T& defaultValue = T{}) const {
        auto it = m_config.find(key);
        if (it == m_config.end()) {
            return defaultValue;
        }
        try {
            return std::get<T>(it->second);
        } catch (const std::bad_variant_access&) {
            return defaultValue;
        }
    }

    bool Has(const std::string& key) const {
        return m_config.find(key) != m_config.end();
    }

private:
    std::unordered_map<std::string, ConfigValue> m_config;

    Config() {
        // 设置默认值
        m_config["renderer.window_width"] = 1280;
        m_config["renderer.window_height"] = 720;
        m_config["renderer.window_title"] = std::string("AquaEngine");
        m_config["renderer.vsync"] = true;
        m_config["renderer.max_frames_in_flight"] = 2;
        
        m_config["vulkan.enable_validation"] = true;
        m_config["vulkan.enable_debug_messenger"] = true;
        
        m_config["logging.level"] = 1;
        m_config["logging.enable_file_output"] = false;
        
        m_config["paths.assets"] = std::string("Assets/");
        m_config["paths.shaders"] = std::string("Assets/Shaders/");
        m_config["paths.textures"] = std::string("Assets/Textures/");
    }
};

#define AQUA_CONFIG Aqua::Utils::Config::GetInstance()

} // namespace Utils
} // namespace Aqua