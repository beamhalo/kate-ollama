/*
 *  SPDX-FileCopyrightText: 2025 tfks <development@worloflinux.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "src/ollama/ollamasystem.h"

#include "src/plugin.h"

// KF Headers
#include <KLocalizedString>

#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStringLiteral>

OllamaSystem::OllamaSystem(QObject* parent) : QObject(parent),
                                              m_plugin(nullptr),
                                              m_net_models(new QNetworkAccessManager(this)),
                                              m_net_requests(new QNetworkAccessManager(this)) {
  connect(m_net_models, &QNetworkAccessManager::finished, this, &OllamaSystem::processModelsResponse);
}

OllamaSystem::~OllamaSystem() {}

void OllamaSystem::initialize(KateOllamaPlugin* plugin) {
  m_plugin = plugin;
  QUrl url = m_plugin->currentUrl();
  url.setPath("/api/tags");
  QNetworkRequest request(url);
  m_net_models->get(request);
}

void OllamaSystem::processModelsResponse(QNetworkReply* reply) {
  QStringList models;
  if (reply->error() == QNetworkReply::NoError) {
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
    if (jsonDoc.isObject()) {
      QJsonObject jsonObj = jsonDoc.object();
      if (jsonObj.contains("models") && jsonObj["models"].isArray()) {
        const QJsonArray modelsArray = jsonObj["models"].toArray();
        for (const QJsonValue& value : modelsArray) {
          QString name = value.toObject().value("name").toString();
          models.append(name);
        }
      }
    }
    emit modelsListLoaded(models);
  } else {
    emit errorReceived(i18n("Error fetching model list: %1", reply->errorString()));
  }
  reply->deleteLater();
}

QByteArray OllamaSystem::formatRequest(const OllamaRequest& req) {
  QJsonObject json;
  QString model = m_plugin->currentModel();
  json.insert("model", QJsonValue(model));
  QJsonArray history;
  for (const auto& msg: req.history) {
    QJsonObject message;
    message.insert("role", msg.role);
    message.insert("content", msg.content);
    history.append(message);
  }
  if (!history.isEmpty()) {
    json.insert("messages", history);
  }
  if (!req.prompt.isEmpty()) {
    json.insert("prompt", QJsonValue(req.prompt));
  }
  if (!req.suffix.isEmpty()) {
    json.insert("suffix", QJsonValue(req.suffix));
  }
  const auto& mc = m_plugin->modelConfig(model);
  if (!mc.systemPrompt.isEmpty()) {
    json.insert("system", QJsonValue(mc.systemPrompt));
  }
  if (!mc.max_tokens.isEmpty()) {
    json.insert("max_tokens", QJsonValue(mc.max_tokens));
  }
  json.insert("temperature", QJsonValue(mc.temperature));
  return QJsonDocument(json).toJson();
}

OllamaResponse OllamaSystem::parseResponse(QByteArray data) {
  OllamaResponse resp;
  QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
  QJsonObject jsonObj = jsonDoc.object();
  if (jsonObj.contains("response")) {
    resp.responseText = jsonObj["response"].toString();
  } else if (jsonObj.contains("message")) {
    resp.responseText = jsonObj.value("message").toObject().value("content").toString();
  }
  return resp;
}

void OllamaSystem::request(const QUrl& url, const OllamaRequest& req) {

  Q_ASSERT(m_plugin);

  m_kill_requested = false;

  QNetworkRequest request(url);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  QNetworkReply* reply = m_net_requests->post(request, formatRequest(req));
  int id = req.context_id;
  connect(reply, &QNetworkReply::readyRead, this, [this, id, reply]() {
    if (reply->error() == QNetworkReply::NoError) {
      if (m_kill_requested) {
        reply->abort();
      }
      QByteArray responseChunk = reply->readAll();
      auto resp = parseResponse(responseChunk);
      resp.context_id = id;
      emit responsePart(resp);
    } else {
      emit errorReceived(i18n("Error parsing response: %1", reply->errorString()));
    }
  });

  connect(reply, &QNetworkReply::finished, this, [this, id, reply]() {
    if (reply->error() == QNetworkReply::NoError) {
      QByteArray responseChunk = reply->readAll();
      auto resp = parseResponse(responseChunk);
      resp.context_id = id;
      emit responseFinished(resp);
    } else {
      emit errorReceived(i18n("Error parsing response: %1", reply->errorString()));
    }
    reply->deleteLater();
  });
}

void OllamaSystem::autofill(const OllamaRequest& req) {
  QUrl url = m_plugin->currentUrl();
  url.setPath("/api/generate");
  request(url, req);
}

void OllamaSystem::chat(const OllamaRequest& req) {
  QUrl url = m_plugin->currentUrl();
  url.setPath("/api/chat");
  request(url, req);
}

void OllamaSystem::killModel() {
  m_kill_requested = true;
}
