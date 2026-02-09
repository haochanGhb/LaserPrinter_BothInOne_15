#ifndef HISTORYDIALOG_H
#define HISTORYDIALOG_H

#include <QCheckBox>
#include <QButtonGroup>
#include <QDateTimeEdit>
#include <QMouseEvent>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QLegend>
#include <QtCharts/QValueAxis>
#include <QFileDialog>
#include <QScroller>
#include <QScrollBar>
#include <QCalendarWidget>
#include <QToolButton>
#include <QSpinBox>

#include <QtCharts/QLineSeries>


#include "base_dialog.h"
#include "tasklist_panel.h"
#include "print_task_data_management.h"
#include "setting_dialog.h"
#include "tasklist_panel.h"
#include "history_table_model.h"

using namespace QtCharts;

// 定义统计数据结构，包含日期和打印次数
struct StatisticsData {
    QString date; // 日期字符串
    int count;    // 打印次数
};

// 定义统计数据向量类型
typedef std::vector<StatisticsData> StatisticsVector;

class MyDateTimeEdit : public QDateTimeEdit {
public:
    MyDateTimeEdit(QWidget *parent = nullptr) : QDateTimeEdit(parent) {
        setCalendarPopup(true);  // 启用日历弹出
        //setReadOnly(true);       // 禁用文本框编辑
    }

protected:
    // 重写鼠标滚轮事件，禁用滚轮修改日期
    void wheelEvent(QWheelEvent *event) override {
        // 阻止滚轮事件传播
        event->ignore();
    }

};

// 继承 QCalendarWidget，自定义调整年份 QSpinBox 的格式
class CustomCalendarWidget : public QCalendarWidget {
    Q_OBJECT
public:
    explicit CustomCalendarWidget(QWidget *parent = nullptr) : QCalendarWidget(parent) {

    }

protected:
    void showEvent(QShowEvent *event) override {
        QCalendarWidget::showEvent(event);  // 调用基类的 showEvent，确保正常显示

        // 查找 QToolButton (年份按钮) 和 QSpinBox (年份编辑)
        QToolButton *yearButton = this->findChild<QToolButton *>("qt_calendar_yearbutton");
        QSpinBox *yearSpinBox = this->findChild<QSpinBox *>("qt_calendar_yearedit");

        if (yearButton) {
            qDebug() << "Found QToolButton for year display!";
        }
        if (yearSpinBox) {
            qDebug() << "Found QSpinBox for year adjustment!";

            // 强制 QSpinBox 取消千位分隔符
            yearSpinBox->setLocale(QLocale(QLocale::English));  // 设置英文格式（无 `,`）
            //yearSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons); // 隐藏加减按钮

            // 监听 QSpinBox 值变化
            connect(yearSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int value) {
                qDebug() << "Year changed:" << value;
                if (yearButton) {
                    yearButton->setText(QString::number(value));  // 更新 QToolButton 显示
                }
            });

            // 监听 QSpinBox 编辑完成
            connect(yearSpinBox, &QAbstractSpinBox::editingFinished, this, [=]() {
                if (yearButton) {
                    int value = yearSpinBox->value();
                    yearButton->setText(QString::number(value));  // 确保 `QToolButton` 格式正确
                }
            });
        }
    }
};

class HistoryDialog : public BaseDialog
{
    Q_OBJECT
public:
    explicit HistoryDialog(BaseDialog *parent = nullptr);
    void Exec();
protected:
    void changeEvent(QEvent *event) override;
private slots:
    void slot_search_quick_time_buttonClicked(int id);
    void slot_statistics_operator_buttonClicked(int id);
    void slot_search_operator_buttonClicked(int id);
    void slot_search_buttonClicked();
    void slot_search_datetime_Changed();

    void on_select_all_item_clicked(int logicalIndex);
    void on_row_state_changed();
    void on_export_btn_clicked();
    void on_delete_btn_clicked();

private:
    QWidget * search_settings_area ;
    QWidget * history_table_area;

    QWidget * search_settings_area_left;
    QWidget * search_settings_area_right;

    QWidget * search_settings_left_panel1;
    QWidget * search_settings_left_panel2;
    QWidget * search_settings_left_panel3;

    //搜索类型选项-search_settings_left_panel1
    QLabel * search_type_Label;
    QCheckBox * search_type_checkbox1;
    QCheckBox * search_type_checkbox2;
    QCheckBox * search_type_checkbox3;

    //搜索时间快速按钮选项-search_settings_left_panel2
    QLabel * search_quick_time_Label;
    QButtonGroup * search_quick_time_button;

    //自定义时间选项-search_settings_left_panel3
    MyDateTimeEdit  *datetimeEdit_Begin;
    QLabel * hyphen_Label;
    MyDateTimeEdit  *datetimeEdit_End;

    QDateTime sel_startTime;
    QDateTime sel_endTime;

    //右边按钮操作组-search_settings_area_right
    QButtonGroup * search_operator_button;

    //历史列表区域
    QWidget *history_title_panel;
    QWidget *history_datetime_panel;
    QWidget *history_table_panel;

    QLabel *history_title;

    QPushButton *export_btn;
    QPushButton *delete_btn;
    QPushButton *search_btn;
    QLabel *history_record_label;
    //QTableWidget *history_table;
    QTableView *history_table;
    HistoryTableModel *history_model;

    //历史统计区域
    QWidget *history_statistics_panel;

    //统计上半部分区域
    QWidget *history_statistics_upper_panel;
    QLabel  *statistics_title;
    QLabel  *statistics_sum_info;
    QButtonGroup * statistics_quick_time_button;

    //统计下半部分区域
    QWidget *history_statistics_lower_panel;

    QLabel *m_total_label;

    PrintTaskDataManagement & print_task_manager = PrintTaskDataManagement::getInstance();

    MainSettings settings;

    QGraphicsScene *scene;
    QGraphicsView *view;
    QDate m_beginDate;
    QDate m_endDate;

    void set_layout();
    void set_style_sheet();
    void load_HistoryTask_List(const QList<int>& taskTypes, const QDateTime& startTime, const QDateTime& endTime);

    void connect_signals_and_slots();
    QString make_task_display_text(const QString &task_text);
    QString make_task_type_info(const QString &task_text);

    QFrame *seprate_line();

    QTimer *load_history_data_timer;
private:
    void retranslateUi();
    void loadHistoryQueryData_OnThread(int id);
    void show_history_search_panel(bool isShow);

    QString GenSQLQueryString(unsigned short int iperType);
    StatisticsVector LoadQueryResults(int perType);
    void DrawChartProc(int nPerType, const std::vector<int> &queryVector);
    void LoadSelectedDataToExportTable(QTableWidget & exportTableWidget);


    void DrawLineChartProc(int nPerType, const std::vector<int> &queryVector);
    void DrawBarChartProc(int nPerType, const StatisticsVector &statisticsVector);

    void copySelectedRowsToTable(QTableWidget *sourceTable, QTableWidget *targetTable);
    bool exportTableWidgetToCsv(QTableWidget* tableWidget, const QString& fileName);
    void exportSelectedRowsToCsv(const QModelIndexList &selectedRows, const QString &filePath);
    bool processTableData(QTableWidget *history_table, QTableWidget *testDataExportTableWidget,const QString ouputFilePath);
    QString escapeCsvField(const QString &field);
    void writeCsvRow(QTextStream &stream, const QStringList &fields);

    void refreshTotalNumber();
};

#endif // HISTORYDIALOG_H
