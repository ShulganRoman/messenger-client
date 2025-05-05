#include "MainWindow.h"
#include "AsyncClientConnection.h"
#include "ChatWindow.h"
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

MainWindow::MainWindow(AsyncClientConnection *client, QWidget *parent)
    : QMainWindow(parent), client_(client) {
  setWindowTitle(u8"Messenger — Поиск пользователей");
  resize(500, 600);

  auto *central = new QWidget;
  auto *layout = new QVBoxLayout(central);

  searchBar_ = new QLineEdit;
  searchBar_->setPlaceholderText(u8"Имя пользователя…");
  searchBtn_ = new QPushButton(u8"Найти");

  auto *searchRow = new QHBoxLayout;
  searchRow->addWidget(searchBar_);
  searchRow->addWidget(searchBtn_);

  results_ = new QListWidget;
  results_->setVisible(false);
  results_->setStyleSheet(
      "QListWidget { border: none; }"
      "QListWidget::item { padding: 8px; }"
      "QListWidget::item:hover    { background: #e6f2ff; }"
      "QListWidget::item:selected { background: #cce0ff; }");

  layout->addWidget(new QLabel(u8"Поиск пользователей"));
  layout->addLayout(searchRow);
  layout->addWidget(results_);
  setCentralWidget(central);

  connect(searchBtn_, &QPushButton::clicked, this,
          &MainWindow::onSearchButtonClicked);
  connect(searchBar_, &QLineEdit::returnPressed, this,
          &MainWindow::onSearchButtonClicked);
  connect(results_, &QListWidget::itemClicked, this,
          &MainWindow::onUserClicked);
  connect(client_, &AsyncClientConnection::messageReceived, this,
          &MainWindow::onServerMessage);
}

void MainWindow::onSearchButtonClicked() {
  const QString username = searchBar_->text().trimmed();
  if (username.isEmpty()) {
    results_->clear();
    results_->setVisible(false);
    return;
  }

  QJsonObject meta{
      {"entity", "find_users"}, {"username", username}, {"self", self_}};
  QJsonObject packet{{"metadata", meta}};

  client_->sendBuffer(QJsonDocument(packet)
                          .toJson(QJsonDocument::Compact)
                          .append('\n')
                          .toStdString());
}

void MainWindow::onServerMessage(const QString &msg) {
  QJsonParseError err;
  const auto doc = QJsonDocument::fromJson(msg.toUtf8(), &err);
  if (err.error != QJsonParseError::NoError || !doc.isObject())
    return;

  const auto obj = doc.object();
  if (obj.isEmpty())
    return;

  results_->clear();
  for (auto it = obj.begin(); it != obj.end(); ++it) {
    const QString name = it.key();
    if (name == self_)
      continue;

    auto *item = new QListWidgetItem(u8"👤 " + name, results_);
    item->setData(Qt::UserRole, name);
  }
  results_->setVisible(results_->count() > 0);
}

void MainWindow::onUserClicked(QListWidgetItem *item) {
  const QString username = item->data(Qt::UserRole).toString();
  auto *chat = new ChatWindow(self_, username, client_);
  chat->setAttribute(Qt::WA_DeleteOnClose);
  chat->show();
}
