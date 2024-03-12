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
        //打卡

        else if (path == "/Clock") {

            // 查找 JSON 数据的起始位置
            int jsonStartIndex = requestData.indexOf("{");
            if (jsonStartIndex == -1) {
                qDebug() << "JSON data not found in request.";

            }

            // 提取 JSON 数据部分
            QByteArray jsonData = requestData.mid(jsonStartIndex);

            // 解析 JSON 数据
            QJsonParseError error;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &error);
            if (error.error != QJsonParseError::NoError) {
                qDebug() << "JSON parse error:" << error.errorString();

            }

            if (!jsonDoc.isObject()) {
                qDebug() << "Received data is not a JSON object.";

            }

            QJsonObject jsonObject = jsonDoc.object();

            // 获取 employee_id 值
            int employeeId = jsonObject.value("employee_id").toInt();
            qDebug() << "Employee ID:" << employeeId;


            if (DatabaseManager::insertIntoDatabase(jsonObject)) {
                socket->write("HTTP/1.1 200 OK\r\n\r\n");
                socket->write("Data inserted into database successfully!");
            }
            else {
                socket->write("HTTP/1.1 500 Internal Server Error\r\n\r\n");
                socket->write("Failed to insert data into database!");
            }
        }
        // Parse JSON data from request
        QJsonDocument jsonDoc = QJsonDocument::fromJson(requestData);
        QJsonObject jsonObject = jsonDoc.object();
        qDebug() << "Received data:" << requestData;

            /*
            

             // 从字符串创建 JSON 文档
             QString jsonString = requestData;
             QJsonDocument jsonDoc1 = QJsonDocument::fromJson(jsonString.toUtf8());
             qDebug() << "jsonDoc Value of key1:" << jsonDoc1["id"].toInt();
             qDebug() << "jsonDoc Value of key2:" << jsonDoc1["employee_id"].toInt();


             // 输出 JSON 对象的值
             qDebug() << "toString Value of key1:" << jsonObject["id"].toInt();
             qDebug() << "toString Value of key2:" << jsonObject["employee_id"].toString();
             // 输出 JSON 对象的值
             int id = jsonObject["id"].toInt();

             qDebug() << "Value of key1:" << id;
             qDebug() << "Value of key2:" << jsonObject["employee_id"].toInt();

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
         */


         // Write HTTP response
            socket->write("HTTP/1.1 200 OK\r\n");
            socket->write("Content-Type: application/json\r\n");
            socket->write("Content-Length: " + QByteArray::number(responseData.size()) + "\r\n\r\n");
            socket->write(responseData);

            socket->disconnectFromHost();
        
    }
};