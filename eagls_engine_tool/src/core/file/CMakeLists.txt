set(MODULE_NAME eagls_file)

# 源文件
set(SOURCES
    pak_file.cpp
    file_utils.cpp
)

# 头文件
set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include/core/file/pak_file.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include/core/file/file_utils.h
)

# 创建动态库
add_library(${MODULE_NAME} SHARED ${SOURCES} ${HEADERS})

# 设置包含目录
target_include_directories(${MODULE_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
)

# 设置导出宏
target_compile_definitions(${MODULE_NAME} PRIVATE EAGLS_FILE_EXPORTS)
target_compile_definitions(${MODULE_NAME} PUBLIC EAGLS_FILE_DLL)

# 链接其他模块
target_link_libraries(${MODULE_NAME}
    eagls_encryption
)

# 安装规则
install(TARGETS ${MODULE_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION bin
)