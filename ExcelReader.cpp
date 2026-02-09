#include "ExcelReader.h"
#include "xlsxdocument.h"
#include "xlsxworksheet.h"

#include <QDebug>

using namespace QXlsx;

ExcelReader::ExcelReader(QObject *parent)
    : QObject(parent)
{
}

bool ExcelReader::load(const QString &filePath)
{
    m_filePath = filePath;
    m_data.clear();
    m_rowCount = 0;
    m_colCount = 0;

    Document xlsx(filePath);
    if (!xlsx.load())
        return false;

    Worksheet *sheet = dynamic_cast<Worksheet *>(xlsx.currentWorksheet());
    if (!sheet)
        return false;

    auto cells = sheet->getFullCells(&m_rowCount, &m_colCount);
    qDebug() << "cell count:" << cells.size();
    qDebug() << "maxRow:" << m_rowCount << "maxCol:" << m_colCount;

//    for (int r = 1; r <= maxRow; ++r) {
//        for (int c = 1; c <= maxCol; ++c) {
//            if (auto cell = sheet->cellAt(r, c)) {
//                qDebug() << r << c << cell->value();
//            }
//        }
//    }


//    CellRange range = sheet->dimension();
//    m_rowCount = range.rowCount();
//    m_colCount = range.columnCount();

    for (int r = 1; r <= m_rowCount; ++r)
    {
        QStringList rowData;
        for (int c = 1; c <= m_colCount; ++c)
        {
            QVariant value = xlsx.read(r, c);
            rowData << value.toString();
        }
        m_data << rowData;
    }

    return true;
}

bool ExcelReader::setSheet(int index)
{
    Document xlsx(m_filePath);
    if (!xlsx.load())
        return false;

    if (index < 0 || index >= xlsx.sheetNames().size())
        return false;

    xlsx.selectSheet(xlsx.sheetNames().at(index));
    m_data.clear();

    Worksheet *sheet = dynamic_cast<Worksheet *>(xlsx.currentWorksheet());
    if (!sheet)
        return false;

    CellRange range = sheet->dimension();
    m_rowCount = range.rowCount();
    m_colCount = range.columnCount();

    for (int r = 1; r <= m_rowCount; ++r)
    {
        QStringList rowData;
        for (int c = 1; c <= m_colCount; ++c)
        {
            rowData << xlsx.read(r, c).toString();
        }
        m_data << rowData;
    }

    return true;
}

QVector<QStringList> ExcelReader::readAll() const
{
    return m_data;
}

int ExcelReader::rowCount() const
{
    return m_rowCount;
}

int ExcelReader::columnCount() const
{
    return m_colCount;
}

QString ExcelReader::cell(int row, int column) const
{
    if (row < 0 || row >= m_data.size())
        return QString();

    if (column < 0 || column >= m_data.at(row).size())
        return QString();

    return m_data.at(row).at(column);
}

QStandardItemModel *ExcelReader::toModel(QObject *parent) const
{
    auto *model = new QStandardItemModel(m_rowCount, m_colCount, parent);

    for (int r = 0; r < m_rowCount; ++r)
    {
        for (int c = 0; c < m_colCount; ++c)
        {
            model->setItem(r, c,
                new QStandardItem(m_data.at(r).at(c)));
        }
    }

    return model;
}

bool ExcelReader::forEachRow(const std::function<bool (int, const QStringList &)> &handler, int startRow)
{
    if (m_filePath.isEmpty())
        return false;

    QXlsx::Document xlsx(m_filePath);
    if (!xlsx.load())
        return false;

    QXlsx::Worksheet *sheet =
            dynamic_cast<QXlsx::Worksheet *>(xlsx.currentWorksheet());
    if (!sheet)
        return false;

    QXlsx::CellRange range = sheet->dimension();
    int rows = range.rowCount();
    int cols = range.columnCount();

    for (int r = startRow + 1; r <= rows; ++r)
    {
        QStringList rowData;
        rowData.reserve(cols);

        for (int c = 1; c <= cols; ++c)
        {
            QVariant v = xlsx.read(r, c);
            rowData << v.toString();
        }

        // handler 返回 false => 立即中断（非常关键）
        if (!handler(r - 1, rowData))
            break;
    }

    return true;
}
