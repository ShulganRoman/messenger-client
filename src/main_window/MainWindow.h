#pragma once
#include <QMainWindow>

class QLineEdit;
class QPushButton;
class QListWidget;
class QListWidgetItem;
class AsyncClientConnection;

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  explicit MainWindow(AsyncClientConnection *client, QWidget *parent = nullptr);
  void setSelf(const QString &username) { self_ = username; }

private slots:
  void onSearchButtonClicked();
  void onServerMessage(const QString &msg);
  void onUserClicked(QListWidgetItem *item);

private:
  AsyncClientConnection *client_;
  QString self_;
  QLineEdit *searchBar_;
  QPushButton *searchBtn_;
  QListWidget *results_;
};
