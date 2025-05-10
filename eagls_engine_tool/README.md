# EAGLS引擎工具包 (EAGLS Engine Tool)

## 结构
```
eagls_engine_tool/
├── src/                    # 源代码
│   ├── core/               # 核心功能模块
│   │   ├── compression/    # 压缩解压模块 (LZSS算法)
│   │   ├── encryption/     # 加密解密模块 (Lehmer算法)
│   │   ├── file/           # 文件处理模块
│   │   ├── image/          # 图像处理模块
│   │   └── text/           # 文本处理模块
│   ├── ui/                 # UI相关代码
│   └── main.cpp            # 主程序入口
├── include/                # 头文件
├── lib/                    # 第三方库
├── build/                  # 构建输出目录
├── bin/                    # 可执行文件和DLL输出目录
└── CMakeLists.txt          # CMake构建脚本
```