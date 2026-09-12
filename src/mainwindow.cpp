/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 HandsomeTurtle0307
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->spinExportSize->addItem("1 GB",1);
    ui->spinExportSize->addItem("2 GB",2);
    ui->spinExportSize->addItem("4 GB",4);
    ui->spinExportSize->addItem("8 GB",8);
    ui->spinExportSize->addItem("16 GB",16);
    ui->spinExportSize->setCurrentIndex(2);
    ui->chkSafetensors->setChecked(true);
    ui->logTextEdit->setReadOnly(true);
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_selectBaseBtn_clicked(){
    QString dir=QFileDialog::getExistingDirectory(this,"选择基础模型文件夹");
    if(dir.isEmpty()){
        QMessageBox::warning(this,"提示","为什么不选了呢");
        return;
    }

    if(!QDir(dir).exists()){
        QMessageBox::critical(this,"出了点问题","选择的文件夹好像不存在呢~核实一下?");
        return;
    }

    QFile configFile(dir+"/config.json");
    if(!configFile.exists()){
        QMessageBox::warning(this,"提示","选择的文件夹好像不是有效的模型目录呢~(缺少config.json) \n 检查一下选没选对目录?");
        return;
    }
    ui->basePathlineEdit->setText(dir);
}

void MainWindow::on_selectLoraBtn_clicked(){
    QString dir=QFileDialog::getExistingDirectory(this,"选择LoRA权重文件夹");
    if(dir.isEmpty()){
        QMessageBox::warning(this,"提示","为什么不选了呢");
        return;
    }

    if(!QDir(dir).exists()){
        QMessageBox::critical(this,"提示","选择的文件夹好像不存在呢~核实一下?");
        return;
    }

    QFile configFile(dir+"/adapter_config.json");
    if(!configFile.exists()){
        QMessageBox::warning(this,"提示","选择的文件夹好像不是有效的LoRA权重目录呢~(缺少adapter_config.json) \n 检查一下选没选对目录?");
        return;
    }
    ui->loraPathlineEdit->setText(dir);
}

void MainWindow::on_mergeBtn_clicked(){
    QString basePath=ui->basePathlineEdit->text();
    QString loraPath=ui->loraPathlineEdit->text();
    QString outputPath=ui->modleSavePathlineEdit->text();

    if(basePath.isEmpty()){
        QMessageBox::warning(this,"提示","你好像没选择基础模型文件夹哦~");
        return;
    }

    if(loraPath.isEmpty()){
        QMessageBox::warning(this,"提示","你好像没选择LoRA文件夹哦~");
        return;
    }

    if(!QDir(basePath).exists()){
        QMessageBox::critical(this,"提示","选择的模型文件夹好像不存在呢~核实一下?");
        return;
    }
    if(!QDir(loraPath).exists()){
        QMessageBox::critical(this,"提示","选择的LoRA文件夹好像不存在呢~核实一下?");
        return;
    }
    QFile configFile(loraPath+"/adapter_config.json");
    if(!configFile.exists()){
        QMessageBox::warning(this,"提示","选择的文件夹好像不是有效的LoRA权重目录呢~(缺少adapter_config.json) \n 检查一下选没选对目录?");
        return;
    }
    QFile configFile1(basePath+"/config.json");
    if(!configFile1.exists()){
        QMessageBox::warning(this,"提示","选择的文件夹好像不是有效的模型目录呢~(缺少config.json) \n 检查一下选没选对目录?");
        return;
    }

    if(outputPath.isEmpty()){
        QMessageBox::warning(this,"提示","你好像没选保存在哪！");
        return;
    }

    if(!QDir(outputPath).exists()){
        QMessageBox::warning(this,"提示","选择的保存位置好像不存在呢，是不是选错了？");
        return;
    }

    QDir selectedDir(outputPath);
    QStringList entries=selectedDir.entryList(QDir::AllEntries|QDir::NoDotAndDotDot);
    if(!entries.isEmpty()){
        QMessageBox::warning(this,"提示","选择的保存文件夹不是空的！重新选下？");
        return;
    }
    QProcess testProcess;
    testProcess.start("llamafactory-cli",{"--help"});
    testProcess.waitForFinished(3000);
    if(testProcess.exitCode()!=0){
        QMessageBox::critical(this,"环境错误","未找到llamafactory-cli命令哦！\n 请确保你的电脑上安装了Llama_Factory并且正确配置了Python环境！(虽然配置Python环境非常非常折磨)");
        return;
    }

    bool isSafetensors=ui->chkSafetensors->isChecked();
    int exportSize=ui->spinExportSize->currentData().toInt();
    ui->mergeBtn->setEnabled(false);
    appendLog("开始合并！");
    appendLog("基础模型："+basePath);
    appendLog("LoRA路径："+loraPath);
    appendLog("将保存到："+outputPath);
    if(isSafetensors){
        appendLog("使用safetensors格式");
    }else{
        appendLog("使用bin格式");
    }
    appendLog("分片大小："+QString::number(exportSize));

    QStringList args;
    args <<"export"
         <<"--model_name_or_path"<<basePath
         <<"--adapter_name_or_path"<<loraPath
         <<"--export_dir"<<outputPath
         <<"--export_size"<<QString::number(exportSize)
         <<"--export_legacy_format"<<(isSafetensors?"false":"true");

    QProcess *mergeProcess=new QProcess(this);
    mergeProcess->setProcessChannelMode(QProcess::MergedChannels);

    //日志
    connect(mergeProcess,&QProcess::readyReadStandardOutput,[this,mergeProcess](){
        QString output=mergeProcess->readAllStandardOutput();
        if(!output.trimmed().isEmpty()){
            appendLog(output);
        }
    });


    connect(mergeProcess,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),[this,mergeProcess,outputPath](int exitCode,QProcess::ExitStatus){
        ui->mergeBtn->setEnabled(true);
        if(exitCode==0){
            appendLog("合并成功啦！");
            appendLog("输出目录："+outputPath);
            QMessageBox::information(this,"完成","合并成功！输出目录："+outputPath);
        }else{
            appendLog("合并失败，退出码"+QString::number(exitCode));
            QMessageBox::critical(this,"错误","合并失败，请查看日志！");
        }
        mergeProcess->deleteLater();
    });

    mergeProcess->start("llamafactory-cli",args);
    if(!mergeProcess->waitForStarted()){
        appendLog("启动合并进程失败！");
        ui->mergeBtn->setEnabled(true);
        QMessageBox::critical(this,"错误","无法启动llamafactory-cli！ \n 具体导致启动失败的原因自行检查下~");
        mergeProcess->deleteLater();
    }
}

void MainWindow::appendLog(const QString &message){
    ui->logTextEdit->append(message);
}

void MainWindow::setStatus(QString const&){
    //没做完
}

void MainWindow::on_aboutBtn_clicked(){
    QMessageBox::about(this,"关于OllaMerge",
        "<h2>OllaMerge</h2>"
        "<p>v1.0</p>"
        "<br>"
        "作者:HandsomeTurtle0307"
        "<br>"
        "<p>GitHub：<a href='https://github.com/HandsomeTurtle0307/OllaMerge'>"
        "HandsomeTurtle0307/OllaMerge</a></p>"
        "<br>"
        "<p>感谢使用!</p>"
        "<p align='center'>"
        "<p style='color: #2ecc71; font-size: 18px; font-weight: bold; font-family: Consolas, monospace;'>"
        "Built with Qt</p>"
        "</p>"
    );
}

void MainWindow::on_selectSaveBin_clicked(){
    QString dir=QFileDialog::getExistingDirectory(this,"选择保存的位置");
    if(dir.isEmpty()){
        QMessageBox::warning(this,"提示","你好像没选呢！");
        return;
    }

    if(!QDir(dir).exists()){
        QMessageBox::warning(this,"提示","选择的文件夹好像不存在呢，是不是选错了？");
        return;
    }

    QDir selectedDir(dir);
    QStringList entries=selectedDir.entryList(QDir::AllEntries|QDir::NoDotAndDotDot);
    if(!entries.isEmpty()){
        QMessageBox::warning(this,"提示","选择的保存文件夹不是空的！重新选下？");
        return;
    }
    ui->modleSavePathlineEdit->setText(dir);
}
