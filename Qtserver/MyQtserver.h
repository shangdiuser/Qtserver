#pragma once

#include <QtNetwork>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include"DatabaseManager.h"
#include "DatabaseManager.h"
class MyQtserver : public QTcpServer
{
    Q_OBJECT

public:
    explicit MyQtserver(QObject* parent = nullptr) : QTcpServer(parent) {}

protected:
    void incomingConnection(qintptr socketDescriptor) override {
        QTcpSocket* socket = new QTcpSocket(this);
        connect(socket, &QTcpSocket::readyRead, this, &MyQtserver::readRequest);
        socket->setSocketDescriptor(socketDescriptor);
    }

private slots:
    void readRequest() {
        QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
        if (!socket)
            return;

        QByteArray requestData = socket->readAll();

        // Parse request URL and parameters
        QString request = QString::fromUtf8(requestData);
        QUrl url = QUrl(request.split(' ')[1]);
        QString path = url.path();
        QString query = url.query();

        QByteArray responseData;
        if (path == "/hello" && query == "user=admin") {
            // Get JSON data from database
            QJsonObject jsonData = DatabaseManager::getJsonDataFromDatabase();

            // Convert JSON data to QByteArray
            QJsonDocument jsonDoc(jsonData);
             responseData = jsonDoc.toJson();
          
            
        }
        else if (path == "/hello" && query == "user=user") {
            responseData = "{\"message\": \"Hello user!\"}";
        }
        //´ò¿¨
        else if (path == "/Clock") {
            // Parse JSON data from request
            QJsonDocument jsonDoc = QJsonDocument::fromJson(requestData);
            QJsonObject jsonObject = jsonDoc.object();

            // Insert data into database
            if (DatabaseManager::insertIntoDatabase(jsonObject)) {
                socket->write("HTTP/1.1 200 OK\r\n\r\n");
                socket->write("Data inserted into database successfully!");
            }
            else {
                socket->write("HTTP/1.1 500 Internal Server Error\r\n\r\n");
                socket->write("Failed to insert data into database!");
            }


            responseData = "{\"message\": \"Hello user!\"}";
        }else {
            responseData = "{\"error\": \"Invalid request!\"}";
        }

        // Write HTTP response
        socket->write("HTTP/1.1 200 OK\r\n");
        socket->write("Content-Type: application/json\r\n");
        socket->write("Content-Length: " + QByteArray::number(responseData.size()) + "\r\n\r\n");
        socket->write(responseData);

        socket->disconnectFromHost();
    }
};
