cmake_minimum_required(VERSION 3.10)
project(eagls_engine_tool VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 添加include目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 添加第三方库目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

# 添加子目录
add_subdirectory(src/core/compression)
add_subdirectory(src/core/encryption)
add_subdirectory(src/core/file)
add_subdirectory(src/core/image)
add_subdirectory(src/core/text)
add_subdirectory(src/ui)

# 主程序
add_executable(eagls_ui src/main.cpp)

# 链接所有模块
target_link_libraries(eagls_ui
    eagls_compression
    eagls_encryption
    eagls_file
    eagls_image
    eagls_text
    eagls_ui_lib
)

# 安装规则
install(TARGETS eagls_ui
    RUNTIME DESTINATION bin
)

# 复制DLL到bin目录
install(TARGETS eagls_compression eagls_encryption eagls_file eagls_image eagls_text eagls_ui_lib
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION bin
)

# 复制第三方库DLL到bin目录
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libpng/lib/libpng16.dll
    DESTINATION bin
)