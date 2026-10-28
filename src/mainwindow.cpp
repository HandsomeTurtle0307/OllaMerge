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
#include "aboutdialog.h"
#include "appicon.h"
#include "appversion.h"
#include "environmentchecker.h"
#include "language.h"
#include "theme.h"
#include "windoweffects.h"
#include <QApplication>
#include <QDir>
#include <QEasingCurve>
#include <QFile>
#include <QFileDialog>
#include <QGraphicsOpacityEffect>
#include <QIcon>
#include <QMessageBox>
#include <QProcess>
#include <QPropertyAnimation>
#include <QShowEvent>
#include <QSignalBlocker>
#include <QTemporaryDir>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    setAttribute(Qt::WA_TranslucentBackground);
    ui->setupUi(this);

    ui->comboQuantBit->addItem(QString(), QStringLiteral("none"));
    ui->comboQuantBit->addItem(QString(), QStringLiteral("8"));
    ui->comboQuantBit->addItem(QString(), QStringLiteral("4"));

    ui->spinExportSize->addItem("1 GB",1);
    ui->spinExportSize->addItem("2 GB",2);
    ui->spinExportSize->addItem("4 GB",4);
    ui->spinExportSize->addItem("8 GB",8);
    ui->spinExportSize->addItem("16 GB",16);

    ui->spinExportSize->setCurrentIndex(2);
    ui->chkSafetensors->setChecked(true);
    ui->logTextEdit->setReadOnly(true);
    ui->basePathlineEdit->setReadOnly(true);
    ui->loraPathlineEdit->setReadOnly(true);
    ui->modleSavePathlineEdit->setReadOnly(true);
    ui->appLogoLabel->setScaledContents(true);
    ui->appVersionLabel->setText(QStringLiteral("v") + appVersionString());

    setupLanguageSelector();
    connect(&I18n::instance(), &I18n::languageChanged, this, &MainWindow::retranslateUi);
    connect(&ThemeManager::instance(), &ThemeManager::modeChanged, this, [this]() {
        updateThemeUi();
        WindowEffects::apply(this, ThemeManager::instance().isDark());
    });
    retranslateUi();

    appendLog(I18n::t("log_env_checking"));
    m_envChecker = new EnvironmentChecker(this);
    connect(m_envChecker, &EnvironmentChecker::finished,
            this, &MainWindow::handleEnvironmentReport);
    setStatus("checking_env");
    QTimer::singleShot(150, m_envChecker, &EnvironmentChecker::start);
}
MainWindow::~MainWindow()
{
    delete m_envChecker;
    m_envChecker = nullptr;

    delete m_tempDir;
    m_tempDir = nullptr;
    delete ui;
}

void MainWindow::setupLanguageSelector()
{
    // Language names stay in their own language so users can always find them.
    ui->comboLanguage->addItem(QStringLiteral("简体中文"), int(I18n::Chinese));
    ui->comboLanguage->addItem(QStringLiteral("English"), int(I18n::English));
    ui->comboLanguage->setCurrentIndex(I18n::instance().language() == I18n::English ? 1 : 0);

    connect(ui->comboLanguage, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onLanguageComboChanged);
}

void MainWindow::onLanguageComboChanged(int index)
{
    Q_UNUSED(index);
    I18n::instance().setLanguage(static_cast<I18n::Language>(ui->comboLanguage->currentData().toInt()));
}

void MainWindow::on_themeBtn_clicked()
{
    if (m_themeAnimating) {
        return;
    }

    m_themeAnimating = true;

    auto *effect = new QGraphicsOpacityEffect;
    effect->setOpacity(1.0);
    ui->centralwidget->setGraphicsEffect(effect);

    auto *fadeOut = new QPropertyAnimation(effect, "opacity", this);
    fadeOut->setDuration(110);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.15);
    fadeOut->setEasingCurve(QEasingCurve::InCubic);

    connect(fadeOut, &QPropertyAnimation::finished, this, [this, effect]() {
        ThemeManager::instance().toggle();

        auto *fadeIn = new QPropertyAnimation(effect, "opacity", this);
        fadeIn->setDuration(190);
        fadeIn->setStartValue(0.15);
        fadeIn->setEndValue(1.0);
        fadeIn->setEasingCurve(QEasingCurve::OutCubic);

        connect(fadeIn, &QPropertyAnimation::finished, this, [this]() {
            ui->centralwidget->setGraphicsEffect(nullptr);
            m_themeAnimating = false;
        });

        fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
    });

    fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    WindowEffects::apply(this, ThemeManager::instance().isDark());
}

void MainWindow::updateThemeUi()
{
    const bool dark = ThemeManager::instance().isDark();
    ui->themeBtn->setText(dark ? QStringLiteral("☀") : QStringLiteral("☾"));
    ui->themeBtn->setToolTip(I18n::t(dark ? "switch_to_light" : "switch_to_dark"));

    ui->appLogoLabel->setPixmap(AppIcon::create(46, dark));

    const QIcon appIcon(AppIcon::create(64, dark));
    setWindowIcon(appIcon);
    QApplication::setWindowIcon(appIcon);
}

void MainWindow::updateStatusDot()
{
    const bool busy = m_statusKey == QLatin1String("merging")
                   || m_statusKey == QLatin1String("quantizing")
                   || m_statusKey == QLatin1String("checking_env");
    const bool error = m_statusKey == QLatin1String("env_error");
    const bool warning = m_statusKey == QLatin1String("env_warning");
    const QString color = busy ? QStringLiteral("#f59e0b")
                               : (error ? QStringLiteral("#ef4444")
                                        : (warning ? QStringLiteral("#eab308")
                                                   : QStringLiteral("#10b981")));

    ui->statusDot->setStyleSheet(
        QStringLiteral("background-color: %1; border-radius: 4px;").arg(color));

    if (busy) {
        if (m_statusPulse) {
            return;
        }

        auto *effect = new QGraphicsOpacityEffect;
        effect->setOpacity(1.0);
        ui->statusDot->setGraphicsEffect(effect);

        auto *pulse = new QPropertyAnimation(effect, "opacity", this);
        pulse->setDuration(1000);
        pulse->setStartValue(1.0);
        pulse->setKeyValueAt(0.5, 0.25);
        pulse->setEndValue(1.0);
        pulse->setLoopCount(-1);
        pulse->setEasingCurve(QEasingCurve::InOutSine);
        pulse->start();
        m_statusPulse = pulse;
    } else {
        if (m_statusPulse) {
            m_statusPulse->stop();
            m_statusPulse->setTargetObject(nullptr);
            m_statusPulse->deleteLater();
            m_statusPulse = nullptr;
        }
        ui->statusDot->setGraphicsEffect(nullptr);
    }
}

void MainWindow::handleEnvironmentReport()
{
    if (!m_envChecker) {
        return;
    }

    const EnvironmentReport &report = m_envChecker->report();

    if (!report.cliOk) {
        appendLog(I18n::t("log_env_failed"));
        if (report.timedOut) {
            appendLog(I18n::t("log_env_timeout"));
        } else if (!report.error.isEmpty()) {
            appendLog(report.error);
        }
        if (m_statusKey == QLatin1String("checking_env")) {
            setStatus("env_error");
        }
        return;
    }

    appendLog(I18n::t("log_env_export_ok"));
    appendLog(report.quantSupported
                  ? I18n::t("log_env_quant_ok")
                  : I18n::t("log_env_quant_options_missing"));

    if (report.pythonProbeDone) {
        if (!report.pythonVersion.isEmpty()) {
            appendLog(I18n::t("log_env_python_version").arg(report.pythonVersion));
        }

        if (!report.pythonOk) {
            if (report.timedOut) {
                appendLog(I18n::t("log_env_timeout"));
            } else {
                appendLog(I18n::t("log_env_probe_failed").arg(report.error));
            }
        } else {
            if (!report.missingCorePackages.isEmpty()) {
                appendLog(I18n::t("log_env_missing_packages")
                              .arg(report.missingCorePackages.join(QStringLiteral(", "))));
            }
            if (!report.quantBackendAvailable) {
                appendLog(I18n::t("log_env_quant_backend_missing"));
            }
            if (!report.optimumAvailable) {
                appendLog(I18n::t("log_env_optimum_missing"));
            }
        }
    } else {
        appendLog(I18n::t("log_env_probe_skipped"));
    }

    if (report.hasWarnings()) {
        appendLog(I18n::t("log_env_warning"));
        if (m_statusKey == QLatin1String("checking_env")) {
            setStatus("env_warning");
        }
    } else {
        appendLog(I18n::t("log_env_ok"));
        if (m_statusKey == QLatin1String("checking_env")) {
            setStatus("ready");
        }
    }
}

void MainWindow::retranslateUi()
{
    setWindowTitle(I18n::t("app_title"));

    ui->appSubtitleLabel->setText(I18n::t("app_subtitle"));
    ui->modelCardTitle->setText(I18n::t("model_card_title"));
    ui->paramsCardTitle->setText(I18n::t("params_card_title"));
    ui->logCardTitle->setText(I18n::t("log_card_title"));

    ui->label->setText(I18n::t("base_model_path"));
    ui->label_2->setText(I18n::t("lora_path"));
    ui->label_3->setText(I18n::t("save_path"));
    ui->label_5->setText(I18n::t("shard_size_gb"));
    ui->label_7->setText(I18n::t("quant_bits"));
    ui->label_8->setText(I18n::t("quant_dataset"));
    ui->label_6->setText(I18n::t("bin_hint"));

    ui->selectBaseBtn->setText(I18n::t("browse"));
    ui->selectLoraBtn->setText(I18n::t("browse"));
    ui->selectSaveBin->setText(I18n::t("browse"));
    ui->mergeBtn->setText(I18n::t("start_merge"));
    ui->aboutBtn->setText(I18n::t("about_button"));
    ui->QuantHelp->setText(I18n::t("help"));
    ui->chkSafetensors->setText(I18n::t("use_safetensors"));

    ui->labelLanguage->setText(I18n::t("language_label"));
    ui->comboLanguage->setToolTip(I18n::t("language_tooltip"));
    ui->quantDatasetLineEdit->setPlaceholderText(I18n::t("dataset_placeholder"));

    {
        QSignalBlocker blocker(ui->comboQuantBit);
        ui->comboQuantBit->setItemText(0, I18n::t("no_quantization"));
        ui->comboQuantBit->setItemText(1, QStringLiteral("8-bit"));
        ui->comboQuantBit->setItemText(2, QStringLiteral("4-bit"));
    }

    ui->Status->setText(I18n::t(m_statusKey));
    updateThemeUi();
    updateStatusDot();
}

void MainWindow::setStatus(const QString &statusKey)
{
    m_statusKey = statusKey;
    ui->Status->setText(I18n::t(m_statusKey));
    updateStatusDot();
}

void MainWindow::on_comboQuantBit_currentIndexChanged(int index){
    Q_UNUSED(index);
    QString bit=ui->comboQuantBit->currentData().toString();
    bool enable=(bit != "none");
    ui->quantDatasetLineEdit->setEnabled(enable);
}

void MainWindow::on_QuantHelp_clicked(){
    QMessageBox::information(this,
                             I18n::t("quant_help_title"),
                             I18n::t("quant_help_text"));
}
void MainWindow::startMerge(){
    QString basePath=ui->basePathlineEdit->text();
    QString loraPath=ui->loraPathlineEdit->text();
    QString outputPath=ui->modleSavePathlineEdit->text();

    if(m_quantBit!="none"){
        //量化前先确认最终输出目录仍然是空的，避免合并跑完才失败。
        QDir finalDir(m_outputPath);
        if(!finalDir.exists()){
            QMessageBox::warning(this,I18n::t("title_hint"),I18n::t("err_dir_not_exist_save"));
            return;
        }
        if(!finalDir.entryList(QDir::AllEntries|QDir::NoDotAndDotDot).isEmpty()){
            QMessageBox::warning(this,I18n::t("title_hint"),I18n::t("err_empty_save_dir"));
            return;
        }

        outputPath=m_tempPath;
        if(outputPath.isEmpty() || !QDir(outputPath).exists()){
            QMessageBox::critical(this,I18n::t("title_error"),I18n::t("err_temp_dir_failed"));
            return;
        }
    }
    QDir selectedDir(outputPath);
    QStringList entries=selectedDir.entryList(QDir::AllEntries|QDir::NoDotAndDotDot);
    if(!entries.isEmpty()){
        QMessageBox::warning(this,I18n::t("title_hint"),I18n::t("err_empty_save_dir"));
        return;
    }
    QProcess testProcess;
    testProcess.start("llamafactory-cli",{"--help"});
    const bool environmentStarted = testProcess.waitForStarted(5000);
    const bool environmentFinished = environmentStarted && testProcess.waitForFinished(10000);
    if(!environmentStarted || !environmentFinished
        || testProcess.exitStatus()!=QProcess::NormalExit
        || testProcess.exitCode()!=0){
        if(environmentStarted && !environmentFinished){
            testProcess.kill();
            testProcess.waitForFinished(2000);
        }
        QMessageBox::critical(this,I18n::t("title_env_error"),I18n::t("err_no_llamafactory"));
        return;
    }
    bool isSafetensors=ui->chkSafetensors->isChecked();
    int exportSize=ui->spinExportSize->currentData().toInt();
    ui->mergeBtn->setEnabled(false);
    appendLog(I18n::t("log_merge_start"));
    setStatus("merging");
    appendLog(I18n::t("log_base_model")+basePath);
    appendLog(I18n::t("log_lora_path")+loraPath);
    if(m_quantBit!="none"){
        appendLog(I18n::t("log_temp_dir")+outputPath);
    }else{
        appendLog(I18n::t("log_save_to")+outputPath);
    }
    if(isSafetensors){
        appendLog(I18n::t("log_safetensors"));
    }else{
        appendLog(I18n::t("log_bin"));
    }
    appendLog(I18n::t("log_shard_size")+QString::number(exportSize));

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


    connect(mergeProcess,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),[this,mergeProcess,outputPath](int exitCode,QProcess::ExitStatus exitStatus){
        if(exitStatus==QProcess::NormalExit && exitCode==0){
            if(m_quantBit!="none"){
                appendLog(I18n::t("log_merge_complete"));
                startQuantize();
            }else{
                appendLog(I18n::t("log_merge_success"));
                ui->mergeBtn->setEnabled(true);
                appendLog(I18n::t("log_output_dir")+outputPath);
                QMessageBox::information(this,
                                         I18n::t("title_done"),
                                         I18n::t("msg_merge_success")+outputPath);
                setStatus("ready");
            }
        }else{
            ui->mergeBtn->setEnabled(true);
            appendLog(I18n::t("log_merge_failed")+QString::number(exitCode));
            QMessageBox::critical(this,I18n::t("title_error"),I18n::t("msg_merge_failed"));
            setStatus("ready");
        }
        mergeProcess->deleteLater();
    });

    mergeProcess->start("llamafactory-cli",args);
    if(!mergeProcess->waitForStarted(5000)){
        appendLog(I18n::t("log_merge_process_start_failed"));
        ui->mergeBtn->setEnabled(true);
        QMessageBox::critical(this,I18n::t("title_error"),I18n::t("err_start_llamafactory"));
        setStatus("ready");
        mergeProcess->deleteLater();
    }
}
void MainWindow::startQuantize(){
    appendLog(I18n::t("log_quant_start"));
    setStatus("quantizing");
    appendLog(I18n::t("log_quant_dataset")+m_quantDataset);
    appendLog(I18n::t("log_quant_bit")+m_quantBit+QStringLiteral("-bit"));
    QStringList args;
    args<<"export"
         <<"--model_name_or_path"<<m_tempPath
         <<"--export_dir"<<m_outputPath
         <<"--export_size"<<QString::number(m_exportSize)
         <<"--export_legacy_format"<<(m_isSafetensors?"false":"true")
         <<"--export_quantization_bit"<<m_quantBit
         <<"--export_quantization_dataset"<<m_quantDataset;
    QProcess *quantProcess=new QProcess(this);
    quantProcess->setProcessChannelMode(QProcess::MergedChannels);
    //日志
    connect(quantProcess,&QProcess::readyReadStandardOutput,[this,quantProcess](){
        QString output=quantProcess->readAllStandardOutput();
        if(!output.trimmed().isEmpty()){
            appendLog(output);
        }
    });
    connect(quantProcess,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),[this,quantProcess](int exitCode,QProcess::ExitStatus exitStatus){
        ui->mergeBtn->setEnabled(true);
        setStatus("ready");
        if(exitStatus==QProcess::NormalExit && exitCode==0){
            appendLog(I18n::t("log_quant_success"));
            appendLog(I18n::t("log_output_dir")+m_outputPath);
            if(m_tempDir){
                m_tempDir->remove();
            }
            QMessageBox::information(this,
                                     I18n::t("title_done"),
                                     I18n::t("msg_quant_success")+m_outputPath);
        }else{
            appendLog(I18n::t("log_quant_failed")+QString::number(exitCode));
            QMessageBox::critical(this,I18n::t("title_error"),I18n::t("msg_quant_failed"));
        }
        quantProcess->deleteLater();
    });

    quantProcess->start("llamafactory-cli",args);
    if(!quantProcess->waitForStarted(5000)){
        appendLog(I18n::t("log_quant_process_start_failed"));
        ui->mergeBtn->setEnabled(true);
        setStatus("ready");
        QMessageBox::critical(this,I18n::t("title_error"),I18n::t("err_start_llamafactory"));
        quantProcess->deleteLater();
    }
}
void MainWindow::on_selectBaseBtn_clicked(){
    QString dir=QFileDialog::getExistingDirectory(this,I18n::t("dlg_select_base"));
    if(dir.isEmpty()){
        QMessageBox::warning(this,I18n::t("title_hint"),I18n::t("err_selection_cancelled"));
        return;
    }

    if(!QDir(dir).exists()){
        QMessageBox::critical(this,I18n::t("title_problem"),I18n::t("err_dir_not_exist"));
        return;
    }

    QFile configFile(dir+"/config.json");
    if(!configFile.exists()){
        QMessageBox::warning(this,I18n::t("title_hint"),I18n::t("err_not_model_dir"));
        return;
    }
    ui->basePathlineEdit->setText(dir);
}

void MainWindow::on_selectLoraBtn_clicked(){
    QString dir=QFileDialog::getExistingDirectory(this,I18n::t("dlg_select_lora"));
    if(dir.isEmpty()){
        QMessageBox::warning(this,I18n::t("title_hint"),I18n::t("err_selection_cancelled"));
        return;
    }

    if(!QDir(dir).exists()){
        QMessageBox::critical(this,I18n::t("title_problem"),I18n::t("err_dir_not_exist"));
        return;
    }

    QFile configFile(dir+"/adapter_config.json");
    if(!configFile.exists()){
        QMessageBox::warning(this,I18n::t("title_hint"),I18n::t("err_not_lora_dir"));
        return;
    }
    ui->loraPathlineEdit->setText(dir);
}

void MainWindow::on_mergeBtn_clicked(){
    //喜闻乐见的存变量环节
    m_basePath=ui->basePathlineEdit->text();
    m_loraPath=ui->loraPathlineEdit->text();
    m_outputPath=ui->modleSavePathlineEdit->text();
    m_quantDataset=ui->quantDatasetLineEdit->text().trimmed();
    m_quantBit=ui->comboQuantBit->currentData().toString();
    m_exportSize=ui->spinExportSize->currentData().toInt();
    m_isSafetensors=ui->chkSafetensors->isChecked();
    //额啊，Qt怎么查找替换 :)
    QString basePath=ui->basePathlineEdit->text();
    QString loraPath=ui->loraPathlineEdit->text();
    QString outputPath=ui->modleSavePathlineEdit->text();

    if(basePath.isEmpty()){
        QMessageBox::warning(this,I18n::t("title_hint"),I18n::t("err_no_base"));
        return;
    }

    if(loraPath.isEmpty()){
        QMessageBox::warning(this,I18n::t("title_hint"),I18n::t("err_no_lora"));
        return;
    }

    if(outputPath.isEmpty()){
        QMessageBox::warning(this,I18n::t("title_hint"),I18n::t("err_no_output"));
        return;
    }

    //分两步（好折磨啊啊啊啊）
    if(m_quantBit!="none"){
        if(m_quantDataset.isEmpty()){
            QMessageBox::warning(this,I18n::t("title_hint"),I18n::t("err_no_quant_dataset"));
            return;
        }

        //每次量化都用一个全新的临时目录，避免误删用户已有的同名文件夹。
        delete m_tempDir;
        const QString tempTemplate = QDir(m_outputPath).absolutePath() + QStringLiteral("_tmp_XXXXXX");
        m_tempDir = new QTemporaryDir(tempTemplate);
        if(!m_tempDir->isValid()){
            delete m_tempDir;
            m_tempDir = nullptr;
            m_tempPath.clear();
            QMessageBox::critical(this,I18n::t("title_error"),I18n::t("err_temp_dir_failed"));
            return;
        }
        m_tempPath = m_tempDir->path();

        appendLog(I18n::t("log_quant_enabled"));
        appendLog(I18n::t("log_quant_temp_warning"));
    }else{
        delete m_tempDir;
        m_tempDir = nullptr;
        m_tempPath.clear();
        appendLog(I18n::t("log_quant_disabled"));
    }
    startMerge();
}

void MainWindow::appendLog(const QString &message){
    ui->logTextEdit->append(message);
}

void MainWindow::on_aboutBtn_clicked(){
    AboutDialog dialog(this);
    dialog.exec();
}

void MainWindow::on_selectSaveBin_clicked(){
    QString dir=QFileDialog::getExistingDirectory(this,I18n::t("dlg_select_save"));
    if(dir.isEmpty()){
        QMessageBox::warning(this,I18n::t("title_hint"),I18n::t("err_nothing_selected"));
        return;
    }

    if(!QDir(dir).exists()){
        QMessageBox::warning(this,I18n::t("title_hint"),I18n::t("err_dir_not_exist_save"));
        return;
    }

    QDir selectedDir(dir);
    QStringList entries=selectedDir.entryList(QDir::AllEntries|QDir::NoDotAndDotDot);
    if(!entries.isEmpty()){
        QMessageBox::warning(this,I18n::t("title_hint"),I18n::t("err_empty_save_dir"));
        return;
    }
    ui->modleSavePathlineEdit->setText(dir);
}
