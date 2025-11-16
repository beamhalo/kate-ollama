/*
    SPDX-FileCopyrightText: 2025 Daniele Mte90 Scasciafratte <mte90net@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
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

#include "plugin.h"
#include "ollama/ollamasystem.h"

#include "settings.h"
#include "ollama/ollamadata.h"
#include "ollama/ollamasystem.h"
#include "ui/views/ollamaview.h"
#include "ui/widgets/toolwidget.h"

using namespace Qt::Literals::StringLiterals;

K_PLUGIN_FACTORY_WITH_JSON(KateOllamaFactory, "kateollama.json",
                           registerPlugin<KateOllamaPlugin>();)

enum MessageType { Log, Info, Warn, Error };

KateOllamaPlugin::KateOllamaPlugin(QObject* parent, const QVariantList&)
  : KTextEditor::Plugin(parent) {
  ollamaSystem_ = new OllamaSystem(this);
}

QObject* KateOllamaPlugin::createToolWindow(KTextEditor::MainWindow* mainWindow) {
  return new OllamaToolWidget(this, mainWindow, ollamaSystem_);
}

QObject* KateOllamaPlugin::createView(KTextEditor::MainWindow* mainwindow) {
  return new KateOllamaView(this, mainwindow, ollamaSystem_);
}

KTextEditor::ConfigPage* KateOllamaPlugin::configPage(int number, QWidget* parent) {
  if (number != 0) {
    return nullptr;
  }
  return new KateOllamaConfigPage(parent, this);
}

void KateOllamaPlugin::setModel(QString model) { model_ = model; }
QString KateOllamaPlugin::getModel() { return model_; }

void KateOllamaPlugin::setSystemPrompt(QString systemPrompt) { systemPrompt_ = systemPrompt; }
QString KateOllamaPlugin::getSystemPrompt() { return systemPrompt_; }

void KateOllamaPlugin::setOllamaUrl(QString ollamaUrl) { ollamaUrl_ = ollamaUrl; }
QString KateOllamaPlugin::getOllamaUrl() { return ollamaUrl_; }

void KateOllamaPlugin::setOllamaData(OllamaData ollamaData) { ollamaData_ = ollamaData; }
OllamaData KateOllamaPlugin::getOllamaData() { return ollamaData_; }

#include <plugin.moc>
