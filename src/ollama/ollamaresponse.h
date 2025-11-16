#ifndef OLLAMARESPONSE_H
#define OLLAMARESPONSE_H

#include <QString>

class OllamaResponse {
 public:
  // Sets the receiver. This can be used to control what to do with the data in the receiving UI.
  void setReceiver(QString receiver);
  // Gets the receiver. This can be used to control what to do with the data in the receiving UI.
  QString getReceiver();

  // Sets the reponse text.
  void setResponseText(QString responseText);
  // Gets the response text.
  QString getResponseText();

  // Gets an error message when applicable.
  void setErrorMessage(QString errorMessage);
  // Sets an error message when applicable.
  QString getErrorMessage();

 private:
  QString receiver_;
  QString responseText_;
  QString errorMessage_;
};

#endif // OLLAMARESPONSE_H
