set(MODULE_NAME eagls_image)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 源文件
set(SOURCES
    bmp_gr_converter.cpp
    image_utils.cpp
    png_bmp_converter.cpp
)

# 头文件
set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include/core/image/bmp_gr_converter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include/core/image/image_utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include/core/image/png_bmp_converter.h
)

# 创建动态库
add_library(${MODULE_NAME} SHARED ${SOURCES} ${HEADERS})

# 设置包含目录
target_include_directories(${MODULE_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/libpng/include
)

# 设置导出宏
target_compile_definitions(${MODULE_NAME} PRIVATE EAGLS_IMAGE_EXPORTS)
target_compile_definitions(${MODULE_NAME} PUBLIC EAGLS_IMAGE_DLL)

# 链接其他模块
target_link_libraries(${MODULE_NAME}
    eagls_compression
    eagls_encryption
    eagls_file
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/libpng/lib/libpng16.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../compression/build/Release/eagls_compression.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../encryption/build/Release/eagls_encryption.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../file/build/Release/eagls_file.lib
)

# 安装规则
install(TARGETS ${MODULE_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION bin
)