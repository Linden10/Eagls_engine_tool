# Eagls Engine Tool

<div align="center">
  <img src="https://img.shields.io/badge/language-C++-blue.svg" alt="Language">
  <img src="https://img.shields.io/badge/platform-Windows-brightgreen.svg" alt="Platform">
  <img src="https://img.shields.io/badge/license-MIT-orange.svg" alt="License">
</div>

## 📝 项目简介 | Project Introduction

Eagls Engine Tool 是一套用于处理 EAGLS 引擎游戏资源的工具集，包括文本提取与修改、图片压缩与加密/解密、资源打包与解包等功能。

Eagls Engine Tool is a toolkit for processing game resources from the EAGLS engine, including text extraction and modification, image compression and encryption/decryption, resource packing and unpacking.

## 🚀 使用方法 | Usage

### 图片转换 | Image Conversion (bmp2gr)

```bash
bmp2gr.exe <输入目录|input_directory> <输出目录|output_directory>
```

### 资源打包 | Resource Packing (pak_packer)

```bash
pak_packer.exe <输入目录|input_directory> <输出文件路径|output_file_path>
```

### 资源解包 | Resource Unpacking (pak_unpacker)

```bash
pak_unpacker.exe <pak文件路径|pak_file_path> <输出目录|output_directory> [解密|decrypt=1]
```

### Python 脚本 | Python Scripts

#### 打包脚本 | Packing Script

```bash
python pack.py
```

#### 解包脚本 | Unpacking Script

```bash
python unpack.py <pak文件路径|pak_file_path> --output <输出目录|output_directory> [--no-decrypt]
```


## ⚠️ 免责声明 | Disclaimer

本工具仅供学习和研究使用，请勿用于任何商业用途或侵犯他人知识产权的行为。使用本工具处理的任何资源，其版权仍归原作者所有。使用本工具所产生的任何法律责任由使用者自行承担，与本工具作者无关。

This tool is for educational and research purposes only. Do not use it for any commercial purposes or activities that infringe on others' intellectual property rights. The copyright of any resources processed using this tool remains with the original authors. Any legal responsibility arising from the use of this tool is borne by the user and is not related to the author of this tool.
