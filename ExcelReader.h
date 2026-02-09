#ifndef EXCELREADER_H
#define EXCELREADER_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QStringList>
#include <QStandardItemModel>

class ExcelReader : public QObject
{
    Q_OBJECT
public:
    explicit ExcelReader(QObject *parent = nullptr);

    // 加载文件
    bool load(const QString &filePath);

    // 设置工作表（index 从 0 开始）
    bool setSheet(int index);

    // 返回所有数据（row x col）
    QVector<QStringList> readAll() const;

    // 行列数
    int rowCount() const;
    int columnCount() const;

    // 单元格读取（从 0 开始）
    QString cell(int row, int column) const;

    // 直接生成 Model（用于 QTableView）
    QStandardItemModel *toModel(QObject *parent = nullptr) const;

    //逐行读取（针对超大文件）
    bool forEachRow(const std::function<bool (int rowIndex, const QStringList &rowData)> &handler, int startRow = 0);

private:
    QString m_filePath;
    int m_rowCount = 0;
    int m_colCount = 0;

    QVector<QStringList> m_data;
};

#endif // EXCELREADER_H
