#pragma once

#include <string>
#include <memory>

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

// 前向声明
namespace eagls {
namespace ui {
    class FilePanel;
    class ImagePanel;
    class TextPanel;
}
}

namespace eagls {
namespace ui {

/**
 * @brief UI管理器
 */
class EAGLS_UI_API UIManager {
public:
    /**
     * @brief 构造函数
     */
    UIManager();
    
    /**
     * @brief 析构函数
     */
    ~UIManager();
    
    /**
     * @brief 运行UI
     * @return 退出代码
     */
    int run();
    
    /**
     * @brief 初始化UI
     * @return 是否成功
     */
    bool initialize();
    
    /**
     * @brief 清理UI
     */
    void cleanup();
    
    /**
     * @brief 设置标题
     * @param title 标题
     */
    void setTitle(const std::string& title);
    
    /**
     * @brief 获取标题
     * @return 标题
     */
    std::string getTitle() const;

private:
    std::string m_title;                          // 窗口标题
    std::unique_ptr<FilePanel> m_filePanel;       // 文件面板
    std::unique_ptr<ImagePanel> m_imagePanel;     // 图像面板
    std::unique_ptr<TextPanel> m_textPanel;       // 文本面板
    bool m_initialized;                           // 是否已初始化
    
    /**
     * @brief 渲染UI
     */
    void render();
    
    /**
     * @brief 处理输入
     */
    void processInput();
    
    /**
     * @brief 渲染菜单栏
     */
    void renderMenuBar();
    
    /**
     * @brief 渲染状态栏
     */
    void renderStatusBar();
};

} // namespace ui
} // namespace eagls