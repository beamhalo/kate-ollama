/*
 S PDX-FileCopyrightText*: 2025 Daniele Mte90 Scasciafratte <mte90net@gmail.com>

 SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KATEOLLAMACONFIGPAGE_H
#define KATEOLLAMACONFIGPAGE_H

#include <KTextEditor/ConfigPage>

class KateOllamaPlugin;
class QLabel;
class QComboBox;
class QLineEdit;
class QTextEdit;
class QWidget;

class KateOllamaConfigPage : public KTextEditor::ConfigPage {
  Q_OBJECT

 public:
  explicit KateOllamaConfigPage(QWidget* parent = nullptr, KateOllamaPlugin* plugin = nullptr);

  void loadSettings();
  void fetchModelList();

 public:
  QString name() const override;
  QString fullName() const override;
  QIcon icon() const override;

 public:
  void apply() override;
  void defaults() override;
  void reset() override;

 private:
  KateOllamaPlugin* const plugin_;
  QComboBox* modelsComboBox_;
  QTextEdit* systemPromptEdit_;
  QLineEdit* ollamaURLText_;
  QLabel* infoLabel_;
};

#endif // KATEOLLAMACONFIGPAGE_H
