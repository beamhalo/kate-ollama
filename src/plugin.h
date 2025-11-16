/*
    SPDX-FileCopyrightText: 2025 Daniele Mte90 Scasciafratte <mte90net@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KATEOLLAMAPLUGIN_H
#define KATEOLLAMAPLUGIN_H

// KF headers
#include "ollama/ollamadata.h"
#include "ollama/ollamasystem.h"
#include <KTextEditor/Document>
#include <KTextEditor/MainWindow>
#include <KTextEditor/Plugin>
#include <KTextEditor/SessionConfigInterface>
#include <KTextEditor/View>
#include <KXMLGUIClient>
#include <QString>

class KateOllamaPlugin : public KTextEditor::Plugin {
  Q_OBJECT

 public:
  explicit KateOllamaPlugin(QObject* parent, const QVariantList& = QVariantList());

  QObject* createToolWindow(KTextEditor::MainWindow* mainWindow);
  QObject* createView(KTextEditor::MainWindow* mainWindow) override;

  void readSettings();

  int configPages() const override { return 1; }

  KTextEditor::ConfigPage* configPage(int number = 0, QWidget* parent = nullptr) override;

  void setModel(QString model);
  QString getModel();

  void setSystemPrompt(QString systemPrompt);
  QString getSystemPrompt();

  void setOllamaUrl(QString ollamaUrl);
  QString getOllamaUrl();

  void setOllamaData(OllamaData ollamaData);
  OllamaData getOllamaData();

 private:
  QString model_;
  QString systemPrompt_;
  QString ollamaUrl_;

  OllamaData ollamaData_;
  OllamaSystem* ollamaSystem_;
};

#endif // KATEOLLAMAPLUGIN_H
