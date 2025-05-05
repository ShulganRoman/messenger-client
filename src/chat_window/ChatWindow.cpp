#include "ChatWindow.h"
#include "AsyncClientConnection.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

ChatWindow::ChatWindow(const QString &selfUsername, const QString &peerUsername,
                       AsyncClientConnection *client, QWidget *parent)
    : QWidget(parent), client_(client), selfUsername_(selfUsername),
      peerUsername_(peerUsername) {
  setWindowTitle(u8"Чат с " + peerUsername_);
  resize(480, 640);

  auto *lay = new QVBoxLayout(this);
  messages_ = new QTextEdit;
  messages_->setReadOnly(true);
  input_ = new QLineEdit;
  input_->setPlaceholderText(u8"Введите сообщение…");
  auto *sendBtn = new QPushButton(u8"Отправить");

  lay->addWidget(messages_);
  lay->addWidget(input_);
  lay->addWidget(sendBtn);

  connect(sendBtn, &QPushButton::clicked, this, &ChatWindow::onSendClicked);
  connect(input_, &QLineEdit::returnPressed, this, &ChatWindow::onSendClicked);
  connect(client_, &AsyncClientConnection::messageReceived, this,
          &ChatWindow::onServerMessage);

  {
    QJsonObject metaOpen{
        {"entity", "open_chat"},
    };
    QJsonObject packetOpen{
        {"metadata", metaOpen}, {"from", selfUsername_}, {"to", peerUsername_}};
    QByteArray data =
        QJsonDocument(packetOpen).toJson(QJsonDocument::Compact) + '\n';
    client_->sendBuffer(data.toStdString());
  }
}

void ChatWindow::onSendClicked() {
  const QString text = input_->text().trimmed();
  if (text.isEmpty())
    return;

  QJsonObject meta{{"entity", "send_message"}, {"name_title", peerUsername_}};
  QJsonObject packet{{"metadata", meta},
                     {"from", selfUsername_},
                     {"to", peerUsername_},
                     {"text", text}};
  QByteArray data = QJsonDocument(packet).toJson(QJsonDocument::Compact) + '\n';
  client_->sendBuffer(data.toStdString());

  messages_->append(u8"<b>Я:</b> " + text);
  input_->clear();
}

void ChatWindow::onServerMessage(const QString &msg) {
  QJsonParseError err;
  const auto doc = QJsonDocument::fromJson(msg.toUtf8(), &err);
  if (err.error != QJsonParseError::NoError || !doc.isObject())
    return;

  const auto obj = doc.object();

  bool isHistory = true;
  for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
    if (it.key() == "metadata" || it.key() == "from" || it.key() == "text") {
      isHistory = false;
      break;
    }
  }
  if (isHistory) {

    for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
      const QString sender = it.key();
      const QString text = it.value().toString();
      messages_->append(u8"<b>" + sender + u8":</b> " + text);
    }
    return;
  }

  if (obj.contains("from") && obj.contains("text")) {
    const QString sender = obj["from"].toString();
    const QString text = obj["text"].toString();
    messages_->append(u8"<b>" + sender + u8":</b> " + text);
  }
}
