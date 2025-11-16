/*
 *  SPDX-FileCopyrightText: 2025 Daniele Mte90 Scasciafratte <mte90net@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QTextEdit>
#include <QVBoxLayout>

#include "src/plugin.h"
#include "src/settings.h"
#include "src/ollama/ollamasystem.h"

KateOllamaConfigPage::KateOllamaConfigPage(QWidget* parent, KateOllamaPlugin* plugin)
  : KTextEditor::ConfigPage(parent), plugin_(plugin) {
  QVBoxLayout* layout = new QVBoxLayout(this);

  // URL
  {
    auto* hl = new QHBoxLayout;

    auto label = new QLabel(i18n("Ollama URL"));
    hl->addWidget(label);

    ollamaURLText_ = new QLineEdit(this);
    hl->addWidget(ollamaURLText_);

    layout->addLayout(hl);
  }

  // Available Models
  {
    auto* hl = new QHBoxLayout;

    auto label = new QLabel(i18n("Available Models"));
    hl->addWidget(label);

    modelsComboBox_ = new QComboBox(this);
    hl->addWidget(modelsComboBox_);

    layout->addLayout(hl);
  }

  // System Prompt
  {
    auto* hl = new QHBoxLayout;

    auto label = new QLabel(i18n("System Prompt"));
    hl->addWidget(label);

    systemPromptEdit_ = new QTextEdit(this);
    systemPromptEdit_->setGeometry(100, 100, 300, 200);
    hl->addWidget(systemPromptEdit_);

    layout->addLayout(hl);
  }

  layout->addStretch();

  // Error/Info label
  {
    infoLabel_ = new QLabel(this);
    infoLabel_->setVisible(false); // its hidden initially
    infoLabel_->setWordWrap(true);
    layout->addWidget(infoLabel_);
  }

  setLayout(layout);

  QObject::connect(modelsComboBox_, &QComboBox::currentIndexChanged, this,
                   &KateOllamaConfigPage::changed);
  QObject::connect(systemPromptEdit_, &QTextEdit::textChanged, this,
                   &KateOllamaConfigPage::changed);
  QObject::connect(ollamaURLText_, &QLineEdit::textEdited, this, &KateOllamaConfigPage::changed);
}

QString KateOllamaConfigPage::name() const { return i18n("Ollama"); }

QString KateOllamaConfigPage::fullName() const {
  return i18nc("Groupbox title", "Ollama Settings");
}

QIcon KateOllamaConfigPage::icon() const {
  return QIcon::fromTheme(QLatin1String("project-open"),
                          QIcon::fromTheme(QLatin1String("view-list-tree")));
}

void KateOllamaConfigPage::apply() {
  // Save settings to disk
  KConfigGroup group(KSharedConfig::openConfig(), "KateOllama");
  group.writeEntry("Model", modelsComboBox_->currentText());
  group.writeEntry("URL", ollamaURLText_->text());
  group.writeEntry("SystemPrompt", systemPromptEdit_->toPlainText());
  group.sync();
// SYNC config
}

void KateOllamaConfigPage::defaults() {
  ollamaURLText_->setText("http://localhost:11434");
  systemPromptEdit_->setPlainText(
      "You are a smart coder assistant, code comments are in the prompt language. You don't "
      "explain, you add only code comments.");
}

void KateOllamaConfigPage::reset() {
  modelsComboBox_->setCurrentIndex(-1);
  defaults();
}

