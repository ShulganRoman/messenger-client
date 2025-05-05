
#include "AsyncClientConnection.h"
#include <QDebug>
#include <boost/asio/write.hpp>
using boost::asio::ip::tcp;

AsyncClientConnection::AsyncClientConnection(QObject *parent)
    : QObject(parent), socket_(ioContext_) {}

void AsyncClientConnection::connectToServer() {
  qDebug() << "[AsyncClient] Попытка соединиться с 127.0.0.1:4444";
  tcp::endpoint endpoint(boost::asio::ip::make_address("127.0.0.1"), 4444);
  socket_.async_connect(endpoint, [this](const boost::system::error_code &ec) {
    handleConnect(ec);
  });

  ioThread_ = std::make_unique<std::thread>([this]() {
    try {
      ioContext_.run();
    } catch (const std::exception &e) {
      emit errorOccurred(QString("IO error: %1").arg(e.what()));
    }
  });
}

void AsyncClientConnection::handleConnect(const boost::system::error_code &ec) {
  if (!ec) {
    qDebug() << "[AsyncClient] Успешно подключились";
    emit connected();
    startRead();
  } else {
    qDebug() << "[AsyncClient] Ошибка подключения:" << ec.message().c_str();
    emit errorOccurred(
        QString("Connection failed: %1").arg(ec.message().c_str()));
  }
}

void AsyncClientConnection::sendBuffer(const std::string &buffer) {
  if (!socket_.is_open()) {
    emit errorOccurred("Socket is not open");
    return;
  }
  auto writeBuffer = std::make_shared<std::string>(buffer);
  boost::asio::async_write(
      socket_, boost::asio::buffer(*writeBuffer),
      [this, writeBuffer](const boost::system::error_code &ec,
                          std::size_t bytesSent) {
        handleWrite(ec, bytesSent);
      });
}

void AsyncClientConnection::handleWrite(const boost::system::error_code &ec,
                                        std::size_t) {
  if (!ec) {
    emit bufferSent();
  } else {
    emit errorOccurred(QString("Send failed: %1").arg(ec.message().c_str()));
  }
}

void AsyncClientConnection::startRead() {
  boost::asio::async_read_until(
      socket_, readBuffer_, '\n',
      [this](const boost::system::error_code &ec, std::size_t bytes) {
        handleRead(ec, bytes);
      });
}

void AsyncClientConnection::handleRead(const boost::system::error_code &ec,
                                       std::size_t) {
  if (!ec) {
    std::istream is(&readBuffer_);
    std::string line;
    std::getline(is, line);
    qDebug() << "[AsyncClient] Получено:" << QString::fromStdString(line);
    emit messageReceived(QString::fromStdString(line));
    startRead();
  } else {
    emit errorOccurred(QString("Read failed: %1").arg(ec.message().c_str()));
  }
}
