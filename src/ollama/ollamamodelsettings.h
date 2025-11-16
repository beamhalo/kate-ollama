#ifndef OLLAMAMODELSETTINGS_H
#define OLLAMAMODELSETTINGS_H

#include <QString>

struct OllamaModelSettings {
  QString modelName{};
  QString systemPrompt{""};
  QString max_tokens{"100"};
  double temperature = 0;
};

#endif // OLLAMAMODELSETTINGS_H
