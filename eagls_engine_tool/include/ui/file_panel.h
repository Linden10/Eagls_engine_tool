#pragma once

#include <string>
#include <vector>

// DLL导出宏定义
#ifdef _WIN32
    #ifdef EAGLS_UI_EXPORTS
        #define EAGLS_UI_API __declspec(dllexport)
    #else
        #define EAGLS_UI_API __declspec(dllimport)
    #endif
#else
    #define EAGLS_UI_API
#endif

namespace eagls {
namespace ui {

/**
 * @brief 文件面板
 */
class EAGLS_UI_API FilePanel {
public:
    /**
     * @brief 构造函数
     */
    FilePanel();
    
    /**
     * @brief 析构函数
     */
    ~FilePanel();
    
    /**
     * @brief 渲染面板
     */
    void render();
    
    /**
     * @brief 设置是否可见
     * @param visible 是否可见
     */
    void setVisible(bool visible);
    
    /**
     * @brief 是否可见
     * @return 是否可见
     */
    bool isVisible() const;

private:
    bool m_visible;                  // 是否可见
    std::string m_pakFilename;       // PAK文件名
    std::string m_outputPath;        // 输出路径
    std::vector<std::string> m_files; // 文件列表
    bool m_decrypt;                  // 是否解密
    
    /**
     * @brief 渲染解包界面
     */
    void renderUnpackUI();
    
    /**
     * @brief 渲染打包界面
     */
    void renderPackUI();
    
    /**
     * @brief 处理解包操作
     */
    void handleUnpack();
    
    /**
     * @brief 处理打包操作
     */
    void handlePack();
};

} // namespace ui
} // namespace eagls