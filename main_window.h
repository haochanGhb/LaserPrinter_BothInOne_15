#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QApplication>
#include <QWidget>
#include <QEvent>
#include <QMouseEvent>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QDateTime>
#include <QVBoxLayout>
#include <QMenu>
#include <QAction>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QThread>
#include <QDebug>
#include <iostream>
#include <vector>
#include <algorithm>
#include <QFileDialog>
#include <QSerialPortInfo>

#include "QSimplePicCrop.h"

#include "stylesheet.h"
#include "title_bar.h"
#include "status_bar.h"
#include "input_panel.h"
#include "edit_panel.h"
#include "tasklist_panel.h"
#include "base_dialog.h"
#include "template_dialog.h"
#include "setting_dialog.h"
#include "maintenance_dialog.h"
#include "about_dialog.h"
#include "history_dialog.h"
#include "print_task_data_management.h"
#include "RemoteRecv.h"
#include "RemoteSend.h"

#include "setting_dialog.h"
#include "MarkHandle.h"
#include "StateHandle.h"
#include "shared_enums.h"
#include "DeviceDiscovery.h"
#include "ScannerCapture.h"
#include "ExcelReader.h"
#include "codec_select_dialog.h"

#include "mark_controller.h"
#include "task_scheduler.h"
#include "print_state_machine.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void on_print_task_exit_request();
    bool isPrintListAutoMode(PrintProcessSta &print_process_sta);
    QString getMcuVersion();
    int getEndPrintState();
signals:
    void allowDialogShow(const QString &desc,int level = 3);
//    void addTextListToTaskTableSignal(const QStringList &textList, TaskType taskType);
//    void addTaskListToPrintTableSignal(const QStringList &taskList, bool isFront);
private slots:
//    void on_addTextListToTaskTable(const QStringList &textList, TaskType taskType);
//    void on_addTaskListToPrintTable(const QStringList &taskList, bool isFront);
private:

//    void addTextListToTaskTable_OnThread(const QStringList &textList, TaskType taskType);
//    void addTaskListToPrintTable_OnThread(const QStringList &taskList, bool isFront);

    MainSettings settings;
    TemplateAttr temp_attr;

    QFuture<void> future;
    QFutureWatcher<void> watcher;
    QDate last_expired_tasks_delete_date; // 记录上次删除过期任务的日期

    PrintProcessSta print_process_sta;

    //顶部标题栏
    QWidget *title_area;
    TitleBar *title_bar;

    //中部区域
    QWidget *main_area;

    //编辑面板
    EditPanel *edit_panel;

    //列表操作面板
    QWidget * list_operate_area;
    QWidget * list_btn_panel;
    QTabWidget * list_tab_widget;
    QPushButton * list_auto_btn;  //自动
    QPushButton * list_view_btn;  //查看
    QPushButton * list_print_btn; //打印
    QPushButton * list_delete_btn; //删除

    //任务列表面板
    TaskListPanel *task_list_panel;

    //打印列表面板
    TaskListPanel *print_list_panel;

    //底部状态面板
    QWidget *status_area;
    StatusBar *status_bar;

    MarkHandle *m_pMarkHandle;

    RemoteRecv *m_pRemoteRecv;
    RemoteRecv *m_pRemoteComRecv;
    RemoteRecv *m_pRemoteSyncRecv;
    RemoteSend *m_pRemoteSend;
    RemoteSend *m_pRemoteScannerSend;

    StateHandle *m_pStateHandle;

    int preheat_cnt;

    void set_layout();
    void set_style_sheet();
    void connect_signals_and_slots();

    void load_data();

    PenArg m_mark_level_args[10];
    PenArg m_mark_image_level_args[10];
    void load_mark_args();
    void load_mark_image_args();

    void laser_preheat(int preheat_time);

    void create_task(bool auto_add_print_list = false, bool print_instant = false);

    void print_process();
    bool read_taskTemplate_param(const QString &task_tempName, TemplateAttr &task_tempAttr);
    void load_template_attr(const QString &tempName);
    void load_template_attr_OnThread(const QString &tempName);

    QString make_task_text_data_from_txt(const QString &remote_text);
    QString make_task_text_data_from_csv(const QString &csv_line_content);
    QStringList getTaskDataListByXlsxParser(const QString &filePath, bool *loadOk);
    QString getTaskDataByCsvParser(const QString &srcText);
    bool isExcelRowEmpty(const QStringList &row);

    QString last_create_task_from_file_path;
    bool get_text_from_file(QString file_path, QStringList &content_list);

    void init_communication_setting();
    void setup_remoteRecv_connection();
    void setup_remoteSend_connection();
    void initScannerRececiverDiscovery();

    void refreshTodayPrintNumber();
    int getLastReminderCycle();
    void saveCurrentReminderCycle(int cycle);

    int getLastReminderPrintCount();
    void saveLastReminderPrintCount(int count);

    int getReminderCount();
    void saveReminderCount(int count);
    int getCurrentPrintCount();
    void checkForReminder();


    QPoint dragPosition;
    bool isDraggable = false;
    bool isMainWindowShow = false;
    int checkReminderCount = 3;
    bool isCloseRemoteSyncSend = false;

    QSimplePicCrop *m_pPicCrop;

    DeviceDiscovery * m_pScannerReceiverDiscovery ;

private slots:
    void slotOnMcuVersion(QString result);
    void slotOnEndPrintState(QString result, int stateValue);

    //顶部标题栏
    void on_history_option_triggered();
    void on_template_option_triggered();
    void on_setting_option_triggered();
    void on_maintenance_option_triggered();
    void on_about_option_triggered();
    void on_quit_btn_clicked();

    //编辑面板
    void on_edit_panel_create_btn_clicked();
    void on_edit_panel_create_task_action_triggered();//菜单创建任务
    void on_edit_panel_create_task_and_print_action_triggered();//菜单创建任务和打印
    void on_edit_panel_create_task_and_print_instant_action_triggered();//菜单创建任务和打印并插队
    void on_edit_panel_template_selector_changed(QString currentTemp);
    void on_edit_panel_create_task_from_file_action_Triggered();
    void on_task_list_content_extracted(QStringList task_list);

    void on_expand_box();
    void retract_box_box();

    //任务列表面板
    void on_task_list_print_btn_clicked();
    void on_task_list_btnMaxMin_clicked(bool isChecked);

    //打印列表面板
    void on_print_list_start_btn_clicked(bool isChecked);
    void on_print_list_btnMaxMin_clicked(bool isChecked);

    void on_list_delete_btn_clicked();

    //点击任务列表获取打印信息
    void on_get_task_list_info(const QString  & infoStr);

    //程序退出处理
    void on_application_about_to_quit();

    //标刻模块
    void on_print_finished(QString print_name);

    void slot_mark_State(int stateNum, QString desc);
    void slot_mark_FinishStart();
    void slot_mark_AlreadyStopped();
    void slot_on_prepared_box_ready(quint8 boxIndex);

    void slot_RecvTextTask(quint64 taskIndex, QStringList boxIndex, QByteArray text);
    void slot_RecvPicTask(quint64 taskIndex, QStringList boxIndex, QByteArray pic);
    void slot_RecvRemoteState(int stateNum, QString desc);
    void slot_RecvRemoteLinkState(bool isOnline);
    void slot_RecvRemoteComLinkState(bool isOnline);
    void slot_RecvRemoteSyncLinkState(bool isOnline);

    void slot_McuState(QString msg);
    void slot_McuAlarm(QString msg);

    void slot_list_tab_widget_currentChanged(int index);
    void slot_setStartButtonChecked(bool isChecked);
    void slot_setfanoff();

    // 新增的槽函数
    void checkPrintState();
    void handleTaskReady(const QString &taskName);
    void handleMarkStarted();
    void handleMarkFinished();
    void handleMarkError(const QString &error);
    void handleMarkStateChanged(int stateNum, const QString &desc);
    void handleStateChanged(PrintProcessSta newState, PrintProcessSta oldState);
    void handleTaskQueueEmpty();
    
    PrintProcessSta getPrintStateMachineState() const;
private:
    //void startMark(quint8 boxIndex,QStringList &textList);
    void startMark();
    void handle_task_ready(const QString& taskName);
    void handle_task_startMark();
    void handle_task_changeSlot();
    void handle_task_finished();
    void stopMark();

    void handle_task_ended();
    void handle_task_ended_signal();
    void handle_task_exception();
    void handle_ready_state_entered();
    void handle_start_mark_state_entered();
    void handle_change_slot_state_entered();
    void handle_finished_state_entered();
    void handle_ended_state_entered();
    void handle_exception_state_entered();
    void setup_expired_tasks_monitor();

    void selectOptimalSlot();
    void selectAlternatingSlot();
    void setFanOffDelay(bool isSetTime);
    void setFanOffDelay(int iSec = 20);
    QString make_remote_text_data(const QString &task_text);
    void retranslateUi();
    //void setActionButtonReady();
    void translateDefaultTemp();
    void setList_auto_btn(bool isStart = false);
    void setActionButtonReady();
    void setTrayButtonEnable(bool isEnable);

    QStringList processSlotStrings(const QString &slots_num_str);
    QString convertRemoteTaskData(const QString &input);
    QString processSlotsString(const QString &slotsStr, bool &isPureNumber);

    Align_type convertAlignType(QString textAlign);
protected:
    void changeEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    // 重写 showEvent
    void showEvent(QShowEvent *event) override;
private:
    QString m_mcuVersion;
    int m_endPrintState = -1;
    QString current_print_name;
    QString current_print_task;
    std::vector<int> m_current_slots;
    int m_slots_current_index = -1;
    int m_slots_last_index = -1;

    int m_FanOff_delay_count = 0;
    bool m_FanOn_Start = false;
    //int m_nPanel_Close = 0;
    bool m_Init_finished = false;

    //托盘电机状态
    int m_turn_motor_state = 0;

    bool m_pause = false;

    JsonDataList m_nMcuStateDataList;
    JsonDataList m_nMcuAlarmDataList;

    QShortcut *shortcut_enter;
    QShortcut *shortcut_return;
    QShortcut *shortcut_tab;
    QShortcut *shortcut_ctrl_enter ;
    QShortcut *shortcut_up;
    QShortcut *shortcut_down;

    QTimer * expired_tasks_monitor_timer;

    QTimer * link_sta_detect_timer ;

    QTimer * remote_init_test_timer ;

    PrintTaskDataManagement & print_task_manager = PrintTaskDataManagement::getInstance();

    QMutex m_mutex;

    // 新增的类成员
    MarkController *m_markController;
    TaskScheduler *m_taskScheduler;
    PrintStateMachine *m_printStateMachine;

    // 状态检查定时器
    QTimer *m_state_check_timer;
};

void filterTaskText(const QStringList &task_text_list, const QString &text_index, QStringList &bar_code_list);

#endif // MAIN_WINDOW_H
