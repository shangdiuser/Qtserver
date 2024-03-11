#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QtSql>

class DatabaseManager {
public:
    static QSqlDatabase& getDatabase() {
        static QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
        static bool initialized = false;

        if (!initialized) {
            initialized = true;
            db.setDatabaseName("sm");
            db.setUserName("root");
            db.setPassword("123456");
            if (!db.open()) {
                qDebug() << "Failed to connect to database!";
            }
        }

        return db;
    }

    static  QJsonObject getJsonDataFromDatabase();
    static  bool insertIntoDatabase(const QJsonObject& jsonData);

};
#endif // DATABASEMANAGER_H