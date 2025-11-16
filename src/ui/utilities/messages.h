#ifndef MESSAGES_H
#define MESSAGES_H

#include <KTextEditor/MainWindow>
#include <KTextEditor/Message>

class Messages {
 public:
  static void showStatusMessage(const QString& message, KTextEditor::Message::MessageType level,
                                KTextEditor::MainWindow* mainWindow);
};
#endif // MESSAGES_H
