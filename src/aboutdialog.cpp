/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 HandsomeTurtle0307
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 */
#include "aboutdialog.h"
#include "appicon.h"
#include "appversion.h"
#include "brandicons.h"
#include "language.h"
#include "theme.h"
#include "windoweffects.h"

#include <QDesktopServices>
#include <QEasingCurve>
#include <QFrame>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QLibraryInfo>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QShowEvent>
#include <QSizePolicy>
#include <QSysInfo>
#include <QUrl>
#include <QVBoxLayout>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setObjectName(QStringLiteral("AboutDialog"));
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowTitle(I18n::t("about_title"));
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setModal(true);
    setFixedSize(580, 540);

    const bool dark = ThemeManager::instance().isDark();

    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(14, 14, 14, 14);
    rootLayout->setSpacing(8);

    // ---------- Header ----------
    auto *header = new QFrame(this);
    header->setObjectName(QStringLiteral("aboutHeader"));
    header->setMinimumHeight(130);

    auto *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(20, 16, 20, 16);
    headerLayout->setSpacing(16);

    auto *logoLabel = new QLabel(header);
    logoLabel->setObjectName(QStringLiteral("aboutLogo"));
    logoLabel->setFixedSize(60, 60);
    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setPixmap(AppIcon::create(46, dark));

    auto *titleLayout = new QVBoxLayout;
    titleLayout->setSpacing(3);

    auto *titleLabel = new QLabel(QStringLiteral("OllaMerge"), header);
    titleLabel->setObjectName(QStringLiteral("aboutTitle"));

    auto *versionLabel = new QLabel(QStringLiteral("v") + appVersionString(), header);
    versionLabel->setObjectName(QStringLiteral("aboutVersion"));

    auto *taglineLabel = new QLabel(I18n::t("app_subtitle"), header);
    taglineLabel->setObjectName(QStringLiteral("aboutTagline"));
    taglineLabel->setWordWrap(true);

    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(versionLabel);
    titleLayout->addWidget(taglineLabel);
    titleLayout->addStretch();

    headerLayout->addWidget(logoLabel, 0, Qt::AlignVCenter);
    headerLayout->addLayout(titleLayout, 1);

    // ---------- Intro ----------
    auto *descriptionLabel = new QLabel(I18n::t("about_description"), this);
    descriptionLabel->setObjectName(QStringLiteral("aboutDescription"));
    descriptionLabel->setWordWrap(true);

    auto *authorLabel = new QLabel(I18n::t("about_author"), this);
    authorLabel->setObjectName(QStringLiteral("aboutMeta"));

    // ---------- Links ----------
    auto *linksTitle = new QLabel(I18n::t("about_links_title"), this);
    linksTitle->setObjectName(QStringLiteral("aboutSectionTitle"));

    auto *linksLayout = new QHBoxLayout;
    linksLayout->setSpacing(8);

    const auto addLink = [this, linksLayout](const QString &text, const QString &url) {
        auto *button = new QPushButton(text, this);
        button->setObjectName(QStringLiteral("aboutLinkButton"));
        button->setCursor(Qt::PointingHandCursor);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        connect(button, &QPushButton::clicked, this, [url]() {
            QDesktopServices::openUrl(QUrl(url));
        });
        linksLayout->addWidget(button);
    };

    addLink(I18n::t("about_github_repo"),
            QStringLiteral("https://github.com/HandsomeTurtle0307/OllaMerge"));
    addLink(I18n::t("about_github_issues"),
            QStringLiteral("https://github.com/HandsomeTurtle0307/OllaMerge/issues"));
    addLink(I18n::t("about_github_releases"),
            QStringLiteral("https://github.com/HandsomeTurtle0307/OllaMerge/releases"));

    // ---------- Environment ----------
    auto *environmentTitle = new QLabel(I18n::t("about_environment_title"), this);
    environmentTitle->setObjectName(QStringLiteral("aboutSectionTitle"));

    QString environmentText = QStringLiteral("Qt %1 · %2");
    environmentText = environmentText.arg(QLibraryInfo::version().toString())
                                     .arg(QSysInfo::prettyProductName());
    auto *environmentLabel = new QLabel(environmentText, this);
    environmentLabel->setObjectName(QStringLiteral("aboutEnvironment"));
    environmentLabel->setWordWrap(true);
    environmentLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    // ---------- Credits ----------
    auto *creditsTitle = new QLabel(I18n::t("about_credits_title"), this);
    creditsTitle->setObjectName(QStringLiteral("aboutSectionTitle"));

    auto *creditsLayout = new QHBoxLayout;
    creditsLayout->setSpacing(14);

    const auto addCredit = [this, creditsLayout](const QPixmap &icon, const QString &text) {
        auto *itemLayout = new QHBoxLayout;
        itemLayout->setSpacing(6);

        auto *iconLabel = new QLabel(this);
        iconLabel->setFixedSize(18, 18);
        iconLabel->setAlignment(Qt::AlignCenter);
        iconLabel->setScaledContents(true);
        iconLabel->setPixmap(icon);

        auto *nameLabel = new QLabel(text, this);
        nameLabel->setObjectName(QStringLiteral("aboutCreditName"));

        itemLayout->addWidget(iconLabel);
        itemLayout->addWidget(nameLabel);
        creditsLayout->addLayout(itemLayout);
    };

    addCredit(BrandIcons::qt(18), QStringLiteral("Qt for Open Source Use"));
    addCredit(BrandIcons::llamaFactory(18), QStringLiteral("LLaMA-Factory"));
    addCredit(BrandIcons::deepSeek(18), QStringLiteral("DeepSeek Harness"));
    creditsLayout->addStretch();

    // ---------- Footer ----------
    auto *footerLayout = new QHBoxLayout;
    footerLayout->setSpacing(10);

    auto *licenseLabel =
        new QLabel(QStringLiteral("© 2026 HandsomeTurtle0307 · GPL-3.0-or-later"), this);
    licenseLabel->setObjectName(QStringLiteral("aboutMeta"));

    auto *closeButton = new QPushButton(I18n::t("about_close"), this);
    closeButton->setObjectName(QStringLiteral("aboutCloseButton"));
    closeButton->setCursor(Qt::PointingHandCursor);
    closeButton->setDefault(true);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

    footerLayout->addWidget(licenseLabel);
    footerLayout->addStretch();
    footerLayout->addWidget(closeButton);

    // ---------- Assemble ----------
    rootLayout->addWidget(header);
    rootLayout->addWidget(descriptionLabel);
    rootLayout->addWidget(authorLabel);
    rootLayout->addWidget(linksTitle);
    rootLayout->addLayout(linksLayout);
    rootLayout->addWidget(environmentTitle);
    rootLayout->addWidget(environmentLabel);
    rootLayout->addWidget(creditsTitle);
    rootLayout->addLayout(creditsLayout);
    rootLayout->addStretch();
    rootLayout->addLayout(footerLayout);

    m_headerEffect = new QGraphicsOpacityEffect;
    m_headerEffect->setOpacity(0.0);
    header->setGraphicsEffect(m_headerEffect);

    WindowEffects::apply(this, dark);
}

void AboutDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    WindowEffects::apply(this, ThemeManager::instance().isDark());

    if (m_headerEffect) {
        auto *fadeIn = new QPropertyAnimation(m_headerEffect, "opacity", this);
        fadeIn->setDuration(260);
        fadeIn->setStartValue(0.0);
        fadeIn->setEndValue(1.0);
        fadeIn->setEasingCurve(QEasingCurve::OutCubic);
        fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
        m_headerEffect = nullptr;
    }
}
