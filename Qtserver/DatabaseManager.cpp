

#include <Qtsql>
#include"DatabaseManager.h"


QJsonObject DatabaseManager::getJsonDataFromDatabase()
{

    QJsonObject resultObject;

    // Query data from database
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    if (query.exec("SELECT * FROM department")) {
        QJsonArray jsonArray;
        while (query.next()) {
            QJsonObject jsonObject;
            jsonObject["id"] = query.value("id").toInt();
            jsonObject["name"] = query.value("name").toString();
            jsonObject["address"] = query.value("address").toString();
            // Add more fields as needed

            jsonArray.append(jsonObject);
        }

        resultObject["data"] = jsonArray;
    }
    else {
        resultObject["error"] = "Database query failed";
    }

    return resultObject;
    
}

bool DatabaseManager::insertIntoDatabase(const QJsonObject& jsonData) {
    // Connect to database
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "Database is not open!";
        return false;
    }

    // Prepare SQL query
    QSqlQuery query(db);
    query.prepare("INSERT INTO your_table (field1, field2) VALUES (:value1, :value2)");

    // Bind values to placeholders
    query.bindValue(":value1", jsonData.value("field1").toString());
    query.bindValue(":value2", jsonData.value("field2").toInt());

    // Execute query
    if (!query.exec()) {
        qDebug() << "Query execution failed:" << query.lastError().text();
        return false;
    }

    return true;
}
