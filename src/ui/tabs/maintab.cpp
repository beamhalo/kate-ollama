/*
 *  SPDX-FileCopyrightText: 2025 tfks <development@worloflinux.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
// KF Headers
#include <KActionCollection>
#include <KLocalizedString>
#include <KXMLGUIClient>

#include <qnamespace.h>
#include <qrawfont.h>
#include <qwidget.h>
#include <QComboBox>
#include <QHBoxLayout>
#include <QJsonObject>
#include <QJsonValue>
#include <QKeyEvent>
#include <QKeySequence>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QLocale>
#include <QObject>
#include <QPlainTextEdit>
#include <QSizePolicy>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>

#include "src/ollama/ollamaglobals.h"
#include "src/ollama/ollamaresponse.h"
#include "src/ollama/ollamasystem.h"
#include "src/ui/controls/qollamaplaintextedit.h"
#include "src/ui/tabs/maintab.h"
#include "src/ui/utilities/messages.h"
#include "src/ui/widgets/toolwidget.h"

MainTab::MainTab(KateOllamaPlugin* plugin, KTextEditor::MainWindow* mainWindow,
                 OllamaSystem* ollamaSystem, OllamaToolWidget* parent)
  : QWidget(parent), mainWindow_(mainWindow), plugin_(plugin), ollamaSystem_(ollamaSystem) {
  leftWidget_ = new QWidget(this);

  rightWidget_ = new QWidget(this);

  topWidget_ = new QWidget(this);
  topWidget_->setFixedHeight(35);
  topWidget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  topLayout_      = new QHBoxLayout(topWidget_);
  modelsComboBox_ = new QComboBox(topWidget_);
  modelsComboBox_->setFixedHeight(30);
  newTabBtn_ = new QPushButton(QIcon::fromTheme(QStringLiteral("tab-new")), QString(), topWidget_);
  newTabBtn_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  newTabBtn_->setFixedHeight(30);
  newTabBtn_->setToolTip(i18n("Add new tab"));
  topLayout_->addWidget(modelsComboBox_);
  topLayout_->addWidget(newTabBtn_);
  topWidget_->setLayout(topLayout_);

  middleWidget_  = new QWidget(this);
  middleLayout_  = new QHBoxLayout(middleWidget_);
  splitter_      = new QSplitter(Qt::Vertical, middleWidget_);
  textAreaInput_ = new QOllamaPlainTextEdit(middleWidget_);
  textAreaInput_->setPlaceholderText(ki18n(OllamaGlobals::HelpText.toUtf8().data()).toString());
  textAreaOutput_ = new QOllamaPlainTextEdit(middleWidget_);
  splitter_->addWidget(textAreaOutput_);
  splitter_->addWidget(textAreaInput_);
  middleLayout_->addWidget(splitter_);
  middleWidget_->setLayout(middleLayout_);

  bottomWidget_ = new QWidget(this);
  bottomWidget_->setFixedHeight(35);
  bottomWidget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  bottomWidget_->setContentsMargins(0, 0, 0, 10);
  bottomLayout_                   = new QHBoxLayout(bottomWidget_);
  label_override_ollama_endpoint_ = new QLabel(
      ki18n(OllamaGlobals::LabelOllamaEndpointOverride.toUtf8().data()).toString(), bottomWidget_);
  label_override_ollama_endpoint_->setFixedHeight(30);
  line_edit_override_ollama_endpoint_ = new QLineEdit(plugin_->currentUrl().toString(), bottomWidget_);
  line_edit_override_ollama_endpoint_->setFixedHeight(30);
  outputInEditorPushButton_ =
      new QPushButton(QIcon::fromTheme(QStringLiteral("text-x-generic")),
                      QString(i18n("Output in editor (Off)")), bottomWidget_);
  outputInEditorPushButton_->setFixedHeight(30);
  outputInEditorPushButton_->setToolTip(i18n("Output in editor"));
  bottomLayout_->addWidget(label_override_ollama_endpoint_);
  bottomLayout_->addWidget(line_edit_override_ollama_endpoint_);
  bottomLayout_->addWidget(outputInEditorPushButton_);
  bottomWidget_->setLayout(bottomLayout_);

  mainLayout_ = new QVBoxLayout(this);
  mainLayout_->addWidget(topWidget_);
  mainLayout_->addWidget(middleWidget_);
  mainLayout_->addWidget(bottomWidget_);

  setLayout(mainLayout_);
/*
  connect(newTabBtn_, &QAbstractButton::clicked, parent, &OllamaToolWidget::newTab);
  connect(ollamaSystem_, &OllamaSystem::signal_modelsListLoaded, this,
          &MainTab::handle_signalModelsListLoaded);
  connect(ollamaSystem_, &OllamaSystem::signal_ollamaRequestMetaDataChanged, this,
          &MainTab::handle_signalOllamaRequestMetaDataChanged);
  connect(ollamaSystem_, &OllamaSystem::signal_ollamaRequestGotResponse, this,
          &MainTab::handle_signalOllamaRequestGotResponse);
  connect(ollamaSystem_, &OllamaSystem::signal_ollamaRequestFinished, this,
          &MainTab::handle_signalOllamaRequestFinished);
  connect(textAreaInput_, &QOllamaPlainTextEdit::signal_enterKeyWasPressed, this,
          &MainTab::handle_signal_textAreaInputEnterKeyWasPressed);
  connect(textAreaOutput_, &QPlainTextEdit::textChanged, textAreaOutput_,
          &QOllamaPlainTextEdit::onTextChanged);
  connect(outputInEditorPushButton_, &QPushButton::clicked, this,
          &MainTab::handle_signalOutputInEditorClicked);

  loadModels();*/
}

MainTab::~MainTab() {}
/*
void MainTab::handle_signalModelsListLoaded(const QList<QJsonValue>& modelsList) {
  int modelSelected = -1;
  modelsComboBox_->clear();
  for (const QJsonValue& modelValue : modelsList) {
    QJsonObject modelObj = modelValue.toObject();
    if (modelObj.contains("name")) {
      modelsComboBox_->addItem(modelObj["name"].toString());
    }

    if (modelObj["name"].toString() == plugin_->getModel()) {
      modelSelected = modelsComboBox_->count();
    }
  }

  if (modelSelected != -1) {
    modelsComboBox_->setCurrentIndex(modelSelected - 1);
  }
}

void MainTab::handle_signalOllamaRequestMetaDataChanged(OllamaResponse ollamaResponse) {
  if (ollamaResponse.getReceiver() == "widget" || ollamaResponse.getReceiver() == ""
      || outputInEditor_) {
    QTextCursor cursor = textAreaInput_->textCursor();
    cursor.insertText("\n");

    Messages::showStatusMessage(QStringLiteral("Info: Request started..."),
                                KTextEditor::Message::Information, mainWindow_);
  }
}

void MainTab::handle_signalOllamaRequestGotResponse(OllamaResponse ollamaResponse) {
  if (ollamaResponse.getReceiver() != "widget" && ollamaResponse.getReceiver() != "")
    return;

  QTextCursor cursor = textAreaOutput_->textCursor();
  cursor.insertText(ollamaResponse.getResponseText());

  Messages::showStatusMessage(QStringLiteral("Info: Reply received..."),
                              KTextEditor::Message::Information, mainWindow_);
}

void MainTab::handle_signalOllamaRequestFinished(OllamaResponse ollamaResponse) {
  if (ollamaResponse.getErrorMessage() != QString("")) {
    Messages::showStatusMessage(
        QStringLiteral("Error encountered: %0").arg(ollamaResponse.getErrorMessage()),
        KTextEditor::Message::Information, mainWindow_);
    qDebug() << "Error:" << ollamaResponse.getErrorMessage();
    qDebug() << "Model:" << plugin_->getModel();
    qDebug() << "System prompt:" << plugin_->getSystemPrompt();
  }

  if (ollamaResponse.getReceiver() == "widget" || ollamaResponse.getReceiver() == "") {
    QTextCursor cursor = textAreaOutput_->textCursor();
    cursor.insertText("\n\n");
  }
}

void MainTab::handle_signal_textAreaInputEnterKeyWasPressed(QKeyEvent* event) {
  if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
    qDebug() << "Enter/Return key pressed in CustomWidget";

    if (!event->modifiers()) {
      QString prompt = textAreaInput_->toPlainText();
      // I need the output text area to always have output.
      // reason is that I can always access it.
      // It is difficult to know what editor the ouput was written in.
      // I could also keep history in a session.
      // Yes, that's the way to go.
      // QString history = QString("");

      ollamaRequest(prompt);
    } else {
      QTextCursor cursor = textAreaInput_->textCursor();
      cursor.insertText("\n");

      textAreaInput_->verticalScrollBar()->maximum();
    }
  }
}

void MainTab::handle_signalOutputInEditorClicked() {
  if (outputInEditor_ == false) {
    outputInEditor_ = true;
    outputInEditorPushButton_->setText(i18n("Output in editor (On)"));
    textAreaInput_->setFocus();
  } else {
    outputInEditor_ = false;
    outputInEditorPushButton_->setText(i18n("Output in editor (Off)"));
  }

  QTextCursor cursor = textAreaInput_->textCursor();
  cursor.movePosition(QTextCursor::End);
  textAreaInput_->setTextCursor(cursor);
}

void MainTab::loadModels() {
  OllamaData ollamaData;

  ollamaData.setOllamaUrl(plugin_->getOllamaUrl());

  ollamaSystem_->fetchModels(ollamaData);
}

QString MainTab::getPrompt() {
  Messages::showStatusMessage(QStringLiteral("Info: Getting prompt..."),
                              KTextEditor::Message::Information, mainWindow_);

  QString text = textAreaInput_->toPlainText();

  return ollamaSystem_->getPromptFromText(text);
}

void MainTab::ollamaRequest(QString prompt) {
  OllamaData data;

  QVector<QString> images;

  if (outputInEditor_) {
    data.setSender("editor");
  } else {
    data.setSender("widget");
  }

  QString ollamaUrl = line_edit_override_ollama_endpoint_->displayText();

  if (ollamaUrl != nullptr && ollamaUrl != "" && data.isOllamaUrlValid()) {
    data.setOllamaUrl(ollamaUrl);
  } else {
    data.setOllamaUrl(plugin_->getOllamaUrl());
  }

  QString model = modelsComboBox_->currentText();

  if (model != nullptr && model != "") {
    data.setModel(model);
  } else {
    data.setModel(plugin_->getModel());
  }

  data.setPrompt(prompt);
  data.setSuffix("");

  for (int i = 0; i < images.size(); ++i) {
    data.addImage(images[i]);
  }

  // data.setFormat("");
  // data.setOptions("");
  data.setSystemPrompt(plugin_->getSystemPrompt());
  // data.setContext("");
  // data.setStream("");

  // we need to connect to the response as that is asynchronous.
  ollamaSystem_->ollamaRequest(data);
}
*/
