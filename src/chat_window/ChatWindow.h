#pragma once
#include <QWidget>

class QTextEdit;
class QLineEdit;
class AsyncClientConnection;

class ChatWindow : public QWidget {
  Q_OBJECT
public:
  ChatWindow(const QString &selfUsername, const QString &peerUsername,
             AsyncClientConnection *client, QWidget *parent = nullptr);

private slots:
  void onSendClicked();
  void onServerMessage(const QString &msg);

private:
  AsyncClientConnection *client_;
  QString selfUsername_;
  QString peerUsername_;
  QTextEdit *messages_;
  QLineEdit *input_;
};
