#include "print_task_data_management.h"

// 构造函数：初始化数据库
PrintTaskDataManagement::PrintTaskDataManagement() {
    initDatabase();
}

// 析构函数：释放资源
PrintTaskDataManagement::~PrintTaskDataManagement() {
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

// 初始化数据库，如果表不存在则创建
bool PrintTaskDataManagement::initDatabase() {
    QMutexLocker locker(&m_mutex); // 加锁
    QSqlDatabase db = createConnection();
    if (!db.isOpen()) return false;

    // 创建打印历史表
    QString createTableSql = R"(
        CREATE TABLE IF NOT EXISTS print_task_history (
            task_id TEXT PRIMARY KEY,
            content TEXT,
            task_type INTEGER,
            remark TEXT,
            timestamp TIMESTAMP default (datetime('now', 'localtime')));
    )";

    // 创建统计表
    QString createStatisticsTableSql = R"(
        CREATE TABLE IF NOT EXISTS print_task_statistics (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            date TEXT NOT NULL, -- 格式：YYYY-MM-DD
            count INTEGER DEFAULT 0,
            UNIQUE(date) -- 确保每天只有一条记录
        );
    )";

    QSqlQuery query(db);
    bool success = query.exec(createTableSql);
    if (!success) {
        qDebug() << "Error creating print_task_history table:" << query.lastError().text();
    }

    success &= query.exec(createStatisticsTableSql);
    if (!success) {
        qDebug() << "Error creating print_task_statistics table:" << query.lastError().text();
    }

    db.close();
    return success;
}

// 创建数据库连接
QSqlDatabase PrintTaskDataManagement::createConnection() {
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    static QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    {
        db.setDatabaseName(m_dbName);
        if (!db.open()) {
            qDebug() << "Error: Unable to open database" << db.lastError().text();
        }
        else
        {
            qDebug() << "connect Database successfully666666!";
        }

    }
    return db;
}

bool PrintTaskDataManagement::executeTransaction(const std::function<bool(QSqlQuery&)>& operation, int timeoutMs) {
    // 尝试获取锁，如果在timeoutMs内获取不到则放弃
    if (!m_mutex.tryLock(timeoutMs)) {
        return false; // 获取锁超时，放弃操作
    }
    // 手动管理锁的释放
    QSqlDatabase db = createConnection();
    if (!db.isOpen()) {
        m_mutex.unlock();
        return false;
    }

    if (!db.transaction()) {
        qDebug() << "Error: Unable to begin transaction";
        db.close();
        m_mutex.unlock();
        return false;
    }

    QSqlQuery query(db);
    if (!operation(query)) {
        qDebug() << "Error: Operation failed, rolling back transaction";
        db.rollback();
        db.close();
        m_mutex.unlock();
        return false;
    }

    if (!db.commit()) {
        qDebug() << "Error: Unable to commit transaction";
        db.close();
        m_mutex.unlock();
        return false;
    }

    db.close();
    m_mutex.unlock();
    return true;
}


// 插入打印历史记录
bool PrintTaskDataManagement::insertPrintTask(const QString& taskId, const QString& content, int taskType, const QString& remark) {
    return executeTransaction([&](QSqlQuery& query) {
        // 1. 插入打印历史记录
        QString insertSql = R"(
            INSERT INTO print_task_history (task_id, content, task_type, remark)
            VALUES (?, ?, ?, ?)
        )";
        query.prepare(insertSql);
        query.addBindValue(taskId);
        query.addBindValue(content);
        query.addBindValue(taskType);
        query.addBindValue(remark);

        qDebug() << "insertPrintTask ==" << insertSql;
        if (!query.exec()) {
            return false;
        }

        // 2. 更新统计表
        QString currentDate = QDateTime::currentDateTime().toString("yyyy-MM-dd");
        
        // 检查当天记录是否存在
        QString checkSql = "SELECT count FROM print_task_statistics WHERE date = ?";
        query.prepare(checkSql);
        query.addBindValue(currentDate);
        
        if (query.exec() && query.next()) {
            // 记录存在，更新计数+1
            int currentCount = query.value(0).toInt();
            QString updateSql = "UPDATE print_task_statistics SET count = ? WHERE date = ?";
            query.prepare(updateSql);
            query.addBindValue(currentCount + 1);
            query.addBindValue(currentDate);
            return query.exec();
        } else {
            // 记录不存在，插入新记录，计数=1
            QString insertStatsSql = "INSERT INTO print_task_statistics (date, count) VALUES (?, ?)";
            query.prepare(insertStatsSql);
            query.addBindValue(currentDate);
            query.addBindValue(1);
            return query.exec();
        }
    });
}

int PrintTaskDataManagement::countPrintTasks(const QList<int>& taskTypes, const QDateTime& startTime, const QDateTime& endTime) {

    QMutexLocker locker(&m_mutex); // 加锁
    QSqlDatabase db = createConnection();
    if (!db.isOpen()) {
            qDebug() << "Database is not open.";
            return 0;
        }

    QSqlQuery query(db);

    // 构造查询字符串
    QString queryStr = R"(SELECT count(*) FROM Print_task_history WHERE 1 = 1 AND timestamp >= ? AND timestamp <= ?)";

    if (!taskTypes.isEmpty()) {
        QStringList placeholders;
        for (int i = 0; i < taskTypes.size(); ++i) {
            placeholders.append(QString("?"));
        }
        queryStr += " AND task_Type IN (" + placeholders.join(", ") + ")";
    }

    if (!query.prepare(queryStr)) {
        qDebug() << "Failed to prepare query:" << query.lastError().text();
        return 0;
    }

    // 绑定时间参数
    query.bindValue(0, startTime.toString("yyyy-MM-dd")); //Qt::ISODate
    query.bindValue(1, endTime.toString("yyyy-MM-dd 23:59:59"));   //Qt::ISODate

    // 绑定任务类型参数
    for (int i = 0; i < taskTypes.size(); ++i) {
        query.bindValue(2 + i, taskTypes[i]);
    }

    // 执行查询
    if (!query.exec()) {
        qDebug() << "Error querying print tasks:" << query.lastError().text();
        return 0;
    }
    else
    {
        qDebug() << "Query executed successfully.";
        qDebug() << QString("queryStr == ") << queryStr;
        qDebug() << QString("startTime == ") << startTime.toString("yyyy-MM-dd");
        qDebug() << QString("endTime == ") << endTime.toString("yyyy-MM-dd 23:59:59");
        qDebug() << QString("taskTypes == ") << taskTypes;
    }

    if(query.next())
    {
        return query.value(0).toInt();
    }
    else
    {
        return 0;
    }

}

int PrintTaskDataManagement::getTotalPrintCount() {
    QMutexLocker locker(&m_mutex); // 加锁
    QSqlDatabase db = createConnection();
    if (!db.isOpen()) {
        qDebug() << "Database is not open.";
        return 0;
    }

    QSqlQuery query(db);
    QString queryStr = "SELECT SUM(count) FROM print_task_statistics";

    if (!query.exec(queryStr)) {
        qDebug() << "Error querying total print count:" << query.lastError().text();
        return 0;
    }

    if (query.next()) {
        return query.value(0).toInt();
    } else {
        return 0;
    }
}


QSqlQuery PrintTaskDataManagement::queryPrintTasks(const QList<int>& taskTypes, const QDateTime& startTime, const QDateTime& endTime) {
    QMutexLocker locker(&m_mutex); // 加锁
    QList<QVariantList> results;
    QSqlDatabase db = createConnection();
    if (!db.isOpen()) {
            qDebug() << "Database is not open.";
            return QSqlQuery();
        }

    QSqlQuery query(db);

    // 构造查询字符串
    QString queryStr = R"(SELECT * FROM Print_task_history WHERE 1 = 1 AND timestamp >= ? AND timestamp <= ?)";

    if (!taskTypes.isEmpty()) {
        QStringList placeholders;
        for (int i = 0; i < taskTypes.size(); ++i) {
            placeholders.append(QString("?"));
        }
        queryStr += " AND task_Type IN (" + placeholders.join(", ") + ")";
    }

    if (!query.prepare(queryStr)) {
        qDebug() << "Failed to prepare query:" << query.lastError().text();
        return QSqlQuery();
    }

    // 绑定时间参数
    query.bindValue(0, startTime.toString("yyyy-MM-dd")); //Qt::ISODate
    query.bindValue(1, endTime.toString("yyyy-MM-dd 23:59:59"));   //Qt::ISODate

    // 绑定任务类型参数
    for (int i = 0; i < taskTypes.size(); ++i) {
        query.bindValue(2 + i, taskTypes[i]);
    }

    // 执行查询
    if (!query.exec()) {
        qDebug() << "Error querying print tasks:" << query.lastError().text();
        return QSqlQuery();
    }
    else
    {
        qDebug() << "Query executed successfully.";
        qDebug() << QString("queryStr == ") << queryStr;
        qDebug() << QString("startTime == ") << startTime.toString("yyyy-MM-dd");
        qDebug() << QString("endTime == ") << endTime.toString("yyyy-MM-dd 23:59:59");
        qDebug() << QString("taskTypes == ") << taskTypes;
    }
    
    // 注意：这里不调用unlock()，因为QSqlQuery需要保持数据库连接
    // 锁会在查询结果被使用完毕后自动释放
    return query;
}

// 删除打印任务
bool PrintTaskDataManagement::deletePrintTask(const QString& taskId) {
    return executeTransaction([&](QSqlQuery& query) {
          QString deleteSql = R"(
              DELETE FROM print_task_history WHERE task_id = ?
          )";
          query.prepare(deleteSql);
          query.addBindValue(taskId);

          qDebug() << "deleteSql ==" << deleteSql;
          qDebug() << "taskId ==" << taskId;
          return query.exec();
      });
}

bool PrintTaskDataManagement::deleteHistoryBatch(const QStringList &guids)
{
    if (guids.isEmpty()) return true;

    QMutexLocker locker(&m_mutex);
    QSqlDatabase db = createConnection();
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    db.transaction();

    query.prepare("DELETE FROM Print_task_history WHERE task_id = ?");
    for (const QString &name : guids) {
        query.bindValue(0, name);
        if (!query.exec()) {
            qWarning() << "Delete failed for" << name << ":" << query.lastError().text();
            db.rollback();
            return false;
        }
    }

    db.commit();
    return true;
}

// 获取每日统计数据
bool PrintTaskDataManagement::getDailyStatistics(const QDate& startDate, const QDate& endDate, QList<QVariantList>& result) {
    QMutexLocker locker(&m_mutex); // 加锁
    QSqlDatabase db = createConnection();
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    QString sql = "SELECT date, count FROM print_task_statistics WHERE date >= ? AND date <= ? ORDER BY date";
    
    query.prepare(sql);
    query.addBindValue(startDate.toString("yyyy-MM-dd"));
    query.addBindValue(endDate.toString("yyyy-MM-dd"));
    
    if (!query.exec()) {
        qDebug() << "Error getting daily statistics:" << query.lastError().text();
        return false;
    }

    qDebug() << QString("getDailyStatistics.sql = %1").arg(sql);
    
    result.clear();
    while (query.next()) {
        QVariantList row;
        row << query.value(0) << query.value(1);
        result << row;
    }
    
    db.close();
    return true;
}

// 获取指定年份的每月统计数据
bool PrintTaskDataManagement::getMonthlyStatistics(int year, QList<QVariantList>& result) {
    QMutexLocker locker(&m_mutex); // 加锁
    QSqlDatabase db = createConnection();
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    // 从date字段中提取年份和月份，然后分组求和
    QString sql = "SELECT substr(date, 1, 7) as month, SUM(count) as total FROM print_task_statistics WHERE substr(date, 1, 4) = ? GROUP BY month ORDER BY month";
    
    query.prepare(sql);
    query.addBindValue(QString::number(year));

    qDebug() << QString("getMonthlyStatistics.sql = %1").arg(sql);
    qDebug() << QString("getMonthlyStatistics.year = %1").arg(QString::number(year));
    
    if (!query.exec()) {
        qDebug() << "Error getting monthly statistics:" << query.lastError().text();
        return false;
    }
    
    result.clear();
    while (query.next()) {
        QVariantList row;
        row << query.value(0) << query.value(1);
        result << row;
    }
    
    db.close();
    return true;
}

// 获取每年的统计数据
bool PrintTaskDataManagement::getYearlyStatistics(QList<QVariantList>& result) {
    QMutexLocker locker(&m_mutex); // 加锁
    QSqlDatabase db = createConnection();
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    // 从date字段中提取年份，然后分组求和
    QString sql = "SELECT substr(date, 1, 4) as year, SUM(count) as total FROM print_task_statistics GROUP BY year ORDER BY year";

    qDebug() << QString("getYearlyStatistics.sql = %1").arg(sql);
    
    if (!query.exec(sql)) {
        qDebug() << "Error getting yearly statistics:" << query.lastError().text();
        return false;
    }
    
    result.clear();
    while (query.next()) {
        QVariantList row;
        row << query.value(0) << query.value(1);
        result << row;
    }
    
    db.close();
    return true;
}

bool  PrintTaskDataManagement::statisticsPrintTask(const QString& sqlStr, QList<QVariantList>& result){
    QMutexLocker locker(&m_mutex); // 加锁
    QSqlDatabase db = createConnection();
    
    // 检查数据库连接是否打开，如果没有打开则尝试重新连接
    if (!db.isOpen()) {
        qDebug() << "Database is not open, attempting to reconnect...";
        if (!db.open()) {
            qDebug() << "Failed to reconnect to database:" << db.lastError().text();
            return false;
        }
    }

    QSqlQuery query(db);

    // 准备查询
    if (!query.prepare(sqlStr)) {
        qDebug() << "Failed to prepare query:" << query.lastError().text();
        qDebug() << "Query string:" << sqlStr;
        
        // 检查数据库连接是否仍然有效
        if (!db.isOpen() || db.lastError().isValid()) {
            qDebug() << "Database connection error, attempting to reconnect...";
            db.close();
            if (!db.open()) {
                qDebug() << "Failed to reconnect to database:" << db.lastError().text();
                return false;
            }
            
            // 重新准备查询
            if (!query.prepare(sqlStr)) {
                qDebug() << "Failed to prepare query after reconnect:" << query.lastError().text();
                return false;
            }
        } else {
            // 连接有效但查询准备失败，可能是SQL语句问题
            return false;
        }
    }

    // 执行查询
    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "Query executed successfully.";
    qDebug() << QString("queryStr == ") << sqlStr;

    // 处理查询结果
    while (query.next()) {
        QVariantList row;
        for (int i = 0; i < query.record().count(); ++i) {
            row.append(query.value(i));
        }
        result.append(row);
    }

    return true;
}

// 清除打印任务历史表中的所有数据
bool PrintTaskDataManagement::clearPrintTaskHistory() {
    return executeTransaction([&](QSqlQuery& query) {
        // 删除表并重新创建，比删除表内容更快且能重置自增ID
        QString dropSql = "DROP TABLE IF EXISTS print_task_history";
        if (!query.exec(dropSql)) {
            return false;
        }

        // 删除统计表
        QString dropStatsSql = "DROP TABLE IF EXISTS print_task_statistics";
        if (!query.exec(dropStatsSql)) {
            return false;
        }

        // 重新创建打印历史表
        QString createTableSql = R"(
            CREATE TABLE IF NOT EXISTS print_task_history (
                task_id TEXT PRIMARY KEY,
                content TEXT,
                task_type INTEGER,
                remark TEXT,
                timestamp TIMESTAMP default (datetime('now', 'localtime')));
        )";
        if (!query.exec(createTableSql)) {
            return false;
        }

        // 重新创建统计表
        QString createStatisticsTableSql = R"(
            CREATE TABLE IF NOT EXISTS print_task_statistics (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                date TEXT NOT NULL, -- 格式：YYYY-MM-DD
                count INTEGER DEFAULT 0,
                UNIQUE(date) -- 确保每天只有一条记录
            );
        )";
        return query.exec(createStatisticsTableSql);
    });
}
