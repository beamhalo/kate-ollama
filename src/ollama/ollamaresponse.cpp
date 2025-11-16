
#include "src/ollama/ollamaresponse.h"

void OllamaResponse::setReceiver(QString receiver) { receiver_ = receiver; }
QString OllamaResponse::getReceiver() { return receiver_; }

void OllamaResponse::setResponseText(QString responseText) { responseText_ = responseText; }
QString OllamaResponse::getResponseText() { return responseText_; }

void OllamaResponse::setErrorMessage(QString errorMessage) { errorMessage_ = errorMessage; }
QString OllamaResponse::getErrorMessage() { return errorMessage_; }
