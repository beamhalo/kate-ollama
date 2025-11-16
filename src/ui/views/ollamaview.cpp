/*
 *  SPDX-FileCopyrightText: 2025 tfks <development@worloflinux.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

// KF headers
#include <KActionCollection>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KSharedConfig>
#include <KTextEditor/Application>
#include <KTextEditor/Document>
#include <KTextEditor/Editor>
#include <KTextEditor/MainWindow>
#include <KTextEditor/Message>
#include <KTextEditor/Plugin>
#include <KTextEditor/View>
#include <KXMLGUIClient>
#include <KXMLGUIFactory>

#include <ktexteditor/message.h>
#include <qcontainerfwd.h>
#include <QAction>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QString>

#include <QLabel>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

#include "src/ollama//ollamasystem.h"
#include "src/ollama/ollamadata.h"
#include "src/ollama/ollamaglobals.h"
#include "src/ollama/ollamaresponse.h"
#include "src/plugin.h"
#include "src/ui/utilities/messages.h"
#include "src/ui/views/ollamaview.h"
#include "src/ui/widgets/toolwidget.h"

using namespace Qt::Literals::StringLiterals;

KateOllamaView::KateOllamaView(KateOllamaPlugin* plugin, KTextEditor::MainWindow* mainwindow,
                               OllamaSystem* ollamaSystem)
  : KXMLGUIClient(), plugin_(plugin), mainWindow_(mainwindow), ollamaSystem_(ollamaSystem) {
  KXMLGUIClient::setComponentName(u"kateollama"_s, i18n("Kate-Ollama"));
  KConfigGroup group(KSharedConfig::openConfig(), "KateOllama");

  plugin_->setModel(group.readEntry("Model"));
  plugin_->setSystemPrompt(group.readEntry("SystemPrompt"));
  plugin_->setOllamaUrl(group.readEntry("URL"));

  auto ac    = actionCollection();
  QAction* a = ac->addAction(QStringLiteral("kateollama"));
  a->setText(i18n("Run Ollama"));
  a->setIcon(QIcon::fromTheme(QStringLiteral("debug-run")));
  KActionCollection::setDefaultShortcut(a, QKeySequence((Qt::CTRL | Qt::Key_Semicolon)));
  connect(a, &QAction::triggered, this, &KateOllamaView::handle_onSinglePrompt);

  QAction* a2 = ac->addAction(QStringLiteral("kateollama-full-prompt"));
  a2->setText(i18n("Run Ollama Full Text"));
  a2->setIcon(QIcon::fromTheme(QStringLiteral("debug-run")));
  KActionCollection::setDefaultShortcut(a2,
                                        QKeySequence((Qt::CTRL | Qt::SHIFT | Qt::Key_Semicolon)));
  connect(a2, &QAction::triggered, this, &KateOllamaView::handle_onFullPrompt);

  QAction* a3 = ac->addAction(QStringLiteral("kateollama-command"));
  a3->setText(i18n("Add Ollama Command"));
  KActionCollection::setDefaultShortcut(a3, QKeySequence((Qt::CTRL | Qt::Key_Slash)));
  connect(a3, &QAction::triggered, this, &KateOllamaView::handle_onPrintCommand);

  mainWindow_->guiFactory()->addClient(this);

  auto toolview = mainWindow_->createToolView(
      plugin, "ollamatoolwidget", KTextEditor::MainWindow::Bottom,
      QIcon::fromTheme(OllamaGlobals::IconName), OllamaGlobals::PluginName);

  toolWidget_ = new OllamaToolWidget(plugin_, mainWindow_, ollamaSystem_, toolview);

  toolview_.reset(toolview);

  connect(ollamaSystem_, &OllamaSystem::signal_ollamaRequestMetaDataChanged, this,
          &KateOllamaView::handle_ollamaRequestMetaDataChanged);

  connect(ollamaSystem_, &OllamaSystem::signal_ollamaRequestGotResponse, this,
          &KateOllamaView::handle_ollamaRequestGotResponse);

  connect(ollamaSystem_, &OllamaSystem::signal_ollamaRequestFinished, this,
          &KateOllamaView::handle_ollamaRequestFinished);
}

KateOllamaView::~KateOllamaView() { mainWindow_->guiFactory()->removeClient(this); }

void KateOllamaView::handle_onSinglePrompt() {
  KTextEditor::View* view = mainWindow_->activeView();
  if (view) {
    QString prompt = KateOllamaView::getPrompt();
    if (!prompt.isEmpty()) {
      KateOllamaView::ollamaRequest(prompt);
    }
  } else {
    Messages::showStatusMessage(QStringLiteral("No prompt source..."),
                                KTextEditor::Message::Warning, mainWindow_);
  }
}

void KateOllamaView::handle_onFullPrompt() {
  KTextEditor::View* view = mainWindow_->activeView();
  if (view) {
    KateOllamaView::autoFillRequest("");
  } else {
    Messages::showStatusMessage(QStringLiteral("No prompt source..."),
                                KTextEditor::Message::Warning, mainWindow_);
  }
}

void KateOllamaView::handle_onPrintCommand() {
  KTextEditor::View* view = mainWindow_->activeView();
  ;
  if (view) {
    KTextEditor::Document* document = view->document();
    QString text                    = document->text();
    KTextEditor::Cursor cursor      = view->cursorPosition();
    document->insertText(cursor, "// AI: ");
  }
}

void KateOllamaView::handle_ollamaRequestMetaDataChanged(OllamaResponse ollamaResponse) {
  if (ollamaResponse.getReceiver() == "editor" || ollamaResponse.getReceiver() == "") {
    KTextEditor::View* view         = mainWindow_->activeView();
    KTextEditor::Document* document = view->document();
    KTextEditor::Cursor cursor      = view->cursorPosition();
    //document->insertText(cursor, "\n");
  }
}

void KateOllamaView::handle_ollamaRequestGotResponse(OllamaResponse ollamaResponse) {
  if (ollamaResponse.getReceiver() != "editor" && ollamaResponse.getReceiver() != "")
    return;

  KTextEditor::View* view         = mainWindow_->activeView();
  KTextEditor::Document* document = view->document();
  KTextEditor::Cursor cursor      = view->cursorPosition();
  document->insertText(cursor, ollamaResponse.getResponseText());
}

void KateOllamaView::handle_ollamaRequestFinished(OllamaResponse ollamaResponse) {
  if (ollamaResponse.getErrorMessage() != QString("")) {
    Messages::showStatusMessage(ollamaResponse.getErrorMessage(),
                                KTextEditor::Message::Error, mainWindow_);
  }
  if (ollamaResponse.getReceiver() == "editor" || ollamaResponse.getReceiver() == "") {
    KTextEditor::View* view         = mainWindow_->activeView();
    KTextEditor::Document* document = view->document();
    KTextEditor::Cursor cursor      = view->cursorPosition();
    //document->insertText(cursor, "\n");
  }
}

QString KateOllamaView::getPrompt() {
  KTextEditor::View* view         = mainWindow_->activeView();
  KTextEditor::Document* document = view->document();
  QString text                    = document->text();

  QRegularExpression re("// AI:(.*)");
  QRegularExpressionMatchIterator matchIterator = re.globalMatch(text);

  QString lastMatch;

  while (matchIterator.hasNext()) {
    QRegularExpressionMatch match = matchIterator.next();
    lastMatch                     = match.captured(1).trimmed();
  }
  return lastMatch;
}

void KateOllamaView::autoFillRequest(QString) {
  Messages::showStatusMessage(QStringLiteral("Wait for model insertion..."),
                              KTextEditor::Message::Information, mainWindow_);
  OllamaData data;
  KTextEditor::View* view = mainWindow_->activeView();
  if (!view) {
    return;
  }
  KTextEditor::Document* document = view->document();
  KTextEditor::Cursor cursor = view->cursorPosition();
  KTextEditor::Range range = document->documentRange();
  KTextEditor::Range before(range.start(), cursor);
  KTextEditor::Range after(cursor, range.end());
  Q_ASSERT(document);
  QString prefix = document->text(before);
  QString suffix = document->text(after);
  data.setSender("editor");
  data.setOllamaUrl(plugin_->getOllamaUrl());
  data.setModel(plugin_->getModel());
  data.setPrompt(prefix);
  data.setSuffix(suffix);


  // data.setFormat("");
  // data.setOptions("");
  data.setSystemPrompt(plugin_->getSystemPrompt() + " Your task is completing the missing code. Only write the minimum required to do the job. Do not include examples or explanations.");
  // data.setContext("");
  // data.setStream("");

  ollamaSystem_->ollamaRequest(data);
}

void KateOllamaView::ollamaRequest(QString prompt) {
  Messages::showStatusMessage(QStringLiteral("Wait for model response..."),
                              KTextEditor::Message::Information, mainWindow_);

  QJsonObject json_data;
  OllamaData data;
  QVector<QString> images;

  data.setSender("editor");
  data.setOllamaUrl(plugin_->getOllamaUrl());
  data.setModel(plugin_->getModel());
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

  json_data = data.toJson();

  QJsonDocument doc(json_data);

  ollamaSystem_->ollamaRequest(data);
}
