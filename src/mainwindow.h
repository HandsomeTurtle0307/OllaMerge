/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 HandsomeTurtle0307
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private slots:
    void on_selectBaseBtn_clicked();
    void on_selectLoraBtn_clicked();
    void on_mergeBtn_clicked();
    void on_selectSaveBin_clicked();
    void appendLog(const QString &message);
    void on_aboutBtn_clicked();
    void startMerge();
    void startQuantize();
    void on_selectQuantDatasetBtn_clicked();
    void on_comboQuantBit_currentIndexChanged(int index);
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
    QString m_basePath;//基础模型路径
    QString m_loraPath;//LoRA路径
    QString m_outputPath;//最终输出路径
    QString m_quantDataset;//量化数据集（应该是叫这个吧，不清楚0w0）;
    QString m_quantBit;//量化位数（这个肯定叫这个）
    //bool m_isQuantizing;//是否在量化中
    int m_exportSize;//分片大小（可能是切片吧）
    bool m_isSafetensors;//是否Safetensors（顾名思义，哈哈哈）
    QString m_tempPath;//临时目录（后续可能会让用户自己选，但是现在没时间搞了，呜呜呜）
};
#endif // MAINWINDOW_H
