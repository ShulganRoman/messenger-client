#pragma once

#include "AsyncClientConnection.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>
#include <boost/json.hpp>

class SignIn : public QWidget {
  Q_OBJECT
public:
  explicit SignIn(AsyncClientConnection *client, QWidget *parent = nullptr);

signals:
  void signUpRequested();

  void signInSuccess(const QString &username);

private slots:
  void onSignInClicked();
  void onSwitchToSignUp();
  void onConnected();
  void onError(const QString &err);
  void onBufferSent();
  void onMessageReceived(const QString &msg);

private:
  AsyncClientConnection *client_;
  QLineEdit *loginInput_;
  QLineEdit *passwordInput_;
  QPushButton *signInButton_;
  QPushButton *toSignUpButton_;
  QLabel *statusLabel_;
  bool socketIsConnected_{false};
};
