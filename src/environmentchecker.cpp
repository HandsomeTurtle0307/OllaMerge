/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 HandsomeTurtle0307
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 */
#include "environmentchecker.h"

#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QProcess>
#include <QStandardPaths>
#include <QTimer>

namespace {

const char *kProbeScript = R"PY(
import importlib.util, json, sys

modules = [
    "torch", "transformers", "peft", "safetensors", "accelerate", "datasets",
    "gptqmodel", "auto_gptq", "optimum",
]
result = {}
for name in modules:
    try:
        result[name] = importlib.util.find_spec(name) is not None
    except Exception:
        result[name] = False

print("OLLAMERGE_ENV_JSON:" + json.dumps({"python": sys.version.split()[0], "modules": result}))
)PY";

} // namespace

bool EnvironmentReport::hasWarnings() const
{
    if (!cliOk) {
        return false;
    }

    return !quantSupported
        || !missingCorePackages.isEmpty()
        || !missingQuantPackages.isEmpty()
        || !pythonProbeDone
        || !pythonOk;
}

EnvironmentChecker::EnvironmentChecker(QObject *parent)
    : QObject(parent)
{
}

EnvironmentChecker::~EnvironmentChecker()
{
    if (m_timer) {
        m_timer->stop();
    }
    if (m_process) {
        QObject::disconnect(m_process, nullptr, this, nullptr);
        if (m_process->state() != QProcess::NotRunning) {
            m_process->kill();
            m_process->waitForFinished(1000);
        }
    }
}

void EnvironmentChecker::start()
{
    if (m_process) {
        return;
    }

    m_report = EnvironmentReport();
    m_done = false;
    m_output.clear();
    m_cliPath = QStandardPaths::findExecutable(QStringLiteral("llamafactory-cli"));

    if (m_cliPath.isEmpty()) {
        m_report.error = QStringLiteral("llamafactory-cli not found");
        finish();
        return;
    }

    startProcess(m_cliPath,
                 {QStringLiteral("export"), QStringLiteral("--help")},
                 20000,
                 Step::ExportHelp);
}

const EnvironmentReport &EnvironmentChecker::report() const
{
    return m_report;
}

void EnvironmentChecker::startProcess(const QString &program,
                                      const QStringList &arguments,
                                      int timeoutMs,
                                      Step step)
{
    cleanupProcess();

    m_step = step;
    m_output.clear();
    QProcess *process = new QProcess(this);
    m_process = process;
    process->setProcessChannelMode(QProcess::MergedChannels);

    connect(process, &QProcess::readyReadStandardOutput, this, [this, process]() {
        if (process == m_process) {
            m_output += process->readAllStandardOutput();
        }
    });
    connect(process, &QProcess::errorOccurred, this,
            [this, process](QProcess::ProcessError error) {
                if (process == m_process && error == QProcess::FailedToStart) {
                    handleFailedToStart();
                }
            });
    connect(process,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            [this, process](int exitCode, QProcess::ExitStatus exitStatus) {
                if (process == m_process) {
                    handleProcessFinished(exitCode, exitStatus);
                }
            });

    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &EnvironmentChecker::handleTimeout);
    m_timer->start(timeoutMs);

    process->start(program, arguments);
}

void EnvironmentChecker::handleProcessFinished(int exitCode,
                                               QProcess::ExitStatus exitStatus)
{
    if (!m_process) {
        return;
    }

    const Step step = m_step;
    QProcess *process = m_process;
    m_process = nullptr;

    if (m_timer) {
        m_timer->stop();
        m_timer->deleteLater();
        m_timer = nullptr;
    }

    m_output += process->readAllStandardOutput();
    process->deleteLater();

    const QString output = QString::fromUtf8(m_output).trimmed();

    if (step == Step::ExportHelp) {
        if (exitStatus != QProcess::NormalExit || exitCode != 0) {
            m_report.cliOk = false;
            m_report.error = output.left(600);
            m_step = Step::Idle;
            finish();
            return;
        }

        m_report.cliOk = true;
        m_report.quantSupported =
            output.contains(QStringLiteral("--export_quantization_bit"))
            && output.contains(QStringLiteral("--export_quantization_dataset"));

        const QString python = findPythonExecutable();
        if (python.isEmpty()) {
            m_report.pythonProbeDone = false;
            m_step = Step::Idle;
            finish();
            return;
        }

        startProcess(python,
                     {QStringLiteral("-c"), QString::fromUtf8(kProbeScript)},
                     15000,
                     Step::PythonProbe);
        return;
    }

    if (step == Step::PythonProbe) {
        m_report.pythonProbeDone = true;
        if (exitStatus == QProcess::NormalExit && exitCode == 0) {
            parsePythonProbe();
        } else {
            m_report.pythonOk = false;
            m_report.error = output.left(400);
        }
        m_step = Step::Idle;
        finish();
    }
}

void EnvironmentChecker::handleFailedToStart()
{
    if (!m_process) {
        return;
    }

    if (m_step == Step::ExportHelp) {
        m_report.cliOk = false;
        m_report.error = QStringLiteral("Failed to start llamafactory-cli");
    } else if (m_step == Step::PythonProbe) {
        m_report.pythonProbeDone = true;
        m_report.pythonOk = false;
        m_report.error = QStringLiteral("Failed to start Python");
    }

    m_step = Step::Idle;
    finish();
}

void EnvironmentChecker::handleTimeout()
{
    if (sender() != m_timer || !m_process) {
        return;
    }

    m_report.timedOut = true;
    const Step step = m_step;
    QProcess *timedProcess = m_process;

    timedProcess->kill();
    timedProcess->waitForFinished(1000);

    if (m_process != timedProcess) {
        return;
    }

    if (step == Step::ExportHelp) {
        m_report.cliOk = false;
        m_report.error = QStringLiteral("timeout");
    } else if (step == Step::PythonProbe) {
        m_report.pythonProbeDone = true;
        m_report.pythonOk = false;
        m_report.error = QStringLiteral("timeout");
    }

    m_step = Step::Idle;
    finish();
}

void EnvironmentChecker::parsePythonProbe()
{
    const QString text = QString::fromUtf8(m_output);
    const QString marker = QStringLiteral("OLLAMERGE_ENV_JSON:");
    const int markerPos = text.lastIndexOf(marker);

    if (markerPos < 0) {
        m_report.pythonOk = false;
        m_report.error = QStringLiteral("invalid probe output");
        return;
    }

    const QString jsonLine = text.mid(markerPos + marker.size())
                                 .section(QLatin1Char('\n'), 0, 0)
                                 .trimmed();
    const QJsonDocument document = QJsonDocument::fromJson(jsonLine.toUtf8());
    if (!document.isObject()) {
        m_report.pythonOk = false;
        m_report.error = QStringLiteral("invalid probe output");
        return;
    }

    const QJsonObject root = document.object();
    const QJsonObject modules = root.value(QStringLiteral("modules")).toObject();
    m_report.pythonVersion = root.value(QStringLiteral("python")).toString();

    const QStringList corePackages = {
        QStringLiteral("torch"),
        QStringLiteral("transformers"),
        QStringLiteral("peft"),
        QStringLiteral("safetensors"),
        QStringLiteral("accelerate"),
        QStringLiteral("datasets"),
    };
    for (const QString &package : corePackages) {
        if (!modules.value(package).toBool()) {
            m_report.missingCorePackages.append(package);
        }
    }

    m_report.quantBackendAvailable =
        modules.value(QStringLiteral("gptqmodel")).toBool()
        || modules.value(QStringLiteral("auto_gptq")).toBool();
    m_report.optimumAvailable = modules.value(QStringLiteral("optimum")).toBool();

    if (!m_report.quantBackendAvailable) {
        m_report.missingQuantPackages.append(QStringLiteral("gptqmodel / auto-gptq"));
    }
    if (!m_report.optimumAvailable) {
        m_report.missingQuantPackages.append(QStringLiteral("optimum"));
    }

    m_report.pythonOk = true;
}

QString EnvironmentChecker::findPythonExecutable() const
{
    if (m_cliPath.isEmpty()) {
        return QString();
    }

    const QDir cliDir = QFileInfo(m_cliPath).absoluteDir();
    QStringList candidates;

#ifdef Q_OS_WIN
    candidates << cliDir.absoluteFilePath(QStringLiteral("../python.exe"))
               << cliDir.absoluteFilePath(QStringLiteral("python.exe"))
               << cliDir.absoluteFilePath(QStringLiteral("../python3.exe"));
#else
    candidates << cliDir.absoluteFilePath(QStringLiteral("../bin/python3"))
               << cliDir.absoluteFilePath(QStringLiteral("../bin/python"))
               << cliDir.absoluteFilePath(QStringLiteral("python3"))
               << cliDir.absoluteFilePath(QStringLiteral("python"));
#endif

    for (const QString &candidate : candidates) {
        const QFileInfo info(candidate);
        if (info.exists() && info.isFile()) {
            return candidate;
        }
    }

    return QString();
}

void EnvironmentChecker::cleanupProcess()
{
    if (m_timer) {
        m_timer->stop();
        m_timer->deleteLater();
        m_timer = nullptr;
    }

    if (m_process) {
        QObject::disconnect(m_process, nullptr, this, nullptr);
        if (m_process->state() != QProcess::NotRunning) {
            m_process->kill();
            m_process->waitForFinished(1000);
        }
        m_process->deleteLater();
        m_process = nullptr;
    }
}

void EnvironmentChecker::finish()
{
    if (m_done) {
        return;
    }
    m_done = true;
    m_step = Step::Idle;
    cleanupProcess();
    emit finished();
}
