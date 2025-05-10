#include <iostream>
#include <ui/ui_manager.h>

int main(int argc, char* argv[]) {
    std::cout << "EAGLS Engine Tool - Starting..." << std::endl;
    
    // 初始化UI管理器
    eagls::ui::UIManager uiManager;
    
    // 运行UI
    int result = uiManager.run();
    
    std::cout << "EAGLS Engine Tool - Exiting..." << std::endl;
    return result;
}