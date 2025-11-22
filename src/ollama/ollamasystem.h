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
  void ollamaChat(OllamaRequest);
  void killModel();

 private slots:
  void processModelsResponse(QNetworkReply*);

 private:
  void extracted(OllamaRequest& req);
  QByteArray formatRequest(const OllamaRequest&);
  OllamaResponse parseResponse(QByteArray);

 signals:
  void modelsListLoaded(QStringList);
  void errorReceived(QString);
  void streamingResponse(OllamaResponse);
  void responseFinished(OllamaResponse);
  void chatResponseFinished(OllamaResponse);

 private:
  KateOllamaPlugin* m_plugin;
  QStringList m_messages;
  QStringList m_errors;
  QNetworkAccessManager* m_net_models;
  QNetworkAccessManager* m_net_requests;
  bool m_kill_requested = false;
  QString m_current_chat_response;

};

#endif // OLLAMASYSTEM_H
