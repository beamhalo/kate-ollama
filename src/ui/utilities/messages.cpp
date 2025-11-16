/*
 *  SPDX-FileCopyrightText: 2025 tfks <development@worloflinux.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include <KTextEditor/Document>
#include <KTextEditor/MainWindow>
#include <KTextEditor/Message>
#include <KTextEditor/View>

#include "src/ui/utilities/messages.h"

void Messages::showStatusMessage(const QString& message, KTextEditor::Message::MessageType level,
                                 KTextEditor::MainWindow* mainWindow) {
  KTextEditor::View* view = mainWindow->activeView();
  if (!view || !view->document()) {
    return;
  }

  auto kmsg = new KTextEditor::Message(message, level);
  kmsg->setPosition(KTextEditor::Message::BottomInView);
  kmsg->setAutoHide(500);
  kmsg->setView(view);
  view->document()->postMessage(kmsg);
}
