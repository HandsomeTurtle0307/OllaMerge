/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 HandsomeTurtle0307
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 */
#ifndef ENVIRONMENTCHECKER_H
#define ENVIRONMENTCHECKER_H

#include <QByteArray>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>

class QTimer;

struct EnvironmentReport
{
    bool cliOk = false;
    bool quantSupported = false;
    bool pythonProbeDone = false;
    bool pythonOk = false;
    QString pythonVersion;
    QStringList missingCorePackages;
    QStringList missingQuantPackages;
    bool quantBackendAvailable = false;
    bool optimumAvailable = false;
    bool timedOut = false;
    QString error;

    bool hasWarnings() const;
};

/*
 * Runs a few checks in sequence without blocking the GUI:
 *   1. llamafactory-cli export --help  (Python + core deps + merge/quant CLI)
 *   2. the Python interpreter behind the CLI, probing installed packages
 *      (torch/transformers/peft/... and the GPTQ export backend).
 */
class EnvironmentChecker : public QObject
{
    Q_OBJECT

public:
    explicit EnvironmentChecker(QObject *parent = nullptr);
    ~EnvironmentChecker() override;

    void start();
    const EnvironmentReport &report() const;

signals:
    void finished();

private:
    enum class Step {
        Idle,
        ExportHelp,
        PythonProbe
    };

    void startProcess(const QString &program, const QStringList &arguments,
                      int timeoutMs, Step step);
    void handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void handleFailedToStart();
    void handleTimeout();
    void parsePythonProbe();
    QString findPythonExecutable() const;
    void cleanupProcess();
    void finish();

    Step m_step = Step::Idle;
    EnvironmentReport m_report;
    QProcess *m_process = nullptr;
    QTimer *m_timer = nullptr;
    QByteArray m_output;
    QString m_cliPath;
    bool m_done = false;
};

#endif // ENVIRONMENTCHECKER_H
