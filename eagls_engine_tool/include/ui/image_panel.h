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
 * @brief 图像面板
 */
class EAGLS_UI_API ImagePanel {
public:
    /**
     * @brief 构造函数
     */
    ImagePanel();
    
    /**
     * @brief 析构函数
     */
    ~ImagePanel();
    
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
    std::vector<std::string> m_files; // 文件列表
    int m_conversionType;            // 转换类型
    
    /**
     * @brief 渲染BMP转GR界面
     */
    void renderBmpToGrUI();
    
    /**
     * @brief 渲染GR转BMP界面
     */
    void renderGrToBmpUI();
    
    /**
     * @brief 渲染PNG转BMP界面
     */
    void renderPngToBmpUI();
    
    /**
     * @brief 渲染BMP转PNG界面
     */
    void renderBmpToPngUI();
    
    /**
     * @brief 处理BMP转GR操作
     */
    void handleBmpToGr();
    
    /**
     * @brief 处理GR转BMP操作
     */
    void handleGrToBmp();
    
    /**
     * @brief 处理PNG转BMP操作
     */
    void handlePngToBmp();
    
    /**
     * @brief 处理BMP转PNG操作
     */
    void handleBmpToPng();
};

} // namespace ui
} // namespace eagls