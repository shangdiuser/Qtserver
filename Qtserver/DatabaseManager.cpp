

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
            isLate = 0; //������
            qDebug() << isLate ;
        }
        else 
        {
            isLate = 1;//�ٵ�
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
        return QByteArray(); // ���ؿյ� QByteArray ��ʾ����
    }

    // ִ�в�ѯ
    QSqlQuery query;
    query.prepare("SELECT attendance_records.*, employees.* "
        "FROM attendance_records "
        "JOIN employees ON attendance_records.employee_id = employees.employee_id "
        "WHERE attendance_records.employee_id = :employeeId "
        "ORDER BY attendance_records.id DESC LIMIT 1");
    query.bindValue(":employeeId", employeeId);

  

    if (!query.exec()) {
        qDebug() << "Query error:" << query.lastError().text();
        return QByteArray(); // ���ؿյ� QByteArray ��ʾ����
    }

    QByteArray jsonData;
    QJsonObject jsonObject;
    // �����ѯ���
    if (query.next()) {
        // ���� JSON ����
        
        jsonObject["employee_id"] = query.value("employee_id").toString();
        jsonObject["punch_in_time"] = query.value("punch_in_time").toString();
        jsonObject["punch_out_time"] = query.value("punch_out_time").toString();
       // jsonObject["is_late"] = query.value("is_late").toString();
        jsonObject["is_absent"] = query.value("is_absent").toString();
        jsonObject["fine_amount"] = query.value("fine_amount").toString();
        jsonObject["name"] = query.value("name").toString(); // ���Ա������
        jsonObject["position"] = query.value("position").toString(); //职位
        jsonObject["department"] = query.value("department").toString(); // ��Ӳ�����Ϣ

        if (1 == query.value("is_absent").toInt()) {
            jsonObject["is_late"] = "迟到";
        }
        else
        {
            jsonObject["is_late"] = "正常";
        }


       
    }
    else {
        qDebug() << "No records found for employee_id =" << employeeId;
        jsonObject["is_late"] = "失败，重新录入";
    }
    // ת��Ϊ JSON �ĵ�
    QJsonDocument jsonDoc(jsonObject);
    jsonData = jsonDoc.toJson();
    return jsonData;
}

QByteArray DatabaseManager::allInfo(QString id, QString name)
{
    // Connect to database
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "Database is not open!";
        return QByteArray(); // ���ؿյ� QByteArray ��ʾ����
    }

    // ִ�в�ѯ
    QSqlQuery query;
     //id = "1002";
    // name = "张三";
    /*SELECT e.*, ar.*
FROM employees e
LEFT JOIN attendance_records ar ON e.employee_id = ar.employee_id
WHERE e.employee_id LIKE '%1002%' AND e.name LIKE '%李四%';
*/
    QString queryText = "SELECT employees.*, attendance_records.* FROM employees  LEFT JOIN attendance_records  ON employees.employee_id = attendance_records.employee_id";

    if (!id.isEmpty() || !name.isEmpty()) {
        // 如果有查询条件，根据工号或姓名筛选
        queryText += " WHERE employees.employee_id LIKE '%" + id + "%' AND employees.name LIKE '%" +  + "%'";
       
    }

    // 执行查询
    //QSqlQuery query(queryText);
    query.prepare(queryText);
  //  query.bindValue(":employeeId", id);
    qDebug() << "查询执行!";

    if (!query.exec()) {
        qDebug() << "Failed to execute query!";
        return "查询失败";
    }

    // 创建JSON数组
    QJsonArray jsonArray;
    QByteArray jsonData;
    // 将查询结果添加到JSON数组中
    while (query.next()) {
        QJsonObject jsonObject;
        jsonObject["employee_id"] = query.value("employee_id").toString();
        jsonObject["punch_in_time"] = query.value("punch_in_time").toString();
        jsonObject["punch_out_time"] = query.value("punch_out_time").toString();
        // jsonObject["is_late"] = query.value("is_late").toString();
        jsonObject["is_absent"] = query.value("is_absent").toString();
        jsonObject["fine_amount"] = query.value("fine_amount").toString();
        jsonObject["name"] = query.value("name").toString(); // ���Ա������
        jsonObject["position"] = query.value("position").toString(); //职位
        jsonObject["department"] = query.value("department").toString(); // ��Ӳ�����Ϣ

     //   qDebug() << query.value("department").toString();

        if (1 == query.value("is_absent").toInt()) {
            jsonObject["is_late"] = "迟到";
        }
        else
        {
            jsonObject["is_late"] = "正常";
        }
        jsonArray.append(jsonObject);
    }

    // 将JSON数组转换为JSON文档
    QJsonDocument jsonDocument(jsonArray);

    // 将JSON文档转换为字符串
    QString jsonString = jsonDocument.toJson(QJsonDocument::Indented);
    qDebug().noquote() << jsonString;

    // ת��Ϊ JSON �ĵ�
    QJsonDocument jsonDoc(jsonArray);
    jsonData = jsonDoc.toJson();

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

    // ִ�в�ѯ
    QSqlQuery query;
    query.prepare("SELECT * FROM attendance_records WHERE employee_id = :employeeId ORDER BY id DESC LIMIT 1");
    query.bindValue(":employeeId", employeeId);

    if (!query.exec()) {
        qDebug() << "Query error:" << query.lastError().text();
       
    }
    QByteArray jsonData = nullptr;
    // �����ѯ���
    if (query.next()) {

        // ���� JSON ����
        QJsonObject jsonObject;
        jsonObject["employee_id"] = query.value("employee_id").toString();
        jsonObject["punch_in_time"] = query.value("punch_in_time").toString();
        jsonObject["punch_out_time"] = query.value("punch_out_time").toString();
        jsonObject["is_late"] = query.value("is_late").toString();
        jsonObject["is_absent"] = query.value("is_absent").toString();
        jsonObject["fine_amount"] = query.value("fine_amount").toString();

        qDebug() << employeeId << "QJsonObject employeeId";

        // ת��Ϊ JSON �ĵ�
        QJsonDocument jsonDoc(jsonObject);
         jsonData = jsonDoc.toJson();

    }
    else {
        qDebug() << "No records found for employee_id = 2";
    }




    return jsonData;
}
*/