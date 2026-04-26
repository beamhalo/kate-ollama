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
#include "src/ollama/ollamaglobals.h"
#include "src/ollama/ollamaresponse.h"
#include "src/plugin.h"
#include "src/ui/utilities/messages.h"
#include "src/ui/views/ollamaview.h"
#include "src/ui/widgets/toolwidget.h"

using namespace Qt::Literals::StringLiterals;

KateOllamaView::KateOllamaView(KateOllamaPlugin* plugin, KTextEditor::MainWindow* mainwindow) :
 KXMLGUIClient(), plugin_(plugin), mainWindow_(mainwindow), ollamaSystem_(plugin->getOllama()) {

  KXMLGUIClient::setComponentName(u"kateollama"_s, i18n("Kate-Ollama"));

  auto ac    = actionCollection();
  QAction* act_fill = ac->addAction(QStringLiteral("kateollama-autofill-code"));
  act_fill->setText(i18n("Run Ollama autofill"));
  act_fill->setIcon(QIcon::fromTheme(QStringLiteral("debug-run")));
  KActionCollection::setDefaultShortcut(act_fill, QKeySequence((Qt::CTRL | Qt::Key_Semicolon)));
  connect(act_fill, &QAction::triggered, this, &KateOllamaView::handle_onAutoFillPrompt);

  QAction* act_force_stop = ac->addAction(QStringLiteral("kateollama-stop-prompt"));
  act_force_stop->setText(i18n("Stop a runaway model"));
  act_force_stop->setIcon(QIcon::fromTheme(QStringLiteral("debug-stop")));
  KActionCollection::setDefaultShortcut(act_force_stop,
                                        QKeySequence((Qt::CTRL | Qt::SHIFT | Qt::Key_Semicolon)));
  connect(act_force_stop, &QAction::triggered, this, &KateOllamaView::handle_onEmergencyStop);

  mainWindow_->guiFactory()->addClient(this);

  auto toolview = mainWindow_->createToolView(
      plugin, "ollamatoolwidget", KTextEditor::MainWindow::Bottom,
      QIcon::fromTheme(OllamaGlobals::IconName), OllamaGlobals::PluginName);

  toolWidget_ = new OllamaToolWidget(plugin_, mainWindow_, toolview);

  toolview_.reset(toolview);

  connect(ollamaSystem_, &OllamaSystem::responsePart, this, &KateOllamaView::handle_ollamaRequestGotResponse);

  connect(ollamaSystem_, &OllamaSystem::responseFinished, this, &KateOllamaView::handle_ollamaRequestFinished);

  connect(ollamaSystem_, &OllamaSystem::errorReceived, this, [this](QString error){
    Messages::showStatusMessage(error, KTextEditor::Message::Error, mainWindow_);
  });
}

KateOllamaView::~KateOllamaView() { mainWindow_->guiFactory()->removeClient(this); }


void KateOllamaView::handle_onAutoFillPrompt() {
  Messages::showStatusMessage(QStringLiteral("Wait for model autofill..."),
                              KTextEditor::Message::Information, mainWindow_);
  KTextEditor::View* view = mainWindow_->activeView();
  if (view) {
    KateOllamaView::autoFillRequest();
  } else {
    Messages::showStatusMessage(QStringLiteral("No prompt source..."),
                                KTextEditor::Message::Warning, mainWindow_);
  }
}

void KateOllamaView::handle_onEmergencyStop() {
  Messages::showStatusMessage(QStringLiteral("Force stopping model ..."),
                              KTextEditor::Message::Warning, mainWindow_);
  ollamaSystem_->killModel();
}

void KateOllamaView::handle_ollamaRequestGotResponse(OllamaResponse resp) {
  KTextEditor::View* view = mainWindow_->activeView();
  if (!view) {
    return;
  }
  if (resp.context_id != request_id_) {
    return;
  }
  KTextEditor::Document* document = view->document();
  KTextEditor::Cursor cursor = view->cursorPosition();
  document->insertText(cursor, resp.responseText);
}

void KateOllamaView::handle_ollamaRequestFinished(OllamaResponse resp) {
  if (resp.context_id != request_id_) {
    return;
  }
  Messages::showStatusMessage(QStringLiteral("Model response complete."),
                              KTextEditor::Message::Information, mainWindow_);
}

void KateOllamaView::autoFillRequest() {
  KTextEditor::View* view = mainWindow_->activeView();
  if (!view) {
    return;
  }
  KTextEditor::Document* document = view->document();
  KTextEditor::Cursor cursor = view->cursorPosition();
  KTextEditor::Range range = document->documentRange();
  KTextEditor::Range before(range.start(), cursor);
  KTextEditor::Range after(cursor, range.end());
  OllamaRequest request;
  request.prompt = document->text(before);
  request.suffix = document->text(after);
  request.context_id = ++request_id_;
  ollamaSystem_->autofill(request);
}

