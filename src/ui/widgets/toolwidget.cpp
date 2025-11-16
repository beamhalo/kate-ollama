/*
 *  SPDX-FileCopyrightText: 2025 tfks <development@worloflinux.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include <KLocalizedString>
#include <KPluginFactory>
#include <KTextEditor/Document>
#include <KTextEditor/MainWindow>
#include <KTextEditor/View>

#include <QHBoxLayout>
#include <QIcon>
#include <QObject>
#include <QVBoxLayout>

#include "src/ollama/ollamaglobals.h"
#include "src/ui/tabs/maintab.h"
#include "src/ui/widgets/toolwidget.h"

OllamaToolWidget::OllamaToolWidget(KateOllamaPlugin* plugin, KTextEditor::MainWindow* mainWindow,
                                   OllamaSystem* ollamaSystem, QWidget* parent)
  : QWidget(parent), plugin_(plugin), mainWindow_(mainWindow), ollamaSystem_(ollamaSystem) {
  new QVBoxLayout(this);
  layout()->setContentsMargins({});
  layout()->addWidget(&tabWidget_);

  tabWidget_.addTab(new MainTab(plugin_, mainWindow, ollamaSystem_, this),
                    OllamaGlobals::PluginName);
  tabWidget_.setTabsClosable(true);

  connect(&tabWidget_, &QTabWidget::tabCloseRequested, this, [this](int idx) {
    if (auto w = tabWidget_.widget(idx)) {
      w->deleteLater();
    }
    tabWidget_.removeTab(idx);
  });
}

OllamaToolWidget::~OllamaToolWidget() {}

void OllamaToolWidget::newTab() {
  int index = tabWidget_.count() + 1;

  QString tabName =
      QString(OllamaGlobals::PluginName).append(" (").append(QString::number(index)).append(")");

  int i = tabWidget_.addTab(new MainTab(plugin_, mainWindow_, ollamaSystem_, this), tabName);

  tabWidget_.setCurrentIndex(i);
}
