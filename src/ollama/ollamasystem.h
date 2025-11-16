/*
 *  SPDX-FileCopyrightText: 2025 tfks <development@worloflinux.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef OLLAMASYSTEM_H
#define OLLAMASYSTEM_H

#include <QJsonArray>
#include <QObject>

#include "src/ollama/ollamadata.h"
#include "src/ollama/ollamaresponse.h"

class OllamaSystem : public QObject {
  Q_OBJECT

 public:
  OllamaSystem(QObject* parent);
  ~OllamaSystem();

  void fetchModels(OllamaData ollamaData);
  void ollamaRequest(OllamaData data);
  QString getPromptFromText(QString text);

 signals:
  void signal_modelsListLoaded(const QList<QJsonValue>& modelsList);
  void signal_errorFetchingModelsList(QString error);

  void signal_ollamaRequestMetaDataChanged(OllamaResponse ollamaResponse);
  void signal_ollamaRequestGotResponse(OllamaResponse ollamaResponse);
  void signal_ollamaRequestFinished(OllamaResponse ollamaResponse);

 private:
  QObject* parent = nullptr;
  QList<QJsonValue> m_modelsList;
  QStringList m_errors;
  QStringList m_messages;
};

#endif // OLLAMASYSTEM_H
