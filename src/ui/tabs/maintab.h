/*
 *  SPDX-FileCopyrightText: 2025 tfks <development@worloflinux.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef MAINTAB_H
#define MAINTAB_H

// KF Headers
#include <KLocalizedString>
#include <KTextEditor/MainWindow>
#include <KTextEditor/Message>
#include <KXMLGUIClient>

#include <qevent.h>
#include <qlist.h>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpacerItem>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>

#include "src/ollama/ollamaresponse.h"
#include "src/ollama/ollamasystem.h"
#include "src/plugin.h"
#include "src/ui/controls//qollamaplaintextedit.h"
#include "src/ui/widgets/toolwidget.h"

class MainTab
  : public QWidget
  , public KXMLGUIClient {
  Q_OBJECT

 public:
  MainTab(KateOllamaPlugin* plugin, KTextEditor::MainWindow* mainWindow, OllamaSystem* ollamaSystem,
          OllamaToolWidget* parent = nullptr);
  ~MainTab();

 public slots:
  // void handle_signalModelsListLoaded(const QList<QJsonValue>& modelsList);
  // void handle_signalOnSinglePrompt();
  // void handle_signalOnFullPrompt();

 //  void handle_signalOllamaRequestMetaDataChanged(OllamaResponse ollamaResponse);
 //  void handle_signalOllamaRequestGotResponse(OllamaResponse ollamaResponse);
 //  void handle_signalOllamaRequestFinished(OllamaResponse ollamaResponse);

 //  void handle_signal_textAreaInputEnterKeyWasPressed(QKeyEvent* event);
 //  void handle_signalOutputInEditorClicked();

 private:
 //  void loadModels();
 //  QString getPrompt();
 //  void ollamaRequest(QString prompt);

  bool outputInEditor_;

  KTextEditor::MainWindow* mainWindow_ = nullptr;

  QVBoxLayout* mainLayout_;

  QWidget* leftWidget_;
  QVBoxLayout* leftLayout_;

  QList<QPushButton>*
      sessionButtons_; // These buttons need to be added based on the saved sessions.
  // Need a custom control with a label and a button for deletion.

  QWidget* rightWidget_;
  QVBoxLayout* rightLayout_; // this houses the main interface. We need to add the top, middle,
                             // bottom widget to it.

  QSplitter* leftRightSplitter_; // the left and right widget need to be added here.

  QWidget* topWidget_;
  QHBoxLayout* topLayout_;
  QComboBox* modelsComboBox_;
  QPushButton* newTabBtn_;

  QWidget* middleWidget_;
  QHBoxLayout* middleLayout_;
  QSplitter* splitter_;
  QOllamaPlainTextEdit* textAreaInput_;
  QOllamaPlainTextEdit* textAreaOutput_;

  QWidget* bottomWidget_;
  QHBoxLayout* bottomLayout_;
  QLabel* label_override_ollama_endpoint_;
  QLineEdit* line_edit_override_ollama_endpoint_;
  QPushButton* outputInEditorPushButton_;

  KateOllamaPlugin* plugin_;
  OllamaSystem* ollamaSystem_;
};
#endif // MAINTAB_H
