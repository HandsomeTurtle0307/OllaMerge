

![License](https://img.shields.io/badge/License-GPL--3.0-orange?style=for-the-badge)![Qt](https://img.shields.io/badge/Qt-6.5-41CD52?style=for-the-badge&logo=qt)

Chinese/English

# OllaMerge Readme

## 这是什么

OllaMerge是一个**图形化**工具，可简化LoRA适配器与原始模型的合并以及模型在Ollama上部署的过程

## 为啥要做这个

我在用我编写的数据集（Saudade-dataset）微调大模型，将LoRA权重合并到大模型本体上的时候，因为各种奇奇怪怪、难以理解的Python软件包问题和命令行指令过于繁琐，导致我无法顺利的完成合并，让我比较生气。所以，我决定自己编写一个图形化的工具，来让程序员或者普通用户的合并过程更加简便。

## 使用方法

由于是图形化工具，使用方法非常简便。只需要在Release页面中下载好自解压文件（.exe），解压后在文件夹内双击**OllaMerge.exe**运行即可。

## 系统要求

- Windows 10/11 或 Linux (Ubuntu 等)
- 已安装 [LlamaFactory](https://github.com/hiyouga/LLaMA-Factory)（需要 `llamafactory-cli` 命令可用）
- **构建环境（如需从源码编译）：**
  - 至少CMake 3.19 
  - C++ 编译器（支持 C++17就行）
  - 至少Qt 6.5

## 一些提示

在使用此工具之前，请确保你的系统中正确安装了**LLaMA-Factory**以及正确配置了各种繁琐复杂的Python软件包，让LLaMA-Factory正确工作！同时，请确保你的磁盘中有足够的控件来存放大模型以及LoRA权重文件！

## 最后

由于我是一名学生，所以我做的软件可以说功能并不完善，甚至可以说有点简陋，请大家多多包涵。目前缺失的功能（比如详细参数选择）我会在后续版本中补全并持续维护！如果你有任何建议，或者发现了一些问题，欢迎在Issue中讨论！我会尽可能的采纳大家的意见，并且将来也会发布英文版的软件。

## 鸣谢

本项目能够顺利完成，感谢以下开源项目：

- **[Qt for Open Source Use](https://www.qt.io/)** 
- **[LLaMA-Factory](https://github.com/hiyouga/LLaMA-Factory)**

# OllaMerge Readme

## What is this?

OllaMerge is a **graphical tool** designed to simplify the process of merging LoRA adapters with base models and deploying them on Ollama.

## Why build this?

While fine-tuning large models using my own dataset (Saudade-dataset), I encountered numerous frustrating issues—strange Python package conflicts and overly complicated command-line instructions—that made merging LoRA weights into the base model nearly impossible. This really annoyed me, so I decided to create a graphical tool to make the merging process much easier for both developers and regular users.

## How to use

As a graphical application, it's extremely simple to get started. Just download the self-extracting executable (.exe) from the Releases page, extract the files, and double-click **OllaMerge.exe** in the folder to run.

## System Requirements

- Windows 10/11 or Linux (Ubuntu, etc.)
- [LlamaFactory](https://github.com/hiyouga/LLaMA-Factory) installed (the `llamafactory-cli` command must be available)
- **Build environment (if compiling from source):**
  - CMake 3.19 or later
  - A C++ compiler supporting C++17
  - Qt 6.5 or later

## Tips

Before using this tool, please ensure that **LLaMA-Factory** is properly installed and all complex Python dependencies are correctly configured so that LLaMA-Factory works as expected. Also, make sure you have sufficient disk space to store both the large model and LoRA weight files.

## Final Notes

Since I'm still a student, the software may lack polish and feel somewhat basic—please bear with me. Missing features (such as detailed parameter options) will be added in future versions, and I'll continue maintaining the project actively. If you have any suggestions or find any bugs, feel free to open an issue. I’ll do my best to incorporate your feedback.An English version of the software will also be released in the future!

## Acknowledgments

This project wouldn't have been possible without the following open-source projects:

- **[Qt for Open Source Use](https://www.qt.io/)**
- **[LLaMA-Factory](https://github.com/hiyouga/LLaMA-Factory)**

> This English version is translated from the Chinese version; in case of discrepancies, the Chinese version shall prevail.
