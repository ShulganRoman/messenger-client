#pragma once

#include <QObject>
#include <boost/asio.hpp>
#include <memory>
#include <thread>

class AsyncClientConnection : public QObject {
  Q_OBJECT

public:
  explicit AsyncClientConnection(QObject *parent = nullptr);
  void connectToServer();
  void sendBuffer(const std::string &buffer);

signals:
  void connected();
  void errorOccurred(const QString &error);
  void bufferSent();
  void messageReceived(const QString &message);

private:
  void handleConnect(const boost::system::error_code &ec);
  void handleWrite(const boost::system::error_code &ec,
                   std::size_t bytesTransferred);
  void startRead();
  void handleRead(const boost::system::error_code &ec,
                  std::size_t bytesTransferred);

  boost::asio::io_context ioContext_;
  boost::asio::ip::tcp::socket socket_;
  boost::asio::streambuf readBuffer_;
  std::unique_ptr<std::thread> ioThread_;
};
