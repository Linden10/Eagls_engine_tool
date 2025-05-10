cmake_minimum_required(VERSION 3.10)

project(Eagls_engine_tool VERSION 1.0.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


if(MSVC)
    # MSVC编译器选项
    add_compile_options(/W4)
else()
    # GCC/Clang编译器选项 | GCC/Clang compiler options
    add_compile_options(-Wall -Wextra -pedantic)
endif()


# pak_packer - 打包工具 | Packing tool
add_executable(pak_packer pak_packer/pak_packer.cpp)
target_include_directories(pak_packer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# pak_unpacker - 解包工具 | Unpacking tool
add_executable(pak_unpacker pak_unpacker/pak_unpacker.cpp)
target_include_directories(pak_unpacker PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# bmp2gr - BMP转换工具 | BMP conversion tool
add_executable(bmp2gr bmp2gr/bmp2gr.cpp)
target_include_directories(bmp2gr PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})


install(TARGETS pak_packer pak_unpacker bmp2gr
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

# 打包配置
include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
include(CPack)

enable_testing()

add_custom_target(docs
    COMMAND echo "Generating documentation..."
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Building documentation"
)
