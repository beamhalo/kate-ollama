/*
 *  SPDX-FileCopyrightText: 2025 tfks <development@worloflinux.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <QHashFunctions>
#include <QJsonArray>
#include <QUrl>

#include "src/ollama/ollamadata.h"

OllamaData::OllamaData() : context_(false), stream_(false) {}

OllamaData::~OllamaData() {}

void OllamaData::setSender(QString sender) { sender_ = sender; }
QString OllamaData::getSender() const { return sender_; }

void OllamaData::setOllamaUrl(QString url) {
  if (url.endsWith('/')) {
    url.removeLast();
  }
  url_ = url;
}
QString OllamaData::getOllamaUrl() const { return url_; }

void OllamaData::setModel(const QString& model) { model_ = model; }
QString OllamaData::getModel() const { return model_; }

void OllamaData::setPrompt(const QString& prompt) { prompt_ = prompt; }
QString OllamaData::getPrompt() const { return prompt_; }

void OllamaData::setSuffix(const QString& suffix) { suffix_ = suffix; }
QString OllamaData::getSuffix() const { return suffix_; }

void OllamaData::addImage(const QString& image) { images_.append(image); }
QVector<QString> OllamaData::getImages() const { return images_; }

void OllamaData::setFormat(const QString& format) { format_ = format; }
QString OllamaData::getFormat() const { return format_; }

void OllamaData::setOptions(const QString& options) { options_ = options; }
QString OllamaData::getOptions() const { return options_; }

void OllamaData::setSystemPrompt(const QString& system) { system_ = system; }
QString OllamaData::getSystemPrompt() const { return system_; }

void OllamaData::setContext(bool context) { context_ = context; }
bool OllamaData::getContext() const { return context_; }

void OllamaData::setStream(bool stream) { stream_ = stream; }
bool OllamaData::isStream() const { return stream_; }

void OllamaData::setRaw(bool raw) { raw_ = raw; }
bool OllamaData::isRaw() const { return raw_; }

void OllamaData::setKeepAlive(bool keepAlive) { keepAlive_ = keepAlive; }
bool OllamaData::isKeepAlive() const { return keepAlive_; }

QJsonObject OllamaData::toJson() const {
  QJsonObject json;

  if (!model_.isEmpty()) {
    json.insert("model", QJsonValue(model_));
  }
  if (!prompt_.isEmpty()) {
    json.insert("prompt", QJsonValue(prompt_));
  }
  if (!suffix_.isEmpty()) {
    json.insert("suffix", QJsonValue(suffix_));
  }
  if (!format_.isEmpty()) {
    json.insert("format", QJsonValue(format_));
  }
  if (!options_.isEmpty()) {
    json.insert("options", QJsonValue(options_));
  }
  if (!system_.isEmpty()) {
    json.insert("system", QJsonValue(system_));
  }
  if (context_) {
    json.insert("context", QJsonValue(context_));
  }
  if (stream_) {
    json.insert("stream", QJsonValue(stream_));
  }
  if (raw_) {
    json.insert("raw", QJsonValue(raw_));
  }
  if (keepAlive_) {
    json.insert("keep_alive", QJsonValue(keepAlive_));
  }

  QJsonArray imageArray;
  for (const auto& image : images_) {
    imageArray.append(QJsonValue(image));
  }
  if (!imageArray.isEmpty()) {
    json.insert("images", imageArray);
  }

  return json;
}

bool OllamaData::isOllamaUrlValid() {
  QUrl qUrl(url_);

  return qUrl.isValid();
}
