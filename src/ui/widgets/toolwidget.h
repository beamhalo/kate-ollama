/*
 *  SPDX-FileCopyrightText: 2025 tfks <development@worloflinux.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef OLLAMATOOLWIDGET_HEADER_H
#define OLLAMATOOLWIDGET_HEADER_H

#include <KTextEditor/MainWindow>

#include <KXMLGUIClient>
#include <QTabWidget>
#include <QTextBrowser>

#include "src/ollama/ollamasystem.h"
#include "src/plugin.h"

class OllamaToolWidget : public QWidget {
  Q_OBJECT

 public:
  explicit OllamaToolWidget(KateOllamaPlugin* plugin, KTextEditor::MainWindow* mainWindow,
                            QWidget* parent = nullptr);

  virtual ~OllamaToolWidget();

  // Add's a new tab
  void newTab();

  void onViewChanged(KTextEditor::View* v);

 private:
  KateOllamaPlugin* plugin_;
  KTextEditor::MainWindow* mainWindow_ = nullptr;
  QTabWidget tabWidget_;
  OllamaSystem* ollamaSystem_;
};
#endif // OLLAMATOOLWIDGET_HEADER_H
