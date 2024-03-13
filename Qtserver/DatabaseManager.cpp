

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
    query.prepare("INSERT INTO attendance_records (employee_id, punch_in_time, punch_out_time, is_late, is_absent, fine_amount) VALUES (:value1, :value2, :value3, :value4, :value5, :value6)");

    // Check if the required fields are present and have valid types
    if (jsonData.contains("employee_id") && jsonData["employee_id"].isString() &&
        jsonData.contains("punch_in_time") && jsonData["punch_in_time"].isString() &&
        jsonData.contains("punch_out_time") && jsonData["punch_out_time"].isString()) {

        QString employeeId = jsonData["employee_id"].toString();
        QString punchInTimeString = jsonData["punch_in_time"].toString();
        QString punchOutTimeString = jsonData["punch_out_time"].toString();

        // Convert punch in and punch out times to QDateTime objects
        QDateTime punchInTime = QDateTime::fromString(punchInTimeString, "yyyy-MM-dd HH:mm:ss");
        QDateTime punchOutTime = QDateTime::fromString(punchOutTimeString, "yyyy-MM-dd HH:mm:ss");

        // Check if punch in time is later than 9:00 AM
        int isLate = 3;
        bool Late = punchInTime.time() > QTime(9, 0);
    
        if (Late == true)
        {
            isLate = 0; //正常打卡
            qDebug() << isLate ;
        }
        else 
        {
            isLate = 1;//迟到
            qDebug() << isLate ;
        }


        // Bind values to placeholders
        query.bindValue(":value1", employeeId);
        query.bindValue(":value2", punchInTimeString);
        query.bindValue(":value3", punchOutTimeString);
        query.bindValue(":value4", isLate);
        query.bindValue(":value5", false); // Assuming is_absent is always false for this case
        query.bindValue(":value6", 0.0); // Assuming fine_amount is always 0.0 for this case

        // Execute query
        if (!query.exec()) {
            qDebug() << "Query execution failed:" << query.lastError().text();
            return false;
        }

        return true;
    }
    else {
        qDebug() << "Required fields not found or have invalid types";
        return false;
    }
}




/*
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
    if (jsonData.contains("employee_id") && jsonData["employee_id"].isString()) {
        QString employeeId = jsonData["employee_id"].toString();
        qDebug() << "Employee ID:" << employeeId;
        // Bind values to placeholders
  // query.bindValue(":value1", jsonData.value("id").toInt());
        query.bindValue(":value2", jsonData["employee_id"].toString());
      
        qDebug() << jsonData["employee_id"].toString() << "99999999999999999";
        query.bindValue(":value3", jsonData["punch_in_time"].toString());
        query.bindValue(":value4", jsonData["punch_out_time"].toString());
        query.bindValue(":value5", jsonData["is_late"].toString());
        query.bindValue(":value6", jsonData["is_absent"].toString());
        query.bindValue(":value7", jsonData["fine_amount"].toString());

    }
    else {
        qDebug() << "employee_id field not found or not a string";
    }
   

    // Execute query
    if (!query.exec()) {
        qDebug() << "Query execution failed:" << query.lastError().text();
        return false;
    }

    return true;
}

*/

QByteArray DatabaseManager::ClockInfo(const QString& employeeId)
{
    // Connect to database
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "Database is not open!";
        return QByteArray(); // 返回空的 QByteArray 表示出错
    }

    // 执行查询
    QSqlQuery query;
    query.prepare("SELECT attendance_records.*, employees.name, employees.department "
        "FROM attendance_records "
        "JOIN employees ON attendance_records.employee_id = :employeeId "
        "WHERE attendance_records.employee_id = :employeeId "
        "ORDER BY attendance_records.id DESC LIMIT 1");
    query.bindValue(":employeeId", employeeId);

    if (!query.exec()) {
        qDebug() << "Query error:" << query.lastError().text();
        return QByteArray(); // 返回空的 QByteArray 表示出错
    }

    QByteArray jsonData;
    // 处理查询结果
    if (query.next()) {
        // 构造 JSON 数据
        QJsonObject jsonObject;
        jsonObject["employee_id"] = query.value("employee_id").toString();
        jsonObject["punch_in_time"] = query.value("punch_in_time").toString();
        jsonObject["punch_out_time"] = query.value("punch_out_time").toString();
        jsonObject["is_late"] = query.value("is_late").toString();
        jsonObject["is_absent"] = query.value("is_absent").toString();
        jsonObject["fine_amount"] = query.value("fine_amount").toString();
        jsonObject["name"] = query.value("name").toString(); // 添加员工姓名
        jsonObject["department"] = query.value("department").toString(); // 添加部门信息

        // 转换为 JSON 文档
        QJsonDocument jsonDoc(jsonObject);
        jsonData = jsonDoc.toJson();
    }
    else {
        qDebug() << "No records found for employee_id =" << employeeId;
    }

    return jsonData;
}


/*
QByteArray DatabaseManager::ClockInfo(const QString& employeeId)
{
    // Connect to database
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "Database is not open!";
       
    }

    // 执行查询
    QSqlQuery query;
    query.prepare("SELECT * FROM attendance_records WHERE employee_id = :employeeId ORDER BY id DESC LIMIT 1");
    query.bindValue(":employeeId", employeeId);

    if (!query.exec()) {
        qDebug() << "Query error:" << query.lastError().text();
       
    }
    QByteArray jsonData = nullptr;
    // 处理查询结果
    if (query.next()) {

        // 构造 JSON 数据
        QJsonObject jsonObject;
        jsonObject["employee_id"] = query.value("employee_id").toString();
        jsonObject["punch_in_time"] = query.value("punch_in_time").toString();
        jsonObject["punch_out_time"] = query.value("punch_out_time").toString();
        jsonObject["is_late"] = query.value("is_late").toString();
        jsonObject["is_absent"] = query.value("is_absent").toString();
        jsonObject["fine_amount"] = query.value("fine_amount").toString();

        qDebug() << employeeId << "QJsonObject employeeId";

        // 转换为 JSON 文档
        QJsonDocument jsonDoc(jsonObject);
         jsonData = jsonDoc.toJson();

    }
    else {
        qDebug() << "No records found for employee_id = 2";
    }




    return jsonData;
}
*/