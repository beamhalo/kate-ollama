#ifndef QOLLAMAPLAINTEXTEDIT_H
#define QOLLAMAPLAINTEXTEDIT_H

#include <qevent.h>
#include <QApplication>
#include <QKeyEvent>
#include <QPlainTextEdit>
#include <QScrollBar>

class QOllamaPlainTextEdit : public QPlainTextEdit {
  Q_OBJECT
 public:
  explicit QOllamaPlainTextEdit(QWidget* parent = nullptr) : QPlainTextEdit(parent) {}

 signals:
  void signal_enterKeyWasPressed(QKeyEvent* event);

 protected:
  void keyPressEvent(QKeyEvent* event) override {
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
      // Handle the Enter or Return key press here
      qDebug() << "Enter/Return key pressed (QOllamaPlainTextEdit)";

      emit signal_enterKeyWasPressed(event);
    } else {
      QPlainTextEdit::keyPressEvent(event);
    }
  }

 public slots:
  void onTextChanged() { verticalScrollBar()->setValue(verticalScrollBar()->maximum()); }
};

#endif // QOLLAMAPLAINTEXTEDIT_H
