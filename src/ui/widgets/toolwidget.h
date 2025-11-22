/*
 *  SPDX-FileCopyrightText: 2025 tfks <development@worloflinux.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef OLLAMATOOLWIDGET_HEADER_H
#define OLLAMATOOLWIDGET_HEADER_H

#include <KTextEditor/MainWindow>

#include <KXMLGUIClient>
#include <QTextBlockFormat>
#include <QTextCharFormat>

#include "src/ollama/ollamasystem.h"
#include "src/plugin.h"

class QLineEdit;
class QPushButton;
class QTextEdit;
class QTextDocument;

class OllamaToolWidget : public QWidget {
  Q_OBJECT

 public:
  explicit OllamaToolWidget(KateOllamaPlugin* plugin, KTextEditor::MainWindow* mainWindow,
                            QWidget* parent = nullptr);

  virtual ~OllamaToolWidget();

 public slots:
  void submit();

  void erase();

 private slots:
  void response(OllamaResponse);

 private:
  KateOllamaPlugin* plugin_;
  KTextEditor::MainWindow* mainWindow_ = nullptr;
  OllamaSystem* ollamaSystem_;
  QTextDocument* m_document;
  QTextEdit* m_chat_view;
  QLineEdit* m_chat_user;
  QPushButton* m_submit_btn;
  QPushButton* m_erase_history_btn;
  QList<OllamaRequest::ChatMessage> m_history;
  QTextBlockFormat m_asst_bfmt;
  QTextCharFormat m_asst_cfmt;
  QTextBlockFormat m_code_bfmt;
  QTextCharFormat m_code_cfmt;
  QTextBlockFormat m_user_bfmt;
  QTextCharFormat m_user_cfmt;

};
#endif // OLLAMATOOLWIDGET_HEADER_H
