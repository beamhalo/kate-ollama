/*
    SPDX-FileCopyrightText: 2025 Daniele Mte90 Scasciafratte <mte90net@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KATEOLLAMAPLUGIN_H
#define KATEOLLAMAPLUGIN_H

// KF headers
#include "ollama/ollamasystem.h"
#include <KTextEditor/Document>
#include <KTextEditor/MainWindow>
#include <KTextEditor/Plugin>
#include <KTextEditor/SessionConfigInterface>
#include <KTextEditor/View>
#include <KXMLGUIClient>
#include <QString>

class KateOllamaConfigPage;

class KateOllamaPlugin : public KTextEditor::Plugin {
  Q_OBJECT

 public:
  explicit KateOllamaPlugin(QObject* parent, const QVariantList& = QVariantList());

  QObject* createToolWindow(KTextEditor::MainWindow* mainWindow);
  QObject* createView(KTextEditor::MainWindow* mainWindow) override;

  void readSettings();

  void fetchModelList();

  int configPages() const override { return 1; }

  KTextEditor::ConfigPage* configPage(int number = 0, QWidget* parent = nullptr) override;

  QString currentModel() { return m_ollama_model; };
  QUrl currentUrl() { return m_ollama_url; };
  OllamaModelSettings modelConfig(QString model) { return m_model_settings.value(model); };

  OllamaSystem* getOllama() { return m_ollama_system; }

 private:
  OllamaSystem* m_ollama_system;
  KateOllamaConfigPage* m_config_page;
  QString m_ollama_model;
  QUrl m_ollama_url;
  QMap<QString, OllamaModelSettings> m_model_settings;
  QStringList m_delayed_initialization_errors;

};

#endif // KATEOLLAMAPLUGIN_H
