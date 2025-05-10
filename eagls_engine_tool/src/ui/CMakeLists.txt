set(MODULE_NAME eagls_ui_lib)

# 源文件
set(SOURCES
    ui_manager.cpp
    file_panel.cpp
    image_panel.cpp
    text_panel.cpp
)

# 头文件
set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/ui/ui_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/ui/file_panel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/ui/image_panel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/ui/text_panel.h
)

# 查找Dear ImGui
# 注意：这里假设您已经将Dear ImGui的源文件添加到项目中
# 如果没有，您需要先下载并添加Dear ImGui的源文件
set(IMGUI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/imgui/backends/imgui_impl_win32.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/imgui/backends/imgui_impl_dx11.cpp
)

# 创建动态库
add_library(${MODULE_NAME} SHARED ${SOURCES} ${HEADERS} ${IMGUI_SOURCES})

# 设置包含目录
target_include_directories(${MODULE_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/imgui/backends
)

# 设置导出宏
target_compile_definitions(${MODULE_NAME} PRIVATE EAGLS_UI_EXPORTS)
target_compile_definitions(${MODULE_NAME} PUBLIC EAGLS_UI_DLL)

# 链接Windows库和DirectX库
target_link_libraries(${MODULE_NAME}
    d3d11
    dxgi
    eagls_file
    eagls_image
    eagls_text
)

# 安装规则
install(TARGETS ${MODULE_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION bin
)