#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QListView>
#include <QCheckBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QLineEdit>
#include <QFrame>
#include <QSettings>
#include <QButtonGroup>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QTranslator>
#include <QIntValidator>
#include <QtConcurrent/QtConcurrent>

#include <QJsonObject>
#include <QJsonDocument>
#include <QMutexLocker>
#include <QDateTime>
#include <QGroupBox>
#include <QHash>
#include <QList>

#ifdef Q_OS_WIN
#include <windows.h>
#include <shellapi.h>
#endif

#include "base_dialog.h"
#include "dialogbox.h"
#include "public_fun.h"
#include "waitframemanager.h"
#include "slotgroups_dialog.h"
#include "MarkPreview.h"
#include "touch_keyboard_controller.h"
#include "print_task_data_management.h"
#include "TouchKeyboardManager.h"
#include "DeviceDiscoveryDialog.h"


#define AUTO_TEST      0

/********************************************************************
 *
 * 设置对话框配置项
 *
 * ******************************************************************/
//language_panel
typedef struct{
    QString language_cbb;
}LanguageSetting;

//typein_panel
typedef struct{
    QString typein_connector_le;
    QString typein_delimiter_le;
    QString sub_number_merge_symbol_le;
    QString typein_subnum_front_identifier_le;
    bool preset_input_text_btn;
    bool touch_keyboard_show_btn;
}TypeinSetting;

//create_panel
typedef struct{
    int create_max_repeat_spb;
    QString create_repeat_mode_cbb;
    QString text_align_cbb;
    QString slot_switch_direct_cbb;
    int auto_del_expired_tasks_cbb;
    int laser_preheat_time_cbb;
    int laser_auto_sleep_cbb;
    int gas_treatment_auto_sleep_cbb;
    int gas_treatment_speed_level_cbb;
    int maintenance_remind_cbb;
    bool alarm_sound_onoff_btn;
    bool tray_empty_confirm_needed_btn;
}CreateSetting;

//list_panel
typedef struct{
    QString task_list_interval_symbol_le;
    QString print_list_interval_symbol_le;
    QString task_list_show_mode_cbb;
    QString print_list_show_mode_cbb;
    QString list_default_show_cbb;
}ListSetting;

//print_panel
typedef struct{
    bool auto_print_after_task_established_btn;
    bool continue_print_after_cleanup_btn;
    bool allow_duplicate_print_task_btn;
    bool auto_increase_after_blank_enter_btn;
    bool main_num_cb;
    bool sub_num_cb;
    bool continue_num_auto_add_zero_btn;
    bool auto_print_after_scan_btn;
    bool scan_code_separate_btn;
    bool scan_delimiter_rdb;
    QString scan_delimiter_le;
    bool scan_preset_text_length_rdb;
    QString preset_text_length_le;
}PrintSetting;

//preset_info_panel
typedef struct{
    bool preset_input_enable_btn;
}PresetSetting;

//remote_text_panel
typedef struct{
    bool remote_text_print_btn;
    QString remote_network_name_le;
    QString print_front_symbol_le;
    QString print_delimiter_le;
    QString print_tail_symbol_le;
    QString slots_front_symbol_le;
    QString slots_delimiter_le;
    QString slots_tail_symbol_le;
    QString remote_text_network_port_le;

    bool   remote_scan_send_enable_btn;
    QString remote_scan_send_ip_le;
    QString remote_scan_send_port_le;

    QString remote_field_1_le;
    QString remote_field_2_le;
    QString remote_field_3_le;
    QString remote_field_4_le;
    QString remote_field_5_le;
    QString remote_field_6_le;
    QString remote_field_7_le;
}RemoteTextSetting;

//remote_image_panel
typedef struct{
    bool remote_image_print_btn;
    QString remote_image_network_port_le;
}RemoteImageSetting;

//sync_print_panel
typedef struct{
    bool sync_print_btn;
    bool sync_print_network_rdb;
    bool sync_print_serial_rdb;
    QString sync_print_network_addr_le;
    QString sync_print_network_port_le;
}SyncPrintSetting;

//设置菜单配置项
typedef struct{
    LanguageSetting    language_setting;
    TypeinSetting      typein_setting;
    CreateSetting      create_setting;
    ListSetting        list_setting;
    PrintSetting       print_setting;
    PresetSetting      preset_setting;
    RemoteTextSetting  remote_text_setting;
    RemoteImageSetting remote_image_setting;
    SyncPrintSetting   sync_print_setting;
}MainSettings;


//// 定义一个结构体来存储状态信息
//struct StatusInfo {
//    int id;
//    QString message;
//};

// 定义提示信息结构体，包含显示文本和声音等级
struct McuAlertInfo {
    QString displayText;
    int soundLevel;  // 声音等级：0-关闭，3-普通提示，2-警告，1-严重错误
    QString errorCode; // 错误代码
};

McuAlertInfo getStatusMessage(int id);
QString convertDateTime(const QString &inputDateTime, const QString &targetFormat);
void realignLabelText(QLabel *lbl,const QString &temptext,QString textalign,int templateLeft = 0);
void setLabelAlignmentInfo(QLabel *lbl, const QString &temptext, QString textalign, int templateLeft = 0);

class JsonDataList {
public:
    struct JsonItem {
        QString key;
        QString value;
        QString keyAlias;  // 键的别名变量
    };
    QList<JsonItem> dataList;

public:
    // 解析 JSON 字符串并填充数据
    static JsonDataList parseJson(const QString& jsonString) {
        JsonDataList result;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
        if (jsonDoc.isNull() || !jsonDoc.isObject()) {
            qWarning() << "Invalid JSON format.";
            return result;
        }

        QJsonObject jsonObj = jsonDoc.object();
        for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
            JsonItem item;
            item.key = it.key();
            item.value = it.value().toString();
            item.keyAlias = "";  // 初始别名为空
            result.dataList.append(item);
        }
        return result;
    }

    QString getValueByKey(const QString& key) const {
        for (const auto& item : dataList) {
            if (item.key == key) {
                return item.value;
            }
        }
        return "";
    }

    // 获取MCU别名和声音等级
//    std::pair<QString, int> getMcuAliasByKey(const QString& key) {
//         const QMap<QString, McuAlertInfo> statusMap = initializeStatusMap();

//        if (statusMap.contains(key)) {
//            const auto& info = statusMap[key];
//            return {info.displayText, info.soundLevel};
//        } else {
//            return {QObject::tr("下位机上报出错状态，请检查！"), 2}; // 默认作为警告级别
//        }
//    }

    // 获取MCU别名、声音等级、错误代码
    McuAlertInfo getMcuAlertInfoByKey(const QString& key) {
        const QMap<QString, McuAlertInfo> statusMap = initializeStatusMap();
        if (statusMap.contains(key)) {
            return statusMap[key];
        } else {
            return {QObject::tr("下位机上报出错状态，请检查！"), 2, "105"}; // 默认作为警告，错误码105
        }
    }


    const QList<JsonDataList::JsonItem>& getDataList() const {
        return dataList;
    }
private:
    // 初始化状态映射表
     QMap<QString, McuAlertInfo> initializeStatusMap() {
        return {
            {"下位机初始化状态位",  {QObject::tr("下位机初始化状态位") ,3, ""}},
            {"当前槽盒位", {QObject::tr("当前槽盒位"),1}},
            {"当前槽盒电机位置", {QObject::tr("当前槽盒电机位置"),3, ""}},
            {"当前运载电机位置", {QObject::tr("当前运载电机位置"),3, ""}},
            {"当前翻转电机位置", {QObject::tr("当前翻转电机位置"),3, ""}},
            {"当前卸载电机位置", {QObject::tr("当前卸载电机位置"),3, ""}},
            {"已打印耗材数", {QObject::tr("已打印耗材数"),3, ""}},
            {"显示面板已合上", {QObject::tr("显示面板未合上"),3, ""}},
            {"卸载滑轨已接好", {QObject::tr("未放置玻片输出盒"),3, ""}},        //卸载滑轨未接好 (载玻片才有)
            {"载玻片物料盒是否已合上", {QObject::tr("未放置玻片输入盒"),3, ""}},  //载玻片物料盒未合上 (载玻片才有)
            {"下位机初始化超时", {QObject::tr("下位机初始化超时"),1, "103"}},
            {"控制板电源电压异常", {QObject::tr("控制板电源电压异常"),1, "205"}},
            {"非工作状态，下位机在线升级更新故障", {QObject::tr("非工作状态，下位机在线升级更新故障"),1, "104"}},

            {"翻转电机(收纳盒)电机错误", {QObject::tr("托盘位移受阻，请检查收集托盘，并清除卡滞的包埋盒。"),1, "309"}},
            {"卸载耗材错误", {QObject::tr("卸载耗材错误！"),1, ""}},
    #ifdef USE_CASSETTE
//            所有电机驱动错误提示已在电机复位做处理，此处提示冗余?
            {"槽盒电机驱动错误", {QObject::tr("槽盒位移受阻，请检查槽盒运转机构，并排除受阻异物。"),1, "307"}},         //槽盒电机驱动错误
            {"运载电机驱动错误", {QObject::tr("进料滑道阻塞，请检查进料滑道，并清除卡滞的包埋盒。"),1, "308"}},         //运载电机驱动错误
            {"翻转电机驱动错误", {QObject::tr("托盘位移受阻，请检查收集托盘，并清除卡滞的包埋盒。"),1, "309"}},         //翻转电机驱动错误
            {"卸载电机驱动错误", {QObject::tr("出料滑道阻塞，请检查出料滑道，并清除卡滞的包埋盒。"),1, "310"}},         //卸载电机驱动错误
            {"组织盒(载玻片)是否已经堆满", {QObject::tr("托盘收纳盒已满"),2, ""}}, //输出滑道已堆满,请清理后重试！
            {"取组织盒(或载玻片)时错误", {QObject::tr("进料滑道阻塞，请检查进料滑道，并清除卡滞的包埋盒。"),1, "308"}}, //tr("所选槽盒内组织盒已用完"),信号重复
            {"卸载组织盒(或载玻片)时错误", {QObject::tr("出料滑道阻塞，请检查出料滑道，并清除卡滞的包埋盒。"),1, "310"}}, //tr("卸载组织盒时错误"),信号提示不明确
    #elif defined(USE_SLIDE) || defined(USE_SLIDE_DOUBLE)
            {"运载电机驱动错误", {QObject::tr("进料滑道阻塞，请检查进料滑道，并清除卡滞的载波片。"),1, "308"}},         //运载电机驱动错误
            {"卸载电机驱动错误", {QObject::tr("出料滑道阻塞，请检查出料滑道，并清除卡滞的载波片。"),1, "310"}},         //卸载电机驱动错误
            {"组织盒(载玻片)是否已经堆满", {QObject::tr("玻片输出盒已满"),2, ""}},
            {"取组织盒(或载玻片)时错误", {QObject::tr("进料滑道阻塞，请检查进料滑道，并清除卡滞的载波片。"),1, "308"}},  //所选槽盒内载玻片已用完，运载电机堵转
            {"卸载组织盒(或载玻片)时错误", {QObject::tr("出料滑道阻塞，请检查出料滑道，并清除卡滞的载波片。"),1, "310"}} //卸载电机堵转
    #endif
        };
    }

};

class Logger {
public:
    static Logger& instance() {
        static Logger loggerInstance;
        return loggerInstance;
    }

    void log(const QString& location, const QString& message) {
        QMutexLocker locker(&mutex); // 确保线程安全

        // 打开日志文件
        QFile logFile("LaserPrintWork.log");
        if (!logFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
            return; // 如果无法打开文件，直接返回
        }

        // 读取现有日志
        QStringList logLines;
        QTextStream in(&logFile);
        while (!in.atEnd()) {
            logLines.append(in.readLine());
        }

        // 写入新日志
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz");
        QString logEntry = QString("[%1]:[%2]:[%3]").arg(timestamp,location, message);
        logLines.append(logEntry);

        // 如果日志超过 1000 行，删除旧记录
        while (logLines.size() > 1000) {
            logLines.removeFirst();
        }

        // 重新写入日志文件
        logFile.resize(0); // 清空文件内容
        QTextStream out(&logFile);
        for (const QString& line : logLines) {
            out << line << "\n";
        }
    }

private:
    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    QMutex mutex; // 用于保证多线程写入的线程安全
};


class GlobalClickGuard
{
public:
    static bool isAllowedClick(QObject* obj, int intervalMs = 3000);

private:
    static QHash<quintptr, qint64> lastClickTimes;
};

class SettingDialog : public BaseDialog
{
    Q_OBJECT
public:
    SettingDialog();

    static QString read_setting_from_file(const QString &group, const QString &key);
    static bool ReadSettings(MainSettings *settings);
    static void ChangeTranslator(const QString langStr);

    // 获取所有映射符
    static QStringList getAllMappings();
    // 根据映射符查找选中槽位的公用函数
    static QString findSelectedSlotsByKeyword(const QString &keyword);

    //获取和设置槽组编辑状态
    SlotGroupsEditState getSlotGroupsEditState() const;
    void setSlotGroupsEditState(const SlotGroupsEditState &state);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void changeEvent(QEvent *event) override;
private:
    QWidget *language_panel;
    QWidget *typein_panel;
    QWidget *create_panel;
    QWidget *list_panel;
    QWidget *print_panel;
    QWidget *preset_info_panel;
    QWidget *remote_text_panel;
    QWidget *remote_image_panel;
    QWidget *sync_print_panel;

    QPushButton *reset_btn;
    QPushButton *edit_btn;
    QPushButton *save_btn;
    QPushButton *cancel_btn;

    //language_panel
    QComboBox *language_cbb;

    QGroupBox *m_pGroupBoxNormal;

    //typein_panel
    QLineEdit *typein_connector_le;
    QLineEdit *typein_delimiter_le;
    QLineEdit *sub_number_merge_symbol_le;
    QLineEdit *typein_subnum_front_identifier_le;
    QPushButton *slot_groups_btn;
    QPushButton *preset_input_text_btn;
    QPushButton *preset_input_text_edit_btn;
    QPushButton *touch_keyboard_show_btn;

    //create_panel
    QSpinBox    *create_max_repeat_spb;
    QComboBox   *create_repeat_mode_cbb;
    QComboBox   *text_align_cbb;
    QComboBox   *slot_switch_direct_cbb;
    QComboBox   *auto_del_expired_tasks_cbb;

    QComboBox   *laser_preheat_time_cbb;
    QComboBox   *laser_auto_sleep_cbb;
    QComboBox   *gas_treatment_auto_sleep_cbb;
    QComboBox   *gas_treatment_speed_level_cbb;
    QComboBox   *maintenance_remind_cbb;
    QPushButton *alarm_sound_onoff_btn;
    QPushButton *tray_empty_confirm_needed_btn;

    //list_panel
    QLineEdit *task_list_interval_symbol_le;
    QLineEdit *print_list_interval_symbol_le;
    QComboBox *task_list_show_mode_cbb;
    QComboBox *print_list_show_mode_cbb;
    QComboBox *list_default_show_cbb;

    //print_panel
    QPushButton *auto_print_after_task_established_btn;
    QPushButton *continue_print_after_cleanup_btn;
    QPushButton *allow_duplicate_print_task_btn;

    QPushButton *auto_increase_after_blank_enter_btn;
    QCheckBox *main_num_cb, *sub_num_cb;

    QPushButton *continue_num_auto_add_zero_btn;
    QPushButton *auto_print_after_scan_btn;

    QPushButton *scan_code_separate_btn;
    QRadioButton *scan_delimiter_rdb;
    QLineEdit *scan_delimiter_le;
    QRadioButton *scan_preset_text_length_rdb;
    QLineEdit *preset_text_length_le;

    //preset_info_panel
    QPushButton *preset_input_enable_btn;
    QPushButton *preset_input_list_view_btn;

    //remote_text_panel
    QPushButton *remote_text_print_btn;
    QLineEdit *remote_network_name_le;
    QLabel *remote_print_text_analyze_lb, *remote_slot_text_analyze_lb;
    QLineEdit *print_front_symbol_le, *print_delimiter_le, *print_tail_symbol_le;
    QLineEdit *slots_front_symbol_le, *slots_delimiter_le, *slots_tail_symbol_le;

    QLineEdit *remote_text_network_port_le;
    QLineEdit *remote_field_1_le;
    QLineEdit *remote_field_2_le;
    QLineEdit *remote_field_3_le;
    QLineEdit *remote_field_4_le;
    QLineEdit *remote_field_5_le;
    QLineEdit *remote_field_6_le;
    QLineEdit *remote_field_7_le;

    //远程扫描发送
    QPushButton *remote_scan_send_enable_btn;
    QLineEdit *remote_scan_send_ip_le;
    QLineEdit *remote_scan_send_port_le;
    QPushButton *remote_scan_send_find_btn;

    //remote_image_panel
    QPushButton *remote_image_print_btn;
    QLineEdit *remote_image_network_port_le;

    //sync_print_panel
    QPushButton  *sync_print_btn;
    QLineEdit    *sync_print_network_addr_le;

    QRadioButton *sync_print_network_rdb;
    QRadioButton *sync_print_serial_rdb;
    QLineEdit    *sync_print_network_port_le;
    QPushButton  *sync_print_search_addr_btn;

    QLabel  *text_label(const QString &text);
    QWidget *create_item(const QString &item_name, QWidget *widget_a=nullptr, QWidget *widget_b=nullptr, QWidget *widget_c=nullptr);
    QWidget *create_item(QWidget *widget_a, QWidget *widget_b=nullptr, QWidget *widget_c=nullptr, QWidget *widget_d=nullptr);
    QWidget *create_item(QWidget *widget_a, QWidget *widget_b, Qt::Alignment align);
    QFrame  *seprate_line();

    //槽组编辑状态
    SlotGroupsEditState slotGroupsEditState;

    void create_widget();
    void setup_layout();
    void set_stylesheet();
    void connect_signals_and_slots();

    void write_preset_input_text_to_file(const QString &text);
    QString read_preset_input_text_from_file();

    void write_settings_to_file();
    void load_settings_from_file();
    void retranslateUi();

//#endif

    bool openTouchKeyboardSettingsDialog();
    bool registerForRestart(const QString &commandLine);
    void installWheelBlocker(QWidget *root);

signals:
    void signale_print_task_exit_request();
private slots:

    void on_remote_symbol_text_changed();
    void on_edit_btn_clicked();
    void on_save_btn_clicked();
    void on_cancel_btn_clicked();

    void on_slot_groups_btn_clicked();
    void on_preset_input_text_edit_btn_clicked();

    void on_back_btn_clicked() override;
    void on_language_selector_changed();
    void onSlotGroupsEditStateChanged(const SlotGroupsEditState &state);

    void on_touch_keyboard_show_btn_clicked();
    void on_reset_btn_clicked();

    //加载和保存槽组状态的方法
    void loadSlotGroupsStateFromIni();
    void saveSlotGroupsStateToIni();
};

#endif // SETTINGDIALOG_H
