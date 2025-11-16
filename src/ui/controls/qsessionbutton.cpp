#include <QHBoxLayout>
#include <QUuid>

#include "src/ui/controls/qsessionbutton.h"

QSessionButton::QSessionButton(QWidget* parent, QString title) : QWidget(parent), title_(title) {
  layout_ = new QHBoxLayout(this);
  label_  = new QLabel(QString(title), this);
  button_ = new QPushButton(QIcon::fromTheme(QStringLiteral("edittrash")), QString(), this);
  layout_->addWidget(label_);
  layout_->addWidget(button_);

  this->setLayout(layout_);
}

void QSessionButton::handleButtonClicked() { emit sessionButtonClicked(uuid_); }

void QSessionButton::setUuid(QString uuid) { uuid_ = uuid; }
QString QSessionButton::getUuid() { return uuid_; }

QString QSessionButton::generateUniqueId() {
  QUuid uuid  = QUuid::createUuid();
  QString uid = uuid.toString(QUuid::WithoutBraces); // Remove braces from the UUID string
  return uid;
}
