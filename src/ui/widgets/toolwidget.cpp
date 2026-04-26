/*
 *  SPDX-FileCopyrightText: 2025 tfks <development@worloflinux.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include <KLocalizedString>
#include <KPluginFactory>
#include <KTextEditor/Document>
#include <KTextEditor/MainWindow>
#include <KTextEditor/View>

#include <QApplication>
#include <QHBoxLayout>
#include <QIcon>
#include <QLineEdit>
#include <QPalette>
#include <QPushButton>
#include <QScrollBar>
#include <QTextBrowser>
#include <QTextBlock>
#include <QVBoxLayout>

#include "src/ui/widgets/toolwidget.h"

OllamaToolWidget::OllamaToolWidget(KateOllamaPlugin* plugin, KTextEditor::MainWindow* mainWindow,
                                   QWidget* parent)
  : QWidget(parent), plugin_(plugin), mainWindow_(mainWindow), ollamaSystem_(plugin->getOllama()) {

  auto vlay = new QVBoxLayout(this);

  m_document = new QTextDocument(this);
  m_chat_view = new QTextBrowser(this);
  m_chat_view->setReadOnly(true);
  m_chat_view->setDocument(m_document);
  vlay->addWidget(m_chat_view);

  auto hlay = new QHBoxLayout;



  m_submit = new QAction(QIcon::fromTheme("document-send"), "Send", this);
  connect(m_submit, &QAction::triggered, this, &OllamaToolWidget::submit);
  m_erase_history = new QAction(QIcon::fromTheme("edit-delete"), "Erase history", this);
  connect(m_erase_history, &QAction::triggered, this, &OllamaToolWidget::erase);

  m_chat_user = new QLineEdit;
  m_chat_user->setPlaceholderText("Chat with ollama...");
  connect(m_chat_user, &QLineEdit::returnPressed, m_submit, &QAction::trigger);

  auto erasebtn = new QPushButton;
  erasebtn->setText(m_erase_history->text());
  erasebtn->setIcon(m_erase_history->icon());
  connect(erasebtn, &QPushButton::clicked, m_erase_history, &QAction::trigger);

  auto submitbtn = new QPushButton;
  submitbtn->setText(m_submit->text());
  submitbtn->setIcon(m_submit->icon());
  connect(submitbtn, &QPushButton::clicked, m_submit, &QAction::trigger);

  hlay->addWidget(erasebtn);
  hlay->addWidget(m_chat_user);
  hlay->addWidget(submitbtn);

  vlay->addLayout(hlay);
  connect(ollamaSystem_, &OllamaSystem::responsePart, this, &OllamaToolWidget::response);
  connect(ollamaSystem_, &OllamaSystem::responseFinished, this, &OllamaToolWidget::responseFinished);

  m_asst_bfmt.setAlignment(Qt::AlignLeft);
  m_asst_bfmt.setLeftMargin(25);
  m_asst_bfmt.setRightMargin(250);
  m_code_bfmt.setAlignment(Qt::AlignLeft);
  m_code_cfmt.setFontFamilies(QStringList("monospace"));
  m_user_bfmt.setAlignment(Qt::AlignRight);
  m_user_bfmt.setRightMargin(25);
  m_user_bfmt.setLeftMargin(250);
  m_user_cfmt.setFontItalic(true);

}

OllamaToolWidget::~OllamaToolWidget() {}

void OllamaToolWidget::submit() {
  m_history.emplaceBack(QString("user"), m_chat_user->text());
  OllamaRequest req;
  req.history = m_history;
  req.context_id = --m_request_id;
  ollamaSystem_->chat(req);
  QTextCursor cursor(m_document);
  cursor.movePosition(QTextCursor::End);
  cursor.insertBlock(m_user_bfmt, m_user_cfmt);
  cursor.insertText(m_chat_user->text());
  cursor.insertText(QString('\n'));
  m_chat_user->clear();
  cursor.insertBlock(m_asst_bfmt, m_asst_cfmt);
  m_cursor = cursor.position();
  auto qsb = m_chat_view->verticalScrollBar();
  qsb->setValue(qsb->maximum());
  m_history.emplaceBack(QString("assistant"), QString());
}

void OllamaToolWidget::erase() {
  m_document->clear();
  m_history.clear();
}

void OllamaToolWidget::response(OllamaResponse resp) {
  if (resp.context_id != m_request_id) {
    return;
  }
  m_history.back().content.append(resp.responseText);
  QTextCursor cursor(m_document);
  cursor.movePosition(QTextCursor::End);
  cursor.insertText(resp.responseText);
  auto qsb = m_chat_view->verticalScrollBar();
  qsb->setValue(qsb->maximum());
}

void OllamaToolWidget::responseFinished(OllamaResponse resp) {
  if (resp.context_id != m_request_id) {
    return;
  }
  response(resp);
  QTextCursor cursor(m_document);
  cursor.setPosition(m_cursor);
  cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
  auto full = cursor.selectedText();
  cursor.removeSelectedText();
  cursor.insertBlock(m_asst_bfmt, m_asst_cfmt);
  cursor.insertMarkdown(full);
}
