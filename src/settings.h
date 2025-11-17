/*
 S PDX-FileCopyrightText*: 2025 Daniele Mte90 Scasciafratte <mte90net@gmail.com>

 SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KATEOLLAMACONFIGPAGE_H
#define KATEOLLAMACONFIGPAGE_H

#include "src/ollama/ollamamodelsettings.h"

#include <KTextEditor/ConfigPage>

class KateOllamaPlugin;

class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QTabWidget;
class QTextEdit;
class QWidget;

class KateOllamaConfigPage : public KTextEditor::ConfigPage {

  Q_OBJECT

  struct ModelTab {
    QTextEdit* systemPrompt;
    QLineEdit* maxTokens;
    QDoubleSpinBox* temperature;
  };

 public:
  explicit KateOllamaConfigPage(QWidget* parent = nullptr, KateOllamaPlugin* plugin = nullptr);

 public:
  QString name() const override;
  QString fullName() const override;
  QIcon icon() const override;
  void apply() override;
  void defaults() override;
  void reset() override;

  QUrl url();
  QString model();
  OllamaModelSettings settingsFor(QString);

 private:
  KateOllamaPlugin* const plugin_;
  QComboBox* modelsComboBox_;
  QLineEdit* ollamaURLText_;
  QTabWidget* m_tabwidget;
  QMap<QString, ModelTab> m_tabs;

};

#endif // KATEOLLAMACONFIGPAGE_H
