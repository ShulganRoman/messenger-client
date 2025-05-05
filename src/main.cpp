#include "AsyncClientConnection.h"
#include "MainWindow.h"
#include "SignIn.h"
#include "SignUp.h"
#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  auto *client = new AsyncClientConnection;

  auto *signIn = new SignIn(client);
  auto *signUp = new SignUp(client);
  auto *mainWin = new MainWindow(client);

  QObject::connect(signUp, &SignUp::signUpSuccess, [&](const QString &me) {
    signUp->hide();
    mainWin->setSelf(me);
    mainWin->show();
  });

  QObject::connect(signIn, &SignIn::signInSuccess, [&](const QString &me) {
    signIn->hide();
    mainWin->setSelf(me);
    mainWin->show();
  });

  QObject::connect(signIn, &SignIn::signUpRequested, [&]() {
    signIn->hide();
    signUp->show();
  });
  QObject::connect(signUp, &SignUp::signInRequested, [&]() {
    signUp->hide();
    signIn->show();
  });

  client->connectToServer();

  signIn->resize(400, 300);
  signIn->show();

  return app.exec();
}
