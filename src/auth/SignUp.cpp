#include "SignUp.h"
#include <QVBoxLayout>
#include <boost/json.hpp>

SignUp::SignUp(AsyncClientConnection *client, QWidget *parent)
    : QWidget(parent), client_(client) {
  setWindowTitle("Регистрация");
  auto *layout = new QVBoxLayout(this);

  layout->addWidget(new QLabel("Введите имя пользователя и пароль:"));

  usernameInput_ = new QLineEdit;
  usernameInput_->setPlaceholderText("Имя пользователя");
  layout->addWidget(usernameInput_);

  passwordInput_ = new QLineEdit;
  passwordInput_->setPlaceholderText("Пароль");
  passwordInput_->setEchoMode(QLineEdit::Password);
  layout->addWidget(passwordInput_);

  confirmPasswordInput_ = new QLineEdit;
  confirmPasswordInput_->setPlaceholderText("Повторите пароль");
  confirmPasswordInput_->setEchoMode(QLineEdit::Password);
  layout->addWidget(confirmPasswordInput_);

  signUpButton_ = new QPushButton("Зарегистрироваться");
  toSignInButton_ = new QPushButton("Войти");
  statusLabel_ = new QLabel;
  layout->addWidget(signUpButton_);
  layout->addWidget(toSignInButton_);
  layout->addWidget(statusLabel_);

  connect(signUpButton_, &QPushButton::clicked, this, &SignUp::onSignUpClicked);
  connect(toSignInButton_, &QPushButton::clicked, this,
          &SignUp::onSwitchToSignIn);
  connect(client_, &AsyncClientConnection::connected, this,
          &SignUp::onConnected);
  connect(client_, &AsyncClientConnection::errorOccurred, this,
          &SignUp::onError);
  connect(client_, &AsyncClientConnection::bufferSent, this,
          &SignUp::onBufferSent);
  connect(client_, &AsyncClientConnection::messageReceived, this,
          &SignUp::onMessageReceived);
}

void SignUp::onSignUpClicked() {
  qDebug() << "[SignUp] onSignUpClicked()";
  if (!socketIsConnected_) {
    statusLabel_->setText("Сервер ещё не доступен, подождите...");
    return;
  }
  if (passwordInput_->text() != confirmPasswordInput_->text()) {
    statusLabel_->setText("Пароли не совпадают");
    return;
  }

  boost::json::object root;
  root["metadata"] = {{"entity", "user"}, {"id", 0}};
  root["mode"] = 0;
  boost::json::object modify;
  modify["username"] = usernameInput_->text().toStdString();
  modify["password"] = passwordInput_->text().toStdString();
  root["modify"] = modify;

  std::string data = boost::json::serialize(root) + "\n";
  qDebug() << "[SignUp] Sending:" << QString::fromStdString(data);
  client_->sendBuffer(data);
  statusLabel_->setText("Запрос на регистрацию отправлен...");
}

void SignUp::onSwitchToSignIn() { emit signInRequested(); }

void SignUp::onConnected() {
  qDebug() << "[SignUp] connected()";
  socketIsConnected_ = true;
  statusLabel_->setText("Соединение установлено");
}

void SignUp::onError(const QString &err) {
  qDebug() << "[SignUp] errorOccurred:" << err;
  statusLabel_->setText("Ошибка: " + err);
}

void SignUp::onBufferSent() {
  qDebug() << "[SignUp] bufferSent()";
  statusLabel_->setText("Запрос отправлен, ждём ответ...");
}

void SignUp::onMessageReceived(const QString &msg) {
  const QString m = msg.trimmed();
  qDebug() << "[SignUp] messageReceived:" << m;

  if (m == "user_created") {
    statusLabel_->setText(
        "Пользователь создан! Переходим на главную страницу...");

    emit signUpSuccess(usernameInput_->text());
  } else if (m == "username_taken") {
    statusLabel_->setText("Пользователь с таким именем уже существует");
  } else {
    statusLabel_->setText("Неизвестный ответ сервера: " + m);
  }
}
