![Qt](https://img.shields.io/badge/Qt-6.5-41CD52?style=for-the-badge&logo=qt)

# OllaMerge Readme

大家好！OllaMerge上线啦！

## 这是什么

OllaMerge是一个图形化工具，可简化 LoRA 适配器的合并以及模型在Ollama上部署的过程

## 为啥要做这个

我在用我编写的数据集（Saudade-dataset）微调大模型，将LoRA权重合并到大模型本体上的时候，因为各种奇奇怪怪、难以理解的Python软件包问题和命令行指令过于繁琐，导致我无法顺利的完成合并，让我比较生气。所以，我决定自己编写一个图形化的工具，来让程序员或者普通用户的合并过程更加简便。

## 使用方法

由于是图形化工具，使用方法非常简便。只需要在Release页面中下载好自解压文件（.exe），解压后在文件夹内双击**OllaMerge.exe**运行即可。

## 系统要求

- Windows 10/11 或 Linux (Ubuntu 等)
- 已安装 [LlamaFactory](https://github.com/hiyouga/LLaMA-Factory)（需要 `llamafactory-cli` 命令可用）
- **构建环境（如需从源码编译）：**
  - CMake 3.19 或更高版本 
  - C++ 编译器（支持 C++17，如 GCC 10+、MSVC 2019+）
  - Qt 6.5 或更高版本 

## 一些提示

在使用此工具之前，请确保你的系统中正确安装了**LLaMA-Factory**以及正确配置了各种繁琐复杂的Python软件包，让LLaMA-Factory正确工作！同时，请确保你的磁盘中有足够的控件来存放大模型以及LoRA权重文件！

## 最后

由于我是一名学生，所以我做的软件可以说功能并不完善，甚至可以说有点简陋，请大家多多包涵。目前缺失的功能（比如详细参数选择）我会在后续版本中补全并持续维护！如果你有任何建议，或者发现了一些问题，欢迎在Issue中讨论！我会斟酌并尽可能的采纳大家的意见的。

## 鸣谢

本项目能够顺利完成，感谢以下开源项目：

- **[Qt](https://www.qt.io/)** —— 提供了一套优雅的 GUI 开发框架，让 C++ 写界面变得如此简单。
- **[LLaMA-Factory](https://github.com/hiyouga/LLaMA-Factory)** —— 强大的大模型微调工具，是 OllaMerge 的核心后端。
