#include "Qtserver.h"
#include <QtWidgets/QApplication>
#include "MyQtserver.h"
#include"DatabaseManager.h"


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


// Example usage:
void fetchDataFromDatabase() {
    QSqlDatabase& db = DatabaseManager::getDatabase();
    QSqlQuery query(db);

    if (query.exec("SELECT * FROM department")) {
        while (query.next()) {
            // Process query results
            qDebug() << query.value("id").toInt() << query.value("name").toString() << query.value("address").toString();
        }
    }
    else {
        qDebug() << "Query failed:" << query.lastError().text();
    }
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Qtserver w;
    w.show();

    MyQtserver server;
    if (!server.listen(QHostAddress::Any, 8080)) {
        qDebug() << "Server could not start!";
        return -1;
    }
    qDebug() << "Server started on port 8080.";
   

    // Example usage
    fetchDataFromDatabase();

    return a.exec();
}
