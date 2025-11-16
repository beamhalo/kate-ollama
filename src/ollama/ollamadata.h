/*
 *  SPDX-FileCopyrightText: 2025 tfks <development@worloflinux.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef OLLAMA_DATA_H
#define OLLAMA_DATA_H

#include <QJsonObject>
#include <QString>
#include <QVector>

/*
 * Can be used to create the request object to all Ollama with.
 * Documentation: https://ollama.readthedocs.io/en/api/#generate-request-streaming
 */
class OllamaData {
 public:
  // Default constructor
  OllamaData();

  // Default destructor
  ~OllamaData();

  // Sets the sender from which the call originated. Can be used to determine what to do with the
  // response in the UI.
  void setSender(QString sender);
  // Gets the sender from which the call originated. Can be used to determine what to do with the
  // response in the UI.
  QString getSender() const;

  // (required) Sets the Ollama URL
  void setOllamaUrl(QString url);
  // (required) Gets the Ollama URL
  QString getOllamaUrl() const;

  // (required) Sets the model name
  void setModel(const QString& model);
  // (required) Gets the model name
  QString getModel() const;

  // Sets the prompt to generate a response for
  void setPrompt(const QString& prompt);
  // Gets the prompt to generate a response for
  QString getPrompt() const;

  // Sets the text after the model response
  void setSuffix(const QString& suffix);
  // Gets the text after the model response
  QString getSuffix() const;

  // Adds an image in Base64 format to an internal list which can be sent to Ollama
  void addImage(const QString& image);
  // Gets a list of images in Base64 format from an internal list
  QVector<QString> getImages() const;

  // Sets the format. The value json is only available option at the moment
  void setFormat(const QString& format);
  // Gets the format. The value json is only available option at the moment
  QString getFormat() const;

  // Sets additional model parameters listed in the documentation for the Modelfile such as
  // temperature Documentation:
  // https://ollama.readthedocs.io/en/modelfile/#valid-parameters-and-values
  void setOptions(const QString& options);
  // Gets additional model parameters listed in the documentation for the Modelfile such as
  // temperature Documentation:
  // https://ollama.readthedocs.io/en/modelfile/#valid-parameters-and-values
  QString getOptions() const;

  // Sets the system message (prompt) which is used.
  // Set this system message (prompt) to (overrides what is defined in the Modelfile)
  void setSystemPrompt(const QString& system);
  // Gets the system message (prompt) which is used.
  // Set this system message (prompt) to (overrides what is defined in the Modelfile)
  QString getSystemPrompt() const;

  // Sets the context parameter returned from a previous request to /generate, this can be used to
  // keep a short conversation
  void setContext(bool context);
  // Gets the context parameter returned from a previous request to /generate, this can be used to
  // keep a short conversation
  bool getContext() const;

  // Sets the is stream setting.
  // If false the response will be returned as a single response object, rather than a stream of
  // objects
  void setStream(bool stream);
  // Gets the is stream setting.
  // If false the response will be returned as a single response object, rather than a stream of
  // objects
  bool isStream() const;

  // Sets the is raw setting. If true no formatting will be applied to the prompt.
  // You may choose to use the raw parameter if you are specifying a full templated prompt in your
  // request to the API
  void setRaw(bool raw);
  // Gets the is raw setting. If true no formatting will be applied to the prompt.
  // You may choose to use the raw parameter if you are specifying a full templated prompt in your
  // request to the API
  bool isRaw() const;

  // Sets the keep alive setting. Default is 5m when not set.
  void setKeepAlive(bool keepAlive);
  // Gets the keep alive setting. Default is 5m when not set.
  bool isKeepAlive() const;

  // Converts all data, if filled, to a QJsonObject
  QJsonObject toJson() const;

  bool isOllamaUrlValid();

 private:
  QString sender_;
  QString url_;
  QString model_;
  QString prompt_;
  QString suffix_;
  QVector<QString> images_;
  QString format_;
  QString options_;
  QString system_;
  bool context_;
  bool stream_;
  bool raw_;
  bool keepAlive_;
};

#endif // OLLAMA_DATA_H
