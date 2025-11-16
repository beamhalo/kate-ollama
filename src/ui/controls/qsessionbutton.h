#ifndef QSESSIONBUTTON_H
#define QSESSIONBUTTON_H

#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

class QSessionButton : public QWidget {
  Q_OBJECT

 public:
  explicit QSessionButton(QWidget* parent = nullptr, QString title = "");

  // Sets the uuid which identifies this session.
  void setUuid(QString uuid);
  // Gets the uuid which identifies this session.
  QString getUuid();

  QString generateUniqueId();

 signals:
  void sessionButtonClicked(const QString& identifier);

 private slots:
  void handleButtonClicked();

 private:
  QString uuid_;
  QString title_;
  QHBoxLayout* layout_;
  QLabel* label_;
  QPushButton* button_;
};

#endif // QSESSIONBUTTON_H
