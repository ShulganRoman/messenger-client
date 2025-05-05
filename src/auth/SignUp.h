#pragma once

#include "AsyncClientConnection.h"
#include <QDebug>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

class SignUp : public QWidget {
  Q_OBJECT
public:
  explicit SignUp(AsyncClientConnection *client, QWidget *parent = nullptr);

signals:
  void signInRequested();

  void signUpSuccess(const QString &username);

private slots:
  void onSignUpClicked();
  void onSwitchToSignIn();
  void onConnected();
  void onError(const QString &err);
  void onBufferSent();
  void onMessageReceived(const QString &msg);

private:
  AsyncClientConnection *client_;
  QLineEdit *usernameInput_;
  QLineEdit *passwordInput_;
  QLineEdit *confirmPasswordInput_;
  QPushButton *signUpButton_;
  QPushButton *toSignInButton_;
  QLabel *statusLabel_;
  bool socketIsConnected_{false};
};
