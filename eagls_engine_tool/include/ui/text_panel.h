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
 * @brief 文本面板
 */
class EAGLS_UI_API TextPanel {
public:
    /**
     * @brief 构造函数
     */
    TextPanel();
    
    /**
     * @brief 析构函数
     */
    ~TextPanel();
    
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
    std::string m_inputPath;         // 输入路径
    std::string m_outputPath;        // 输出路径
    std::string m_textPath;          // 文本路径
    std::vector<std::string> m_files; // 文件列表
    std::string m_fromEncoding;      // 源编码
    std::string m_toEncoding;        // 目标编码
    int m_operationType;             // 操作类型
    
    /**
     * @brief 渲染文本提取界面
     */
    void renderExtractUI();
    
    /**
     * @brief 渲染文本替换界面
     */
    void renderReplaceUI();
    
    /**
     * @brief 渲染编码转换界面
     */
    void renderConvertUI();
    
    /**
     * @brief 处理文本提取操作
     */
    void handleExtract();
    
    /**
     * @brief 处理文本替换操作
     */
    void handleReplace();
    
    /**
     * @brief 处理编码转换操作
     */
    void handleConvert();
};

} // namespace ui
} // namespace eagls