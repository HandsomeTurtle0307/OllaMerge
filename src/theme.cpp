/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 HandsomeTurtle0307
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 */
#include "theme.h"

#include <QApplication>
#include <QColor>
#include <QGuiApplication>
#include <QPalette>
#include <QSettings>
#include <QStyle>
#include <QStyleHints>

namespace {

struct ThemeColor
{
    const char *token;
    const char *light;
    const char *dark;
};

const ThemeColor kThemeColors[] = {
    {"@WINDOW_BG@", "rgba(243, 245, 251, 0.55)", "rgba(15, 23, 42, 0.58)"},
    {"@CARD_BG@", "rgba(255, 255, 255, 0.74)", "rgba(30, 41, 59, 0.74)"},
    {"@CARD_BORDER@", "rgba(226, 232, 240, 0.90)", "rgba(71, 85, 105, 0.70)"},
    {"@TEXT@", "#111827", "#f1f5f9"},
    {"@TEXT_SECONDARY@", "#374151", "#cbd5e1"},
    {"@TEXT_MUTED@", "#6b7280", "#94a3b8"},
    {"@FIELD_BG@", "rgba(249, 250, 251, 0.80)", "rgba(15, 23, 42, 0.60)"},
    {"@FIELD_BORDER@", "rgba(203, 213, 225, 0.95)", "rgba(71, 85, 105, 0.95)"},
    {"@FIELD_TEXT@", "#111827", "#e5e7eb"},
    {"@FIELD_DISABLED_BG@", "rgba(241, 245, 249, 0.55)", "rgba(15, 23, 42, 0.40)"},
    {"@FIELD_DISABLED_TEXT@", "#94a3b8", "#64748b"},

    // Light mode: deep emerald gradient with white foreground.
    // Dark mode: bright mint gradient with dark-green foreground.
    {"@ACCENT_START@", "#059669", "#34d399"},
    {"@ACCENT_END@", "#0f766e", "#2dd4bf"},
    {"@ACCENT_HOVER_START@", "#047857", "#6ee7b7"},
    {"@ACCENT_HOVER_END@", "#115e59", "#5eead4"},
    {"@ACCENT_PRESSED_START@", "#065f46", "#10b981"},
    {"@ACCENT_PRESSED_END@", "#134e4a", "#14b8a6"},
    {"@ON_ACCENT@", "#ffffff", "#052e16"},
    {"@ON_ACCENT_MUTED@", "#d1fae5", "#065f46"},

    {"@BUTTON_BG@", "rgba(255, 255, 255, 0.70)", "rgba(30, 41, 59, 0.70)"},
    {"@BUTTON_BORDER@", "rgba(203, 213, 225, 0.95)", "rgba(71, 85, 105, 0.95)"},
    {"@BUTTON_TEXT@", "#374151", "#cbd5e1"},
    {"@BUTTON_HOVER_BG@", "#ecfdf5", "rgba(6, 78, 59, 0.55)"},
    {"@BUTTON_HOVER_BORDER@", "#6ee7b7", "#34d399"},
    {"@BUTTON_HOVER_TEXT@", "#047857", "#d1fae5"},
    {"@BUTTON_PRESSED_BG@", "#d1fae5", "rgba(4, 120, 87, 0.55)"},
    {"@BUTTON_DISABLED_BG@", "rgba(241, 245, 249, 0.55)", "rgba(30, 41, 59, 0.40)"},
    {"@BUTTON_DISABLED_TEXT@", "#94a3b8", "#64748b"},
    {"@MERGE_DISABLED_BG@", "rgba(167, 243, 208, 0.80)", "rgba(52, 211, 153, 0.25)"},
    {"@MERGE_DISABLED_TEXT@", "#047857", "#a7f3d0"},
    {"@LOG_BG@", "rgba(15, 23, 42, 0.92)", "rgba(2, 6, 23, 0.88)"},
    {"@LOG_BORDER@", "rgba(30, 41, 59, 0.90)", "rgba(51, 65, 85, 0.90)"},
    {"@LOG_TEXT@", "#e2e8f0", "#e2e8f0"},
    {"@SCROLL_HANDLE@", "#334155", "#475569"},
    {"@SCROLL_HANDLE_HOVER@", "#475569", "#64748b"},
    {"@ABOUT_BG@", "rgba(255, 255, 255, 0.85)", "rgba(15, 23, 42, 0.88)"},
    {"@ABOUT_BODY@", "#374151", "#cbd5e1"},
    {"@ABOUT_META@", "#94a3b8", "#94a3b8"},
    {"@LINK_BG@", "#ecfdf5", "rgba(6, 78, 59, 0.45)"},
    {"@LINK_BORDER@", "#a7f3d0", "rgba(52, 211, 153, 0.45)"},
    {"@LINK_TEXT@", "#047857", "#6ee7b7"},
    {"@VERSION_BG@", "#ecfdf5", "rgba(6, 78, 59, 0.55)"},
    {"@VERSION_BORDER@", "#a7f3d0", "rgba(52, 211, 153, 0.45)"},
    {"@VERSION_TEXT@", "#047857", "#6ee7b7"},
    {"@DIALOG_BG@", "#ffffff", "#1e293b"},
    {"@TOOLTIP_BG@", "#111827", "#f8fafc"},
    {"@TOOLTIP_TEXT@", "#f9fafb", "#0f172a"},
    {"@SELECTION@", "#10b981", "#34d399"},
    {"@SELECTION_TEXT@", "#ffffff", "#052e16"},
};

QPalette darkPalette()
{
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(15, 23, 42));
    palette.setColor(QPalette::WindowText, QColor(241, 245, 249));
    palette.setColor(QPalette::Base, QColor(15, 23, 42));
    palette.setColor(QPalette::AlternateBase, QColor(30, 41, 59));
    palette.setColor(QPalette::Text, QColor(229, 231, 235));
    palette.setColor(QPalette::Button, QColor(30, 41, 59));
    palette.setColor(QPalette::ButtonText, QColor(241, 245, 249));
    palette.setColor(QPalette::BrightText, QColor(255, 255, 255));
    palette.setColor(QPalette::Highlight, QColor(16, 185, 129));
    palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
    palette.setColor(QPalette::ToolTipBase, QColor(248, 250, 252));
    palette.setColor(QPalette::ToolTipText, QColor(15, 23, 42));
    palette.setColor(QPalette::PlaceholderText, QColor(148, 163, 184));
    return palette;
}

} // namespace

QString Theme::styleSheet(Mode mode)
{
    QString sheet = QString::fromUtf8(R"QSS(
/* ---------- Window ---------- */
QMainWindow {
    background-color: transparent;
}

QWidget#centralwidget {
    background-color: @WINDOW_BG@;
}

/* ---------- Header ---------- */
QLabel#appTitleLabel {
    color: @TEXT@;
    font-size: 26px;
    font-weight: bold;
}

QLabel#appVersionLabel {
    color: @VERSION_TEXT@;
    background-color: @VERSION_BG@;
    border: 1px solid @VERSION_BORDER@;
    border-radius: 10px;
    padding: 2px 8px;
    font-size: 9pt;
    font-weight: bold;
}

QLabel#appSubtitleLabel {
    color: @TEXT_MUTED@;
    font-size: 9pt;
}

QLabel#labelLanguage {
    color: @TEXT_MUTED@;
    font-size: 9pt;
}

QPushButton#themeBtn {
    background-color: @BUTTON_BG@;
    border: 1px solid @BUTTON_BORDER@;
    border-radius: 15px;
    padding: 0px;
    color: @BUTTON_TEXT@;
    font-size: 14pt;
}

QPushButton#themeBtn:hover {
    background-color: @BUTTON_HOVER_BG@;
    border-color: @BUTTON_HOVER_BORDER@;
    color: @BUTTON_HOVER_TEXT@;
}

QPushButton#themeBtn:pressed {
    background-color: @BUTTON_PRESSED_BG@;
}

/* ---------- Cards ---------- */
QFrame#modelCard, QFrame#paramsCard, QFrame#logCard {
    background-color: @CARD_BG@;
    border: 1px solid @CARD_BORDER@;
    border-radius: 14px;
}

QLabel#modelCardTitle, QLabel#paramsCardTitle, QLabel#logCardTitle {
    color: @TEXT@;
    font-size: 11pt;
    font-weight: bold;
    padding-bottom: 2px;
}

/* ---------- Form labels ---------- */
QLabel#label, QLabel#label_2, QLabel#label_3,
QLabel#label_5, QLabel#label_7, QLabel#label_8 {
    color: @TEXT_SECONDARY@;
    font-weight: bold;
}

QLabel#label_6 {
    color: @TEXT_MUTED@;
    font-size: 8.5pt;
}

/* ---------- Inputs ---------- */
QLineEdit, QComboBox {
    background-color: @FIELD_BG@;
    border: 1px solid @FIELD_BORDER@;
    border-radius: 8px;
    padding: 5px 10px;
    min-height: 18px;
    color: @FIELD_TEXT@;
    selection-background-color: @SELECTION@;
    selection-color: @SELECTION_TEXT@;
}

QLineEdit:focus, QComboBox:focus {
    background-color: @DIALOG_BG@;
    border: 1px solid @SELECTION@;
}

QLineEdit:disabled {
    background-color: @FIELD_DISABLED_BG@;
    color: @FIELD_DISABLED_TEXT@;
}

QComboBox {
    padding-right: 26px;
}

QComboBox::drop-down {
    border: none;
    width: 24px;
}

QComboBox QAbstractItemView {
    background-color: @DIALOG_BG@;
    color: @TEXT@;
    border: 1px solid @FIELD_BORDER@;
    border-radius: 8px;
    padding: 4px;
    selection-background-color: @LINK_BG@;
    selection-color: @LINK_TEXT@;
}

/* ---------- Buttons ---------- */
QPushButton {
    background-color: @BUTTON_BG@;
    border: 1px solid @BUTTON_BORDER@;
    border-radius: 8px;
    padding: 6px 14px;
    min-height: 18px;
    color: @BUTTON_TEXT@;
}

QPushButton:hover {
    background-color: @BUTTON_HOVER_BG@;
    border-color: @BUTTON_HOVER_BORDER@;
    color: @BUTTON_HOVER_TEXT@;
}

QPushButton:pressed {
    background-color: @BUTTON_PRESSED_BG@;
}

QPushButton:disabled {
    background-color: @BUTTON_DISABLED_BG@;
    border-color: @CARD_BORDER@;
    color: @BUTTON_DISABLED_TEXT@;
}

QPushButton#mergeBtn {
    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                      stop:0 @ACCENT_START@, stop:1 @ACCENT_END@);
    color: @ON_ACCENT@;
    border: none;
    border-radius: 12px;
    padding: 10px 32px;
    font-size: 11pt;
    font-weight: bold;
}

QPushButton#mergeBtn:hover {
    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                      stop:0 @ACCENT_HOVER_START@, stop:1 @ACCENT_HOVER_END@);
}

QPushButton#mergeBtn:pressed {
    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                      stop:0 @ACCENT_PRESSED_START@, stop:1 @ACCENT_PRESSED_END@);
}

QPushButton#mergeBtn:disabled {
    background-color: @MERGE_DISABLED_BG@;
    color: @MERGE_DISABLED_TEXT@;
}

QPushButton#aboutBtn {
    background-color: transparent;
    border: none;
    color: @LINK_TEXT@;
    padding: 4px 10px;
}

QPushButton#aboutBtn:hover {
    background-color: @LINK_BG@;
    border-radius: 6px;
    color: @LINK_TEXT@;
}

/* ---------- Checkbox / status ---------- */
QCheckBox {
    color: @TEXT_SECONDARY@;
    spacing: 8px;
}

QLabel#Status {
    color: @TEXT_MUTED@;
    font-size: 9pt;
    padding-left: 2px;
}

QLabel#statusDot {
    background-color: #10b981;
    border-radius: 4px;
}

/* ---------- Log console ---------- */
QTextEdit#logTextEdit {
    background-color: @LOG_BG@;
    color: @LOG_TEXT@;
    border: 1px solid @LOG_BORDER@;
    border-radius: 10px;
    padding: 10px;
    font-family: "Consolas";
    font-size: 9pt;
    selection-background-color: @SELECTION@;
    selection-color: @SELECTION_TEXT@;
}

QTextEdit#logTextEdit QScrollBar:vertical {
    background: transparent;
    width: 10px;
    margin: 6px 2px 6px 0;
}

QTextEdit#logTextEdit QScrollBar::handle:vertical {
    background: @SCROLL_HANDLE@;
    min-height: 24px;
    border-radius: 4px;
}

QTextEdit#logTextEdit QScrollBar::handle:vertical:hover {
    background: @SCROLL_HANDLE_HOVER@;
}

QTextEdit#logTextEdit QScrollBar::add-line:vertical,
QTextEdit#logTextEdit QScrollBar::sub-line:vertical {
    height: 0px;
}

QTextEdit#logTextEdit QScrollBar::add-page:vertical,
QTextEdit#logTextEdit QScrollBar::sub-page:vertical {
    background: transparent;
}

/* ---------- Message boxes ---------- */
QMessageBox, QInputDialog {
    background-color: @DIALOG_BG@;
}

QMessageBox QLabel, QInputDialog QLabel {
    color: @TEXT@;
}

/* ---------- About dialog ---------- */
QDialog#AboutDialog {
    background-color: @ABOUT_BG@;
}

QFrame#aboutHeader {
    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                      stop:0 @ACCENT_START@, stop:1 @ACCENT_END@);
    border-radius: 14px;
}

QLabel#aboutTitle {
    color: @ON_ACCENT@;
    font-size: 26px;
    font-weight: bold;
}

QLabel#aboutVersion {
    color: @ON_ACCENT_MUTED@;
    font-size: 10pt;
    font-weight: bold;
}

QLabel#aboutTagline {
    color: @ON_ACCENT_MUTED@;
    font-size: 9pt;
}

QLabel#aboutDescription {
    color: @ABOUT_BODY@;
    font-size: 10pt;
}

QLabel#aboutMeta {
    color: @ABOUT_META@;
    font-size: 8.5pt;
}

QLabel#aboutLogo {
    background-color: rgba(255, 255, 255, 0.90);
    border: 1px solid rgba(255, 255, 255, 0.65);
    border-radius: 18px;
}

QLabel#aboutSectionTitle {
    color: @TEXT_MUTED@;
    font-size: 8.5pt;
    font-weight: bold;
}

QLabel#aboutEnvironment, QLabel#aboutCreditName {
    color: @TEXT_MUTED@;
    font-size: 9pt;
}

QPushButton#aboutLinkButton {
    background-color: @LINK_BG@;
    border: 1px solid @LINK_BORDER@;
    border-radius: 8px;
    padding: 8px 14px;
    color: @LINK_TEXT@;
}

QPushButton#aboutLinkButton:hover {
    background-color: @BUTTON_HOVER_BG@;
    border-color: @BUTTON_HOVER_BORDER@;
}

QPushButton#aboutCloseButton {
    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                      stop:0 @ACCENT_START@, stop:1 @ACCENT_END@);
    color: @ON_ACCENT@;
    border: none;
    border-radius: 9px;
    padding: 8px 24px;
    font-weight: bold;
}

QPushButton#aboutCloseButton:hover {
    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                      stop:0 @ACCENT_HOVER_START@, stop:1 @ACCENT_HOVER_END@);
}

/* ---------- Tooltip ---------- */
QToolTip {
    background-color: @TOOLTIP_BG@;
    color: @TOOLTIP_TEXT@;
    border: none;
    padding: 6px 8px;
}
)QSS");

    for (const ThemeColor &color : kThemeColors) {
        sheet.replace(QString::fromLatin1(color.token),
                      QString::fromLatin1(mode == Theme::Dark ? color.dark : color.light));
    }
    return sheet;
}

ThemeManager &ThemeManager::instance()
{
    static ThemeManager manager;
    return manager;
}

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
    , m_mode(Theme::Light)
{
    QSettings settings(QStringLiteral("HandsomeTurtle0307"), QStringLiteral("OllaMerge"));
    const QString saved = settings.value(QStringLiteral("ui/theme")).toString();

    if (saved == QLatin1String("dark")) {
        m_mode = Theme::Dark;
    } else if (saved == QLatin1String("light")) {
        m_mode = Theme::Light;
    } else if (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark) {
        m_mode = Theme::Dark;
    } else {
        m_mode = Theme::Light;
    }
}

Theme::Mode ThemeManager::mode() const
{
    return m_mode;
}

bool ThemeManager::isDark() const
{
    return m_mode == Theme::Dark;
}

void ThemeManager::setMode(Theme::Mode mode)
{
    if (m_mode == mode) {
        return;
    }

    m_mode = mode;

    QSettings settings(QStringLiteral("HandsomeTurtle0307"), QStringLiteral("OllaMerge"));
    settings.setValue(QStringLiteral("ui/theme"),
                      mode == Theme::Dark ? QStringLiteral("dark") : QStringLiteral("light"));

    apply();
    emit modeChanged();
}

void ThemeManager::toggle()
{
    setMode(m_mode == Theme::Dark ? Theme::Light : Theme::Dark);
}

void ThemeManager::apply() const
{
    if (!qApp) {
        return;
    }

    if (m_mode == Theme::Dark) {
        qApp->setPalette(darkPalette());
    } else if (QStyle *style = QApplication::style()) {
        qApp->setPalette(style->standardPalette());
    }

    qApp->setStyleSheet(Theme::styleSheet(m_mode));
}
