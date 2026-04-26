#ifndef OLLAMAREQUEST_H
#define OLLAMAREQUEST_H

#include <QList>
#include <QString>

struct OllamaRequest {
  int context_id;
  struct ChatMessage {
    QString role;
    QString content;
  };
  QString prompt;
  QString suffix;
  QList<ChatMessage> history;
};

#endif // OLLAMAREQUEST_H
