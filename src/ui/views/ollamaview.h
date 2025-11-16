/*
 *  SPDX-FileCopyrightText: 2025 tfks <development@worloflinux.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KATEOLLAMAVIEW_H
#define KATEOLLAMAVIEW_H

#include <KTextEditor/Plugin>

#include <KXMLGUIClient>
#include <QObject>

#include "src/ollama/ollamaresponse.h"
#include "src/ollama/ollamasystem.h"
#include "src/ui/widgets/toolwidget.h"

class KateOllamaPlugin;

class KateOllamaView : public QObject, public KXMLGUIClient {

  Q_OBJECT

 public:
  explicit KateOllamaView(KateOllamaPlugin* plugin, KTextEditor::MainWindow* mainwindow);
  ~KateOllamaView();

  QObject* createToolWindow(KTextEditor::MainWindow* mainWindow);

 private slots:
  void handle_onSinglePrompt();
  void handle_onFullPrompt();
  void handle_onPrintCommand();
  void handle_ollamaRequestGotResponse(OllamaResponse ollamaResponse);
  void handle_ollamaRequestFinished(OllamaResponse ollamaResponse);

 private:
  QString getPrompt();
  void autoFillRequest(QString prompt);
  void ollamaRequest(QString prompt);

 private:
  KateOllamaPlugin* plugin_            = nullptr;
  KTextEditor::MainWindow* mainWindow_ = nullptr;
  OllamaToolWidget* toolWidget_        = nullptr;
  std::unique_ptr<QWidget> toolview_;
  OllamaSystem* ollamaSystem_;
};

#endif // KATEOLLAMAVIEW_H
