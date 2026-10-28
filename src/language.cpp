/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 HandsomeTurtle0307
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 */
#include "language.h"

#include <QLocale>
#include <QSettings>

namespace {

struct TranslationEntry
{
    const char *key;
    const char *zh;
    const char *en;
};

// Keep every user-visible string here. Dynamic values (paths, exit codes)
// are appended by the caller.
const TranslationEntry kTranslations[] = {
    // ---- Main window ----
    {"app_title", "OllaMerge", "OllaMerge"},
    {"app_subtitle", "LoRA 合并与量化工具", "LoRA merge & quantization toolkit"},
    {"base_model_path", "基础模型路径：", "Base model path:"},
    {"lora_path", "LoRA路径：", "LoRA path:"},
    {"save_path", "保存路径：", "Save path:"},
    {"quant_dataset", "量化校准数据集名称：", "Calib. dataset:"},
    {"model_card_title", "模型文件", "Model files"},
    {"params_card_title", "合并参数", "Merge options"},
    {"log_card_title", "运行日志", "Run log"},
    {"shard_size_gb", "切片大小（GB）:", "Shard (GB):"},
    {"quant_bits", "量化位数：", "Quant bits:"},
    {"use_safetensors", "使用safetensors格式", "Use safetensors format"},
    {"bin_hint", "不勾选此选项的话将使用bin格式", "Unchecked = bin format"},
    {"browse", "浏览...", "Browse..."},
    {"start_merge", "开始合并！", "Merge now!"},
    {"about_button", "关于...", "About..."},
    {"help", "帮助", "Help"},
    {"language_label", "语言：", "Language:"},
    {"language_tooltip", "切换界面语言（立即生效）", "Switch interface language (applies immediately)"},
    {"switch_to_dark", "切换到深色模式", "Switch to dark mode"},
    {"switch_to_light", "切换到浅色模式", "Switch to light mode"},
    {"no_quantization", "不量化", "None"},
    {"dataset_placeholder", "在 LLaMA-Factory 中注册的数据集名称", "Dataset name registered in LLaMA-Factory"},

    // ---- Status ----
    {"checking_env", "检测环境中...", "Checking environment..."},
    {"env_warning", "环境不完整", "Environment incomplete"},
    {"env_error", "环境异常", "Environment error"},
    {"ready", "就绪！", "Ready!"},
    {"merging", "合并中", "Merging..."},
    {"quantizing", "量化中", "Quantizing..."},

    // ---- Log messages ----
    {"log_merge_start", "开始合并！", "Starting merge..."},
    {"log_base_model", "基础模型：", "Base model: "},
    {"log_lora_path", "LoRA路径：", "LoRA path: "},
    {"log_save_to", "将保存到：", "Saving to: "},
    {"log_safetensors", "使用safetensors格式", "Using the safetensors format"},
    {"log_bin", "使用bin格式", "Using the bin format"},
    {"log_shard_size", "分片大小：", "Shard size: "},
    {"log_merge_complete", "合并完成！", "Merge complete!"},
    {"log_merge_success", "合并成功啦！", "Merge succeeded!"},
    {"log_output_dir", "输出目录：", "Output directory: "},
    {"log_merge_failed", "合并失败，退出码", "Merge failed, exit code "},
    {"log_quant_start", "开始量化！", "Starting quantization..."},
    {"log_quant_dataset", "量化校准数据集：", "Calibration dataset: "},
    {"log_quant_bit", "量化位数：", "Quantization bits: "},
    {"log_quant_success", "量化成功！", "Quantization succeeded!"},
    {"log_quant_failed", "量化失败，退出码", "Quantization failed, exit code "},
    {"log_quant_enabled", "量化已启用", "Quantization enabled"},
    {"log_quant_disabled", "量化未启用", "Quantization disabled"},
    {"log_temp_dir", "临时目录：", "Temporary directory: "},
    {"log_quant_temp_warning",
     "注意：量化会先把合并结果写入输出目录旁的临时文件夹，请确保磁盘空间充足。",
     "Note: quantization first writes the merged model to a temporary folder next to the output directory. Make sure you have enough disk space."},
    {"log_merge_process_start_failed", "启动合并进程失败！", "Failed to start the merge process!"},
    {"log_quant_process_start_failed", "启动量化进程失败！", "Failed to start the quantization process!"},
    {"log_env_checking", "正在检测 LlamaFactory 环境...", "Checking the LlamaFactory environment..."},
    {"log_env_export_ok", "合并/导出命令可用。", "Merge/export command is available."},
    {"log_env_quant_ok", "量化导出参数可用。", "Quantization export options are available."},
    {"log_env_quant_options_missing",
     "当前 LLaMA-Factory 不提供量化导出参数。",
     "The installed LLaMA-Factory does not expose quantization export options."},
    {"log_env_python_version", "Python 版本：%1", "Python version: %1"},
    {"log_env_probe_failed", "Python 包检查失败：%1", "Python package check failed: %1"},
    {"log_env_probe_skipped",
     "未定位到 CLI 对应的 Python，已跳过包检查。",
     "Could not locate the Python interpreter used by the CLI; skipped the package check."},
    {"log_env_missing_packages", "缺少关键 Python 包：%1", "Missing Python packages: %1"},
    {"log_env_quant_backend_missing",
     "未检测到 GPTQ 后端（gptqmodel 或 auto-gptq）。",
     "No GPTQ backend found (gptqmodel or auto-gptq)."},
    {"log_env_optimum_missing",
     "未检测到 optimum，量化导出可能失败。",
     "optimum is missing; quantization export may fail."},
    {"log_env_timeout", "环境检测超时。", "Environment check timed out."},
    {"log_env_warning",
     "环境检测完成，但存在不完整项。",
     "Environment check completed with warnings."},
    {"log_env_ok",
     "环境检测通过：Python、合并与量化组件均可用。",
     "Environment check passed: Python, merge and quantization components are available."},
    {"log_env_failed",
     "环境检测失败：llamafactory-cli export 无法运行。",
     "Environment check failed: llamafactory-cli export could not run."},

    // ---- Dialog titles ----
    {"title_hint", "提示", "Notice"},
    {"title_error", "错误", "Error"},
    {"title_done", "完成", "Done"},
    {"title_env_error", "环境错误", "Environment error"},
    {"title_problem", "出了点问题", "Something went wrong"},

    // ---- Messages ----
    {"err_empty_save_dir",
     "选择的保存文件夹不是空的！重新选下？",
     "The selected save folder is not empty. Please choose another one."},
    {"err_no_llamafactory",
     "未找到llamafactory-cli命令哦！\n 请确保你的电脑上安装了Llama_Factory并且正确配置了Python环境！(虽然配置Python环境非常非常折磨)",
     "Could not find the llamafactory-cli command!\n Please make sure Llama-Factory is installed and your Python environment is configured correctly. (Configuring the Python environment can be quite painful.)"},
    {"msg_merge_success", "合并成功！输出目录：", "Merge succeeded! Output directory: "},
    {"msg_merge_failed", "合并失败，请查看日志！", "Merge failed. Please check the log."},
    {"err_start_llamafactory",
     "无法启动llamafactory-cli！ \n 具体导致启动失败的原因自行检查下~",
     "Could not start llamafactory-cli!\n Please check why the process failed to start."},
    {"msg_quant_success", "量化成功！输出目录：", "Quantization succeeded! Output directory: "},
    {"msg_quant_failed", "量化失败，请查看日志！", "Quantization failed. Please check the log."},
    {"err_nothing_selected", "你好像没选呢！", "It looks like you didn't select anything."},
    {"err_selection_cancelled", "为什么不选了呢", "Why did you cancel?"},
    {"err_dir_not_exist",
     "选择的文件夹好像不存在呢~核实一下?",
     "The selected folder does not seem to exist. Please check it."},
    {"err_not_model_dir",
     "选择的文件夹好像不是有效的模型目录呢~(缺少config.json) \n 检查一下选没选对目录?",
     "The selected folder does not appear to be a valid model directory (config.json is missing).\n Please check the folder."},
    {"err_not_lora_dir",
     "选择的文件夹好像不是有效的LoRA权重目录呢~(缺少adapter_config.json) \n 检查一下选没选对目录?",
     "The selected folder does not appear to be a valid LoRA weights directory (adapter_config.json is missing).\n Please check the folder."},
    {"err_no_base", "你好像没选择基础模型文件夹哦~", "You haven't selected a base model folder."},
    {"err_no_lora", "你好像没选择LoRA文件夹哦~", "You haven't selected a LoRA folder."},
    {"err_no_output", "你好像没选保存在哪", "You haven't chosen where to save the model."},
    {"err_no_quant_dataset",
     "你选择了量化，但没填校准数据集的名字（这玩意的注意事项请点击输入框旁边的“帮助”按钮！）",
     "Quantization is enabled, but the calibration dataset name is empty. Click the \"Help\" button next to the field for details."},
    {"err_dir_not_exist_save",
     "选择的文件夹好像不存在呢，是不是选错了？",
     "The selected folder does not seem to exist. Did you pick the wrong one?"},
    {"err_temp_dir_failed",
     "无法创建临时目录！请检查输出目录所在磁盘的空间和写入权限。",
     "Could not create a temporary directory. Check the free space and write permissions of the output location."},

    // ---- File dialogs ----
    {"dlg_select_base", "选择基础模型文件夹", "Select base model folder"},
    {"dlg_select_lora", "选择LoRA权重文件夹", "Select LoRA weights folder"},
    {"dlg_select_save", "选择保存的位置", "Select save location"},

    // ---- Quantization help ----
    {"quant_help_title", "量化数据集帮助", "Quantization dataset help"},
    {"quant_help_text",
     "由于LLaMA-Factory的设计，使用量化功能前，请先在 LLaMA-Factory 的 data/ 目录下放置校准文件，并在 data/dataset_info.json 中注册，然后在 OllaMerge 中填入注册的数据集名称（注意：这里填名称，不是文件路径）。校准集建议使用中文长文本，单条长度超过 1024 token，字段名需与 dataset_info.json 中 columns 配置一致。",
     "Because of Llama-Factory's design, before using quantization you must place a calibration file in LLaMA-Factory's data/ directory, register it in data/dataset_info.json, and then enter the registered dataset name in OllaMerge (note: enter the dataset name, not a file path). Use long Chinese texts for the calibration set, with each entry longer than 1024 tokens. Field names must match the columns configuration in dataset_info.json."},

    // ---- About ----
    {"about_title", "关于OllaMerge", "About OllaMerge"},
    {"about_author", "作者:HandsomeTurtle0307", "Author: HandsomeTurtle0307"},
    {"about_description",
     "OllaMerge 是一个图形化工具，用于简化 LoRA 适配器与基础模型的合并，以及合并后的量化导出流程。",
     "OllaMerge is a graphical tool that simplifies merging LoRA adapters into base models and exporting quantized models."},
    {"about_github_repo", "GitHub 仓库", "GitHub repository"},
    {"about_github_issues", "问题反馈", "Report an issue"},
    {"about_github_releases", "发布页面", "Releases"},
    {"about_links_title", "相关链接", "Links"},
    {"about_environment_title", "运行环境", "Environment"},
    {"about_credits_title", "致谢", "Credits"},
    {"about_close", "关闭", "Close"},
};

constexpr int kTranslationCount = int(sizeof(kTranslations) / sizeof(kTranslations[0]));

} // namespace

I18n &I18n::instance()
{
    static I18n manager;
    return manager;
}

I18n::I18n(QObject *parent)
    : QObject(parent)
    , m_language(Chinese)
{
    for (int i = 0; i < kTranslationCount; ++i) {
        const TranslationEntry &entry = kTranslations[i];
        m_translations.insert(QString::fromUtf8(entry.key),
                              std::pair<QString, QString>(QString::fromUtf8(entry.zh),
                                                          QString::fromUtf8(entry.en)));
    }
    loadSettings();
}

void I18n::loadSettings()
{
    QSettings settings(QStringLiteral("HandsomeTurtle0307"), QStringLiteral("OllaMerge"));
    const QString saved = settings.value(QStringLiteral("ui/language")).toString();

    if (saved == QLatin1String("en")) {
        m_language = English;
    } else if (saved == QLatin1String("zh")) {
        m_language = Chinese;
    } else if (QLocale::system().language() == QLocale::Chinese) {
        m_language = Chinese;
    } else {
        m_language = English;
    }
}

I18n::Language I18n::language() const
{
    return m_language;
}

void I18n::setLanguage(Language language)
{
    if (m_language == language) {
        return;
    }

    m_language = language;

    QSettings settings(QStringLiteral("HandsomeTurtle0307"), QStringLiteral("OllaMerge"));
    settings.setValue(QStringLiteral("ui/language"),
                      language == English ? QStringLiteral("en") : QStringLiteral("zh"));

    emit languageChanged();
}

QString I18n::text(const QString &key) const
{
    const auto it = m_translations.constFind(key);
    if (it == m_translations.constEnd()) {
        return key;
    }
    const std::pair<QString, QString> &pair = it.value();
    return m_language == English ? pair.second : pair.first;
}

QString I18n::text(const char *key) const
{
    return text(QString::fromUtf8(key));
}

QString I18n::t(const QString &key)
{
    return instance().text(key);
}

QString I18n::t(const char *key)
{
    return instance().text(key);
}
