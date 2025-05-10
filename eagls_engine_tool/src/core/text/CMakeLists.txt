set(MODULE_NAME eagls_text)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 源文件
set(SOURCES
    text_extractor.cpp
    text_replacer.cpp
    encoding_converter.cpp
    text_converter.cpp
)

# 头文件
set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include/core/text/text_extractor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include/core/text/text_replacer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include/core/text/encoding_converter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include/core/text/text_converter.h
)

# 创建动态库
add_library(${MODULE_NAME} SHARED ${SOURCES} ${HEADERS})

# 设置包含目录
target_include_directories(${MODULE_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
)

# 设置导出宏
target_compile_definitions(${MODULE_NAME} PRIVATE EAGLS_TEXT_EXPORTS)
target_compile_definitions(${MODULE_NAME} PUBLIC EAGLS_TEXT_DLL)

# 链接其他模块
target_link_libraries(${MODULE_NAME}
    eagls_file
    ${CMAKE_CURRENT_SOURCE_DIR}/../file/build/Release/eagls_file.lib
)

# 安装规则
install(TARGETS ${MODULE_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION bin
)