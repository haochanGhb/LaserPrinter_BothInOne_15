#ifndef TASKLIST_PANEL_H
#define TASKLIST_PANEL_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QEvent>
#include <QDateTime>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QSettings>
#include <QCoreApplication>
#include <QDir>
#include <QProgressDialog>
#include <QMenu>
#include <QAction>
#include <QMutex>
#include <QThread>
#include <QTableView>
#include <QAbstractTableModel>

#include "stylesheet.h"
#include "dialogbox.h"
#include "public_fun.h"
#include "setting_dialog.h"
#include "task_table_model.h"
#include "status_item_delegate.h"

#include "shared_enums.h"


/*
class IconTextCellWidget : public QWidget
{
    Q_OBJECT

public:
    enum Status {
        Waiting,
        Printing
    };

    explicit IconTextCellWidget(PrintSta status, const QString &text, QWidget *parent = nullptr)
        : QWidget(parent)
    {
        iconLabel = new QLabel(this);
        textLabel = new QLabel(text, this);

        iconLabel->setFixedSize(24, 24);  // 图标大小
        iconLabel->setScaledContents(true);
        textLabel->setStyleSheet("font-size:18px;font-family:'微软雅黑';");

        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->setContentsMargins(8, 0, 8, 0);
        layout->setSpacing(10);
        layout->addWidget(iconLabel);
        layout->addWidget(textLabel);
        layout->addStretch();
        setLayout(layout);

        setFixedHeight(63); // 设置控件高度
        setState(status);
    }

    void setState(PrintSta status, const QString &text = QString()) {
        QPixmap pixmap;

        if (status == PRINT_STA_WAIT) {
            iconLabel->setPixmap(getWaitingIcon());
            if (!text.isEmpty()) textLabel->setText(text);
        } else if (status == PRINT_STA_PRINTING) {
            iconLabel->setPixmap(getPrintingIcon());
            if (!text.isEmpty()) textLabel->setText(text);
        }


    }

    void setSelected(bool selected) {
        if (selected)
            this->setStyleSheet("background-color: #d9f0f8;");
        else
            this->setStyleSheet("background-color: white;");
    }

signals:
    void clicked();
    void rightClicked();

protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton)
            emit clicked();
        else if (event->button() == Qt::RightButton)
            emit rightClicked();

        // 事件转发到父控件（QTableWidget）
//        if (parentWidget()) {
//            QMouseEvent *newEvent = new QMouseEvent(event->type(),
//                                                    mapToParent(event->pos()),
//                                                    event->globalPos(),
//                                                    event->button(),
//                                                    event->buttons(),
//                                                    event->modifiers());
//            QApplication::postEvent(parentWidget(), newEvent);
//        }

        QMouseEvent newEvent(event->type(),
                             mapToParent(event->pos()),
                             event->globalPos(),
                             event->button(),
                             event->buttons(),
                             event->modifiers());
         QApplication::sendEvent(parentWidget(), &newEvent);
    }

private:
    QLabel *iconLabel;
    QLabel *textLabel;

    // 懒加载图标函数（静态局部变量，只加载一次）
    static const QPixmap& getWaitingIcon() {
        static QPixmap pix(":/image/waiting.png");
        return pix;
    }

    static const QPixmap& getPrintingIcon() {
        static QPixmap pix(":/image/printing.png");
        return pix;
    }
};
*/


struct TaskDeleteRequest {
    QStringList keysToDelete;
    QStringList updatedArrangeList;
};

// 前向声明，避免循环引用
class MainWindow;
class TaskTableModel;
class PrintStateMachine;

class TaskListPanel : public QWidget
{
    Q_OBJECT
public:
    explicit TaskListPanel(QWidget *parent = nullptr);

    //初始化相关
    void setTitle(QString title);
    void setIniFilePath(const QString &iniFilePath);
    void setHeaderText(QString Col_1, QString Col_2, QString Col_3);
//    void setButtonText(QString mainBtnText, QString mainBtnCheckedText, QString delBtnText);
    void setIntervalSymbol(const QString &symbol);
//    void setActionButtonReady();

//    void execActionButtonClicked();
    void execDeleteButtonClicked(bool isButtonChecked);
    void deleteSelectedTasks();
    void setPrintStateMachine(PrintStateMachine *stateMachine);

    static QString task_type_display_str(int type);
    static QString sta_display_str(int sta);
    static QString slots_display_str(const QString &slots_num);

    int getListCount();
    int getSelectedRowCnt();
    QString getTaskdataDir();

    void setPanelMax();
    void setPanelMin();

    //任务列表相关
    QStringList addTextListToTaskTable(const QStringList &textList, TaskType taskType, bool isFront = false);
    void increaseTaskSta(QString taskName);
    QStringList getSelectedTaskList();

    QStringList addImageTaskToTaskTable(const QString &imageName, QByteArray imageData, bool isFront = false,bool isSelect = true);
    QStringList addImageTaskToTaskTable(const QString &imageName, const QImage &image, bool isFront = false);
    bool saveTaskImage(QString taskName, QByteArray imageData);
    bool saveTaskImage(QString taskName, QImage imageData);

    //打印列表相关
    //void setSelectedPrintTasksSta(PrintSta sta);
    void addTaskListToPrintTable(const QStringList &taskList, bool isFront = false,bool isSelect = true);
//    bool getPrintTask(QString &printName, QString &printTask);

//    bool getPrintActionCheckedState();
    void setPrintTaskState(const QString &printName,PrintSta sta);
    void setPrintTaskFinished(const QString &printName, QString &taskName);
    void deleteExpiredTasks(int expirationDay, QDate currentDate = QDate::currentDate());
    void setAllowDuplicatePrintTask(bool isAllow = true);

    bool isPanelMax();

    QStringList getAllWaitingTaskNames();
    bool isTaskStillWaiting(const QString& taskName);
    
    QStringList getAllWaitingTaskNamesFromModel();
    bool isTaskStillWaitingFromModel(const QString& taskName);
    QString getFirstWaitingTaskNameFromModel();
    TaskRecoverStatus getTaskRecoverStatus(const QString& taskName);
    TaskRecoverStatus getTaskRecoverStatusFromModel(const QString& taskName);
    TaskRecoverStatus getTaskRecoverStatusFromPrintListModel(const QString& taskName);
    QStringList getTaskArrangeOrder() const;
    QStringList getTaskArrangeOrderFromModel() const;

    void loadTaskListFromFile_OnThread();
signals:
    void rowAdded(int row);
    void batchRowsAdded(int start, int end);

protected:
    void changeEvent(QEvent *event) override;
private slots:

private:
    void retranslateUi();
    void loadTaskListFromFile();
    void updateTableRowTexts();
    QDateTime extractTaskTime(const TaskData &task) const;
    TaskData buildTaskFromText(const QString &task_name, const QString &task_text);
private:
    // MainWindow指针，用于访问主窗口状态
    MainWindow *mainWindow;
    PrintStateMachine *m_printStateMachine;
    int s_num = 0;
    QString ini_file;
    QWidget *title_area;
    QWidget *table_area;
    QLabel *title;
    //QPushButton *action_btn;
    QPushButton *delete_btn;
    QLabel *task_stat;
    QPushButton *max_btn;
    QString action_btn_checked_text;
    QString action_btn_text;

    //QTableWidget *task_table;
    QTableView *task_table;
    TaskTableModel *task_model;

    QMenu *context_menu;
    QAction *select_all_action;
    QAction *extract_action;
    QAction *delete_action;
    QAction *cancel_action;

    QString interval_symbol = " # ";

    bool allow_duplicate_print_task = true;

    QMutex m_mutex;

    bool m_bIsPrintStart = false;

    void set_layout();
    void set_style_sheet();

//    QString task_type_display_str(int type);
//    QString sta_display_str(int sta);
//    QString slots_display_str(const QString &slots_num);

    //void add_row(QString col_name, QString col_sta, QString col_info, QString col_content, QString content_data, bool is_front = false);
    void add_task_to_table(const QString &task_name, const QString &task_text, bool is_front = false);

    int serial_number();
    QStringList extractWaitingTasksFromIni(const QString& iniPath);
signals:
    void action_btn_Clicked(bool isChecked);
    void max_btn_Clicked(bool isChecked);
    void set_print_sta(const QString &printName, PrintSta sta);
    void signal_task_info(const QString &info);
    //void set_print_finished(const QString &printName);

    void task_content_list_Extracted(QStringList task_list);
private slots:
    void on_max_btn_clicked();
//    void on_action_btn_clicked();
    void on_delete_btn_clicked();
    void on_select_all_item_clicked(int logicalIndex);
    void on_row_state_changed();
    //void on_table_cellclick(QTableWidgetItem *item);
    void on_table_cellclick(const QModelIndex &index);

    void set_print_sta_slot(const QString &printName, PrintSta sta);
    void show_context_menu(const QPoint &pos);

};

#endif // TASKLIST_PANEL_H
