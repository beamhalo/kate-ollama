/*
    SPDX-FileCopyrightText: 2025 Daniele Mte90 Scasciafratte <mte90net@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugin.h"
#include "settings.h"

#include "ollama/ollamasystem.h"
#include "ui/views/ollamaview.h"
#include "ui/widgets/toolwidget.h"

// KF headers
#include <KConfigGroup>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KSharedConfig>
#include <KTextEditor/Plugin>

using namespace Qt::Literals::StringLiterals;

K_PLUGIN_FACTORY_WITH_JSON(KateOllamaFactory, "kateollama.json", registerPlugin<KateOllamaPlugin>();)

KateOllamaPlugin::KateOllamaPlugin(QObject* parent, const QVariantList&) : KTextEditor::Plugin(parent) {
  readSettingsFromFile();
  m_ollama_system = new OllamaSystem(this);
  fetchModelList();
}

QObject* KateOllamaPlugin::createToolWindow(KTextEditor::MainWindow* mainWindow) {
  return new OllamaToolWidget(this, mainWindow);
}

QObject* KateOllamaPlugin::createView(KTextEditor::MainWindow* mainwindow) {
  if (!m_delayed_initialization_errors.isEmpty()) {
    QVariantMap vm;
    vm["category"] = "Ollama";
    vm["type"] = "Warning";
    vm["text"] = "\n -" + m_delayed_initialization_errors.join("\n -");
    mainwindow->showMessage(vm);
  }
  return new KateOllamaView(this, mainwindow);
}

KTextEditor::ConfigPage* KateOllamaPlugin::configPage(int number, QWidget* parent) {
  if (number != 0) {
    return nullptr;
  }
  m_config_page = new KateOllamaConfigPage(parent, this);
  return m_config_page;
}

void KateOllamaPlugin::readSettingsFromConfig() {
  m_ollama_url = m_config_page->url();
  m_ollama_model = m_config_page->model();
  for (const QString& model: m_model_settings.keys()) {
    m_model_settings[model] = m_config_page->settingsFor(model);
  }
}

void KateOllamaPlugin::readSettingsFromFile() {
  KConfigGroup group(KSharedConfig::openConfig(), "KateOllama");
  m_ollama_url = group.readEntry("URL", "http://localhost:11434");
  m_ollama_model = group.readEntry("Model");
}

void KateOllamaPlugin::fetchModelList() {
  connect(m_ollama_system, &OllamaSystem::modelsListLoaded, this, [this] (QStringList modelsList) {
    for (const QString& model: modelsList){
      m_model_settings[model] = OllamaModelSettings{};
      m_model_settings[model].systemPrompt =
          "You are a smart programming assistant. You prefer clean, modern code when writing c++."
          "You complete code based on the prompt and the surrounding code."
          "Take care to seamlessly integrate your response with the code preceding and following it."
          "Do not write comments. Do not write explanations. Do not write examples of any type."
          "Write only the code needed to fulfill requirements.";
    }
    if (m_model_settings.isEmpty()) {
      m_delayed_initialization_errors << i18n("Ollama: No models found.");
    } else if (!m_model_settings.contains(m_ollama_model)) {
      m_delayed_initialization_errors << i18n("Ollama: Model %1 not found.").arg(m_ollama_model);
      m_ollama_model = m_model_settings.begin().key();
      m_delayed_initialization_errors << i18n("Ollama: Selected %1 instead.").arg(m_ollama_model);;
    }
  });
  m_ollama_system->initialize(this);
}

#include <plugin.moc>
