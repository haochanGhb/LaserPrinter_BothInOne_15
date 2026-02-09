#ifndef PRINTTASKDATAMANAGEMENT_H
#define PRINTTASKDATAMANAGEMENT_H

#include <QtSql>
#include <QDateTime>
#include <QDebug>
#include <QMutex>

class PrintTaskDataManagement {
public:
    static PrintTaskDataManagement& getInstance() {
        static PrintTaskDataManagement instance;
        return instance;
    }

    bool insertPrintTask(const QString& taskId, const QString& content, int taskType, const QString& remark);
    QSqlQuery  queryPrintTasks(const QList<int>& taskTypes, const QDateTime& startTime = {}, const QDateTime& endTime = {});
    int countPrintTasks(const QList<int>& taskTypes, const QDateTime& startTime = {}, const QDateTime& endTime = {});
    int getTotalPrintCount();

    bool deletePrintTask(const QString& taskId);
    bool deleteHistoryBatch(const QStringList &guids);

    bool  statisticsPrintTask(const QString& sqlStr, QList<QVariantList>& result);
    bool clearPrintTaskHistory();
    
    // 新的统计查询方法
    bool getDailyStatistics(const QDate& startDate, const QDate& endDate, QList<QVariantList>& result);
    bool getMonthlyStatistics(int year, QList<QVariantList>& result);
    bool getYearlyStatistics(QList<QVariantList>& result);
private:
    PrintTaskDataManagement();
    ~PrintTaskDataManagement();
    bool initDatabase();
    QSqlDatabase createConnection();

    QString m_dbName = "print_task_history.db";

    PrintTaskDataManagement(const PrintTaskDataManagement&) = delete;
    PrintTaskDataManagement& operator=(const PrintTaskDataManagement&) = delete;
    bool executeTransaction(const std::function<bool(QSqlQuery&)>& operation, int timeoutMs = 100);

    QMutex m_mutex;
};


#endif // PRINTTASKDATAMANAGEMENT_H
