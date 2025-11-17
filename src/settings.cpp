/*
 *  SPDX-FileCopyrightText: 2025 Daniele Mte90 Scasciafratte <mte90net@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QTabWidget>
#include <QTextEdit>
#include <QFormLayout>

#include "src/plugin.h"
#include "src/settings.h"
#include "src/ollama/ollamasystem.h"

static const char defaultPrompt[] =
    "You are a smart programming assistant. You prefer clean, modern code when writing c++."
    "You complete code based on the prompt and the surrounding code."
    "Take care to seamlessly integrate your response with the code preceding and following it."
    "Do not write comments. Do not write explanations. Do not write examples of any type."
    "Write only the code needed to fulfill requirements.";

KateOllamaConfigPage::KateOllamaConfigPage(QWidget* parent, KateOllamaPlugin* plugin)
  : KTextEditor::ConfigPage(parent), plugin_(plugin) {
  QFormLayout* layout = new QFormLayout(this);

  ollamaURLText_ = new QLineEdit(this);
  layout->addRow(i18n("Ollama URL"), ollamaURLText_);

  modelsComboBox_ = new QComboBox(this);
  modelsComboBox_->addItems(plugin_->models());
  layout->addRow(i18n("Default Model"), modelsComboBox_);

  KateOllamaConfigPage::reset();

  QObject::connect(modelsComboBox_, &QComboBox::currentIndexChanged, this, &KateOllamaConfigPage::changed);
  QObject::connect(ollamaURLText_, &QLineEdit::textEdited, this, &KateOllamaConfigPage::changed);

  m_tabwidget = new QTabWidget(this);
  for (const QString& model: plugin_->models()) {
    QWidget* tab = new QWidget(m_tabwidget);
    QFormLayout* tablayout = new QFormLayout(m_tabwidget);
    ModelTab tabwidgets;
    tabwidgets.systemPrompt = new QTextEdit(tab);
    tablayout->addRow(i18n("System Prompt"), tabwidgets.systemPrompt);
    QObject::connect(tabwidgets.systemPrompt, &QTextEdit::textChanged, this, &KateOllamaConfigPage::changed);
    tabwidgets.maxTokens = new QLineEdit(tab);
    tabwidgets.maxTokens->setValidator(new QIntValidator(0, 2048, tabwidgets.maxTokens));
    tablayout->addRow(i18n("Max Tokens"), tabwidgets.maxTokens);
    QObject::connect(tabwidgets.maxTokens, &QLineEdit::textEdited, this, &KateOllamaConfigPage::changed);
    tabwidgets.temperature = new QDoubleSpinBox(tab);
    tabwidgets.temperature->setDecimals(1);
    tabwidgets.temperature->setRange(0, 2);
    tablayout->addRow(i18n("Temperature"), tabwidgets.temperature);
    QObject::connect(tabwidgets.temperature, &QDoubleSpinBox::valueChanged, this, &KateOllamaConfigPage::changed);
    tab->setLayout(tablayout);
    m_tabwidget->addTab(tab, model);
    m_tabs[model] = tabwidgets;
  }
  layout->addWidget(m_tabwidget);

  setLayout(layout);
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
  group.writeEntry("DefaultModel", modelsComboBox_->currentText());
  group.writeEntry("URL", ollamaURLText_->text());
  KConfigGroup modelsettings(&group, "ModelSettings");
  for (const auto& [model, tab]: m_tabs.asKeyValueRange()) {
    KConfigGroup modeltab(&group, "Settings_" + model);
    group.writeEntry("Prompt", tab.systemPrompt->toPlainText());
    group.writeEntry("MaxTokens", tab.maxTokens->text());
    group.writeEntry("Temperature", tab.temperature->text());
  }
  group.sync();
  plugin_->readSettingsFromConfig();
}

void KateOllamaConfigPage::defaults() {
  ollamaURLText_->setText("http://localhost:11434");
  for (const auto& tab: m_tabs) {
    tab.systemPrompt->setPlainText(defaultPrompt);
    tab.maxTokens->setText("128");
    tab.temperature->setValue(0.5);
  }
}

void KateOllamaConfigPage::reset() {
  ollamaURLText_->setText(plugin_->currentUrl().toString());
  modelsComboBox_->setCurrentIndex(plugin_->models().indexOf(plugin_->currentModel()));
}

QUrl KateOllamaConfigPage::url() { return ollamaURLText_->text(); }

QString KateOllamaConfigPage::model() { return modelsComboBox_->currentText(); }

OllamaModelSettings KateOllamaConfigPage::settingsFor(QString model) {
  if (!m_tabs.contains("model")) {
    return OllamaModelSettings{model, QString(defaultPrompt), QString("128"), 0.5};
  }
  auto tab = m_tabs.value("model");
  return OllamaModelSettings{ model, tab.systemPrompt->toPlainText(),
                              tab.maxTokens->text(), tab.temperature->value() };
}
