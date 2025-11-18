/*
 *  SPDX-FileCopyrightText: 2025 tfks <development@worloflinux.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef OLLAMASYSTEM_H
#define OLLAMASYSTEM_H

#include "src/ollama/ollamamodelsettings.h"
#include "src/ollama/ollamarequest.h"
#include "src/ollama/ollamaresponse.h"

#include <QMap>
#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;
class KateOllamaPlugin;

class OllamaSystem : public QObject {

  Q_OBJECT

 public:
  OllamaSystem(QObject*);
  ~OllamaSystem();

 public slots:
  void initialize(KateOllamaPlugin*);
  void ollamaRequest(OllamaRequest);
  void killModel();

 private slots:
  void processModelsResponse(QNetworkReply*);

 private:
  QByteArray formatRequest(OllamaRequest);
  OllamaResponse parseResponse(QByteArray);

 signals:
  void modelsListLoaded(QStringList);
  void errorReceived(QString);
  void streamingResponse(OllamaResponse);
  void responseFinished(OllamaResponse);

 private:
  KateOllamaPlugin* m_plugin;
  QStringList m_messages;
  QStringList m_errors;
  QNetworkAccessManager* m_net_models;
  QNetworkAccessManager* m_net_requests;
  bool m_kill_requested = false;

};

#endif // OLLAMASYSTEM_H
