

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
    query.prepare("INSERT INTO attendance_records (id, employee_id,punch_in_time,punch_out_time,is_late,is_absent,fine_amount) VALUES (:value1, :value2, :value3, :value4, :value5, :value6, :value7)");

    //INSERT INTO `sm`.`attendance_records` (`id`, `employee_id`, `punch_in_time`, `punch_out_time`) 
    // VALUES ('2', '2', '2024-03-11 19:22:07', '2024-03-11 19:22:11'); 
    // 
    // Bind values to placeholders
   // query.bindValue(":value1", jsonData.value("id").toInt());
    query.bindValue(":value2", jsonData.value("employee_id").toInt());
    qDebug() << jsonData.value("id").toInt() << "8888888888";
    qDebug() << jsonData.value("employee_id").toInt()<<"99999999999999999";
    query.bindValue(":value3", jsonData.value("punch_in_time").toString());
    query.bindValue(":value4", jsonData.value("punch_out_time").toString());
    query.bindValue(":value5", jsonData.value("is_late").toString());
    query.bindValue(":value6", jsonData.value("is_absent").toString());
    query.bindValue(":value7", jsonData.value("fine_amount").toString());

    // Execute query
    if (!query.exec()) {
        qDebug() << "Query execution failed:" << query.lastError().text();
        return false;
    }

    return true;
}
