/*
 *  SPDX-FileCopyrightText: 2025 Daniele Mte90 Scasciafratte <mte90net@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>
#include <KTextEditor/ConfigPage>

#include <QComboBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTextEdit>
#include <QVBoxLayout>
#include <algorithm>

#include "src/plugin.h"
#include "src/settings.h"

KateOllamaConfigPage::KateOllamaConfigPage(QWidget* parent, KateOllamaPlugin* plugin)
  : KTextEditor::ConfigPage(parent), plugin_(plugin) {
  QVBoxLayout* layout = new QVBoxLayout(this);

  // URL
  {
    auto* hl = new QHBoxLayout;

    auto label = new QLabel(i18n("Ollama URL"));
    hl->addWidget(label);

    ollamaURLText_ = new QLineEdit(this);
    hl->addWidget(ollamaURLText_);

    layout->addLayout(hl);
  }

  // Available Models
  {
    auto* hl = new QHBoxLayout;

    auto label = new QLabel(i18n("Available Models"));
    hl->addWidget(label);

    modelsComboBox_ = new QComboBox(this);
    hl->addWidget(modelsComboBox_);

    layout->addLayout(hl);
  }

  // System Prompt
  {
    auto* hl = new QHBoxLayout;

    auto label = new QLabel(i18n("System Prompt"));
    hl->addWidget(label);

    systemPromptEdit_ = new QTextEdit(this);
    systemPromptEdit_->setGeometry(100, 100, 300, 200);
    hl->addWidget(systemPromptEdit_);

    layout->addLayout(hl);
  }

  layout->addStretch();

  // Error/Info label
  {
    infoLabel_ = new QLabel(this);
    infoLabel_->setVisible(false); // its hidden initially
    infoLabel_->setWordWrap(true);
    layout->addWidget(infoLabel_);
  }

  setLayout(layout);

  loadSettings();
  QObject::connect(modelsComboBox_, &QComboBox::currentIndexChanged, this,
                   &KateOllamaConfigPage::changed);
  QObject::connect(systemPromptEdit_, &QTextEdit::textChanged, this,
                   &KateOllamaConfigPage::changed);
  QObject::connect(ollamaURLText_, &QLineEdit::textEdited, this, &KateOllamaConfigPage::changed);
}

void KateOllamaConfigPage::fetchModelList() {
  QNetworkAccessManager* manager = new QNetworkAccessManager(this);
  connect(manager, &QNetworkAccessManager::finished, this, [this](QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
      infoLabel_->setVisible(false); // Hide the label on success

      QByteArray responseData = reply->readAll();
      QJsonDocument jsonDoc   = QJsonDocument::fromJson(responseData);

      if (jsonDoc.isObject()) {
        QJsonObject jsonObj = jsonDoc.object();
        if (jsonObj.contains("models") && jsonObj["models"].isArray()) {
          QJsonArray modelsArray = jsonObj["models"].toArray();
          QList<QJsonValue> modelsList;
          for (const QJsonValue& value : modelsArray) {
            modelsList.append(value);
          }
          std::sort(modelsList.begin(), modelsList.end(),
                    [](const QJsonValue& a, const QJsonValue& b) {
                      return a.toObject()["name"].toString().toLower()
                           < b.toObject()["name"].toString().toLower();
                    });

          int modelSelected = -1;
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
      }
    } else {
      qWarning() << "Error fetching model list:" << reply->errorString();
      // Show error in UI
      infoLabel_->setText(i18n("Error fetching model list: %1", reply->errorString()));
    }
    reply->deleteLater();
  });

  QUrl url(ollamaURLText_->text() + "/api/tags");
  QNetworkRequest request(url);
  manager->get(request);

  infoLabel_->setText(i18n("Loading model list..."));
  infoLabel_->setVisible(true);
}

QString KateOllamaConfigPage::name() const { return i18n("Ollama"); }

QString KateOllamaConfigPage::fullName() const {
  return i18nc("Groupbox title", "Ollama Settings");
}

QIcon KateOllamaConfigPage::icon() const {
  return QIcon::fromTheme(QLatin1String("project-open"),
                          QIcon::fromTheme(QLatin1String("view-list-tree")));
}

void KateOllamaConfigPage::apply() {
  // Save settings to disk
  KConfigGroup group(KSharedConfig::openConfig(), "KateOllama");
  group.writeEntry("Model", modelsComboBox_->currentText());
  group.writeEntry("URL", ollamaURLText_->text());
  group.writeEntry("SystemPrompt", systemPromptEdit_->toPlainText());
  group.sync();

  // Update the cached variables in Plugin
  plugin_->setModel(modelsComboBox_->currentText());
  plugin_->setModel(systemPromptEdit_->toPlainText());
  plugin_->setOllamaUrl(ollamaURLText_->text());
}

void KateOllamaConfigPage::defaults() {
  ollamaURLText_->setText("http://localhost:11434");
  systemPromptEdit_->setPlainText(
      "You are a smart coder assistant, code comments are in the prompt language. You don't "
      "explain, you add only code comments.");
}

void KateOllamaConfigPage::reset() {
  // Reset the UI values to last known settings
  modelsComboBox_->setCurrentText(plugin_->getModel());
  systemPromptEdit_->setPlainText(plugin_->getSystemPrompt());
  ollamaURLText_->setText(plugin_->getOllamaUrl());
}

void KateOllamaConfigPage::loadSettings() {
  KConfigGroup group(KSharedConfig::openConfig(), "KateOllama");

  QString model        = group.readEntry("Model");
  QString url          = group.readEntry("URL");
  QString systemPrompt = group.readEntry("SystemPrompt");

  if (url.isEmpty()) {
    defaults();
  }

  ollamaURLText_->setText(url);
  systemPromptEdit_->setPlainText(systemPrompt);

  plugin_->setSystemPrompt(systemPromptEdit_->toPlainText());
  plugin_->setOllamaUrl(ollamaURLText_->text());
  plugin_->setModel(model);

  fetchModelList();
}
