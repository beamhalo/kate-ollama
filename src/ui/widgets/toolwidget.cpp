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

  m_erase_history_btn = new QPushButton(QIcon::fromTheme("edit-delete"), "Erase history");
  connect(m_erase_history_btn, &QPushButton::clicked, this, &OllamaToolWidget::erase);
  hlay->addWidget(m_erase_history_btn);

  m_chat_user = new QLineEdit;
  m_chat_user->setPlaceholderText("Chat with ollama...");
  connect(m_chat_user, &QLineEdit::returnPressed, this, &OllamaToolWidget::submit);
  hlay->addWidget(m_chat_user);

  m_submit_btn = new QPushButton(QIcon::fromTheme("document-send"), "Send");
  connect(m_submit_btn, &QPushButton::clicked, this, &OllamaToolWidget::submit);
  hlay->addWidget(m_submit_btn);

  vlay->addLayout(hlay);
  connect(ollamaSystem_, &OllamaSystem::chatResponseFinished, this, &OllamaToolWidget::response);

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
  ollamaSystem_->ollamaChat(req);
  QTextCursor cursor(m_document);
  cursor.movePosition(QTextCursor::End);
  cursor.insertBlock(m_user_bfmt, m_user_cfmt);
  cursor.insertText(m_chat_user->text());
  cursor.insertText(QString('\n'));
  m_chat_user->clear();
  auto qsb = m_chat_view->verticalScrollBar();
  qsb->setValue(qsb->maximum());
}

void OllamaToolWidget::erase() {
  m_document->clear();
  m_history.clear();
}

void OllamaToolWidget::response(OllamaResponse resp) {
  m_history.emplaceBack(QString("assistant"), resp.responseText);
  QTextCursor cursor(m_document);
  cursor.movePosition(QTextCursor::End);
  cursor.insertBlock(m_asst_bfmt, m_asst_cfmt);
  cursor.insertMarkdown(resp.responseText);
  cursor.insertText(QString('\n'));
  auto qsb = m_chat_view->verticalScrollBar();
  qsb->setValue(qsb->maximum());
}
