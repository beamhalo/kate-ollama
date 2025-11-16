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
#include <ktexteditor/message.h>
#include <qcontainerfwd.h>

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

KateOllamaView::KateOllamaView(KateOllamaPlugin *plugin, KTextEditor::MainWindow *mainwindow, OllamaSystem *ollamaSystem)
    : KXMLGUIClient()
    , plugin_(plugin)
    , mainWindow_(mainwindow)
    , ollamaSystem_(ollamaSystem)
{
    KXMLGUIClient::setComponentName(u"kateollama"_s, i18n("Kate-Ollama"));
    KConfigGroup group(KSharedConfig::openConfig(), "KateOllama");

    plugin_->setModel(group.readEntry("Model"));
    plugin_->setSystemPrompt(group.readEntry("SystemPrompt"));
    plugin_->setOllamaUrl(group.readEntry("URL"));

    auto ac = actionCollection();
    QAction *a = ac->addAction(QStringLiteral("kateollama"));
    a->setText(i18n("Run Ollama"));
    a->setIcon(QIcon::fromTheme(QStringLiteral("debug-run")));
    KActionCollection::setDefaultShortcut(a, QKeySequence((Qt::CTRL | Qt::Key_Semicolon)));
    connect(a, &QAction::triggered, this, &KateOllamaView::handle_onSinglePrompt);

    QAction *a2 = ac->addAction(QStringLiteral("kateollama-full-prompt"));
    a2->setText(i18n("Run Ollama Full Text"));
    a2->setIcon(QIcon::fromTheme(QStringLiteral("debug-run")));
    KActionCollection::setDefaultShortcut(a2, QKeySequence((Qt::CTRL | Qt::SHIFT | Qt::Key_Semicolon)));
    connect(a2, &QAction::triggered, this, &KateOllamaView::handle_onFullPrompt);

    QAction *a3 = ac->addAction(QStringLiteral("kateollama-command"));
    a3->setText(i18n("Add Ollama Command"));
    KActionCollection::setDefaultShortcut(a3, QKeySequence((Qt::CTRL | Qt::Key_Slash)));
    connect(a3, &QAction::triggered, this, &KateOllamaView::handle_onPrintCommand);

    mainWindow_->guiFactory()->addClient(this);

    auto toolview = mainWindow_->createToolView(plugin,
                                                "ollamatoolwidget",
                                                KTextEditor::MainWindow::Bottom,
                                                QIcon::fromTheme(OllamaGlobals::IconName),
                                                OllamaGlobals::PluginName);

    toolWidget_ = new OllamaToolWidget(plugin_, mainWindow_, ollamaSystem_, toolview);

    toolview_.reset(toolview);

    connect(ollamaSystem_, &OllamaSystem::signal_ollamaRequestMetaDataChanged, this, &KateOllamaView::handle_ollamaRequestMetaDataChanged);

    connect(ollamaSystem_, &OllamaSystem::signal_ollamaRequestGotResponse, this, &KateOllamaView::handle_ollamaRequestGotResponse);

    connect(ollamaSystem_, &OllamaSystem::signal_ollamaRequestFinished, this, &KateOllamaView::handle_ollamaRequestFinished);
}

KateOllamaView::~KateOllamaView()
{
    mainWindow_->guiFactory()->removeClient(this);
}

void KateOllamaView::handle_onSinglePrompt()
{
    KTextEditor::View *view = mainWindow_->activeView();
    if (view) {
        QString prompt = KateOllamaView::getPrompt();
        if (!prompt.isEmpty()) {
            Messages::showStatusMessage(QStringLiteral("Info: Single prompt.."), KTextEditor::Message::Information, mainWindow_);
            KateOllamaView::ollamaRequest(prompt);
        } else {
            Messages::showStatusMessage(QStringLiteral("Info: No single prompt..."), KTextEditor::Message::Information, mainWindow_);
        }
    } else {
        Messages::showStatusMessage(QStringLiteral("Info: Single prompt, no view..."), KTextEditor::Message::Information, mainWindow_);
    }
}

void KateOllamaView::handle_onFullPrompt()
{
    KTextEditor::View *view = mainWindow_->activeView();
    KTextEditor::Document *document = view->document();
    QString text = document->text();
    if (view) {
        QString prompt = KateOllamaView::getPrompt();
        if (!prompt.isEmpty()) {
            Messages::showStatusMessage(QStringLiteral("Info: Full prompt..."), KTextEditor::Message::Information, mainWindow_);
            KateOllamaView::ollamaRequest(text + "\n" + prompt);
        } else {
            Messages::showStatusMessage(QStringLiteral("Info: No full prompt..."), KTextEditor::Message::Information, mainWindow_);
        }
    } else {
        Messages::showStatusMessage(QStringLiteral("Info: Full prompt, no view..."), KTextEditor::Message::Information, mainWindow_);
    }
}

void KateOllamaView::handle_onPrintCommand()
{
    Messages::showStatusMessage(QStringLiteral("Info: Printing command..."), KTextEditor::Message::Information, mainWindow_);
    KTextEditor::View *view = mainWindow_->activeView();
    ;
    if (view) {
        KTextEditor::Document *document = view->document();
        QString text = document->text();
        KTextEditor::Cursor cursor = view->cursorPosition();
        document->insertText(cursor, "// AI: ");
    }
}

void KateOllamaView::handle_ollamaRequestMetaDataChanged(OllamaResponse ollamaResponse)
{
    if (ollamaResponse.getReceiver() == "editor" || ollamaResponse.getReceiver() == "") {
        KTextEditor::View *view = mainWindow_->activeView();
        KTextEditor::Document *document = view->document();
        KTextEditor::Cursor cursor = view->cursorPosition();
        document->insertText(cursor, "\n");
        Messages::showStatusMessage(QStringLiteral("Info: Request started..."), KTextEditor::Message::Information, mainWindow_);
    }
}

void KateOllamaView::handle_ollamaRequestGotResponse(OllamaResponse ollamaResponse)
{
    if (ollamaResponse.getReceiver() != "editor" && ollamaResponse.getReceiver() != "")
        return;

    KTextEditor::View *view = mainWindow_->activeView();
    KTextEditor::Document *document = view->document();
    KTextEditor::Cursor cursor = view->cursorPosition();
    document->insertText(cursor, ollamaResponse.getResponseText());
    Messages::showStatusMessage(QStringLiteral("Info: Reply received..."), KTextEditor::Message::Information, mainWindow_);
}

void KateOllamaView::handle_ollamaRequestFinished(OllamaResponse ollamaResponse)
{
    if (ollamaResponse.getErrorMessage() != QString("")) {
        Messages::showStatusMessage(QStringLiteral("Error encountered: %0").arg(ollamaResponse.getErrorMessage()),
                                    KTextEditor::Message::Information,
                                    mainWindow_);
        qDebug() << "Error:" << ollamaResponse.getErrorMessage();
        qDebug() << "Model:" << plugin_->getModel();
        qDebug() << "System prompt:" << plugin_->getSystemPrompt();
    }

    if (ollamaResponse.getReceiver() == "editor" || ollamaResponse.getReceiver() == "") {
        KTextEditor::View *view = mainWindow_->activeView();
        KTextEditor::Document *document = view->document();
        KTextEditor::Cursor cursor = view->cursorPosition();
        document->insertText(cursor, "\n");
    }
}

QString KateOllamaView::getPrompt()
{
    Messages::showStatusMessage(QStringLiteral("Info: Getting prompt..."), KTextEditor::Message::Information, mainWindow_);
    KTextEditor::View *view = mainWindow_->activeView();
    KTextEditor::Document *document = view->document();
    QString text = document->text();

    QString lastMatch = ollamaSystem_->getPromptFromText(text);

    return lastMatch;
}

void KateOllamaView::ollamaRequest(QString prompt)
{
    Messages::showStatusMessage(QStringLiteral("Info: Setting up request..."), KTextEditor::Message::Information, mainWindow_);

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
