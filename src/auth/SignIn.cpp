#include "SignIn.h"
#include <QDebug>
#include <QVBoxLayout>

SignIn::SignIn(AsyncClientConnection *client, QWidget *parent)
    : QWidget(parent), client_(client) {
  setWindowTitle("Вход");
  auto *layout = new QVBoxLayout(this);
  layout->addWidget(new QLabel("Введите логин и пароль:"));

  loginInput_ = new QLineEdit;
  loginInput_->setPlaceholderText("Логин");
  layout->addWidget(loginInput_);

  passwordInput_ = new QLineEdit;
  passwordInput_->setPlaceholderText("Пароль");
  passwordInput_->setEchoMode(QLineEdit::Password);
  layout->addWidget(passwordInput_);

  signInButton_ = new QPushButton("Войти");
  toSignUpButton_ = new QPushButton("Регистрация");
  statusLabel_ = new QLabel;
  layout->addWidget(signInButton_);
  layout->addWidget(toSignUpButton_);
  layout->addWidget(statusLabel_);

  connect(signInButton_, &QPushButton::clicked, this, &SignIn::onSignInClicked);
  connect(toSignUpButton_, &QPushButton::clicked, this,
          &SignIn::onSwitchToSignUp);
  connect(client_, &AsyncClientConnection::connected, this,
          &SignIn::onConnected);
  connect(client_, &AsyncClientConnection::errorOccurred, this,
          &SignIn::onError);
  connect(client_, &AsyncClientConnection::bufferSent, this,
          &SignIn::onBufferSent);
  connect(client_, &AsyncClientConnection::messageReceived, this,
          &SignIn::onMessageReceived);
}

void SignIn::onSignInClicked() {
  qDebug() << "[SignIn] onSignInClicked()";
  if (!socketIsConnected_) {
    statusLabel_->setText("Сервер ещё не доступен, подождите...");
    return;
  }

  boost::json::object root;
  root["metadata"] = {{"entity", "user"}, {"id", 0}};
  root["mode"] = 1;
  boost::json::object modify;
  modify["username"] = loginInput_->text().toStdString();
  modify["password"] = passwordInput_->text().toStdString();
  root["modify"] = modify;

  std::string data = boost::json::serialize(root) + "\n";
  qDebug() << "[SignIn] Sending:" << QString::fromStdString(data);
  client_->sendBuffer(data);
  statusLabel_->setText("Запрос на вход отправлен...");
}

void SignIn::onSwitchToSignUp() { emit signUpRequested(); }

void SignIn::onConnected() {
  qDebug() << "[SignIn] connected()";
  socketIsConnected_ = true;
  statusLabel_->setText("Соединение установлено");
}

void SignIn::onError(const QString &err) {
  qDebug() << "[SignIn] errorOccurred:" << err;
  statusLabel_->setText("Ошибка: " + err);
}

void SignIn::onBufferSent() {
  qDebug() << "[SignIn] bufferSent()";
  statusLabel_->setText("Запрос отправлен, ждём ответ...");
}

void SignIn::onMessageReceived(const QString &msg) {
  const QString m = msg.trimmed();
  qDebug() << "[SignIn] messageReceived:" << m;

  if (m == "user_enter_success") {
    statusLabel_->setText("Вход выполнен успешно");

    emit signInSuccess(loginInput_->text());
  } else if (m == "incorrect_password_or_username") {
    statusLabel_->setText("Неверный логин или пароль");
  } else {
    statusLabel_->setText("Неизвестный ответ сервера: " + m);
  }
}
