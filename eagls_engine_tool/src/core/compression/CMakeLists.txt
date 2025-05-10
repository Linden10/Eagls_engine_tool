set(MODULE_NAME eagls_compression)

# 源文件
set(SOURCES
    lzss.cpp
)

# 头文件
set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include/core/compression/lzss.h
)

# 创建动态库
add_library(${MODULE_NAME} SHARED ${SOURCES} ${HEADERS})

# 设置包含目录
target_include_directories(${MODULE_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
)

# 设置导出宏
target_compile_definitions(${MODULE_NAME} PRIVATE EAGLS_COMPRESSION_EXPORTS)
target_compile_definitions(${MODULE_NAME} PUBLIC EAGLS_COMPRESSION_DLL)

# 安装规则
install(TARGETS ${MODULE_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION bin
)