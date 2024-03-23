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
        DatabaseManager manager;
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
        //��

        else if (path == "/Clock") {
            qDebug() << requestData << "JrequestData";
            // ���� JSON ���ݵ���ʼλ��
            int jsonDataStart = requestData.indexOf("{");
            if (jsonDataStart != -1) {
                QByteArray jsonData = requestData.mid(jsonDataStart);

                // ���� JSON ����
                QJsonParseError error;
                QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &error);

                if (error.error == QJsonParseError::NoError && jsonDoc.isObject()) {
                    QJsonObject jsonObj = jsonDoc.object();
                    if (DatabaseManager::insertIntoDatabase(jsonObj)) {
                        socket->write("HTTP/1.1 200 OK\r\n\r\n");
                        socket->write("Data inserted into database successfully!");
                       
                        if (jsonData.contains("employee_id") && jsonObj["employee_id"].isString()) {
                            QString employeeId = jsonObj["employee_id"].toString();
                            qDebug() << "Employee ID:" << employeeId;

                          //  QByteArray InfoDate = manager.ClockInfo(employeeId);
                            socket->write(manager.ClockInfo(employeeId));                          
                        }
                    }
                    else {
                        socket->write("HTTP/1.1 500 Internal Server Error\r\n\r\n");
                        socket->write("Failed to insert data into database!");
                    }
              }
                else {
                    qDebug() << "JSON data not found in the request";
                }
            }
        }

        else if (path == "/allInof") {
           
            // 解析请求中的参数
            // 解析请求中的参数
            QString requestString(requestData);
            QString queryString = requestString.split(" ")[1].split("?").last();
            QUrlQuery query(queryString);

            QString idValue = query.queryItemValue("id");
            QString nameValue = query.queryItemValue("name");

            qDebug() << "ID:" << idValue;
            qDebug() << "Name:" << nameValue;

             responseData = manager.allInfo(idValue, nameValue);
            qDebug() << responseData << "responseData";
           // socket->write(responseData);
          
            // 返回响应
           // responseData = "{\"message\": \"Hello all!\"}";
            socket->write("HTTP/1.1 200 OK\r\n");
            socket->write("Content-Type: application/json\r\n");
            socket->write("Content-Length: " + QByteArray::number(responseData.size()) + "\r\n\r\n");
            socket->write(responseData);
            qDebug() << "responseData:" << responseData;

          //  socket->waitForBytesWritten();
           // socket->disconnectFromHost();

        }
        // Write HTTP response
        socket->write("HTTP/1.1 200 OK\r\n");
        socket->write("Content-Type: application/json\r\n");
        socket->write("Content-Length: " + QByteArray::number(responseData.size()) + "\r\n\r\n");
        socket->write(responseData);

        socket->disconnectFromHost();
    }


};