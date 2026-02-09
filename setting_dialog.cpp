#include "setting_dialog.h"
#include "menu_dialog_style.h"

QString convertDateTime(const QString &inputDateTime, const QString &targetFormat)
{
    // 固定的输入格式
    QString inputFormat = "yyyy-MM-dd HH:mm:ss";

    // 使用 QDateTime 解析输入字符串
    QDateTime dateTime = QDateTime::fromString(inputDateTime, inputFormat);

    // 检查解析是否成功
    if (!dateTime.isValid()) {
        return "Invalid DateTime Format";
    }

    // 按目标格式返回字符串
    return dateTime.toString(targetFormat);
}

/**
 * @brief 根据基准字符串宽度重新对齐 QLabel 的文本
 * @param lbl      需要调整的 QLabel
 * @param temptext 基准字符串（比如 "tempStr"）
 * @param align    对齐方式（Left, Center, Right）
 */
void realignLabelText(QLabel *lbl,const QString &temptext,QString textalign,int templateLeft)
{
    if (!lbl) return;

    //QWidget *parent = lbl->parentWidget();
    //if (!parent) return;

    // 字体测量
    QFont labelFont = lbl->font();
    QFontMetrics fmLabel(labelFont);

    QFont tempFont = labelFont;
    //if (tempItalic) tempFont.setItalic(true);
    QFontMetrics fmTemp(tempFont);

    QString labelText = lbl->text();
    int labelW = fmLabel.horizontalAdvance(labelText);
    int tempW = fmTemp.horizontalAdvance(temptext);

    // 基准：tempStr 的左边界 X（这里认为是 lbl 所在坐标系下的 templateLeft）
    int templateLeftLocal = templateLeft;

    // 计算新坐标
    int newX = lbl->x();
    if(textalign.compare("left")==0)
    {
        newX = templateLeftLocal;
    }
    if(textalign.compare("middle")==0)
    {
        int Xc = templateLeftLocal + tempW / 2;
        newX = Xc - labelW / 2;
    }
    if(textalign.compare("right")==0)
    {
        int Xr = templateLeftLocal + tempW;
        newX = Xr - labelW ;
    }

    // 可选：label 自适应宽度
    lbl->setFixedWidth(labelW);

    // 移动到新的位置
    lbl->move(newX, lbl->y());
}

void setLabelAlignmentInfo(QLabel *lbl, const QString &temptext, QString textalign, int templateLeft)
{
    if (!lbl) return;

    // 获取标签字体
    QFont labelFont = lbl->font();
    QFontMetrics fm(labelFont);

    // 计算基准文本的宽度和高度
    int tempW = fm.horizontalAdvance(temptext);
    int tempH = fm.height();

    // 计算新的x坐标
    int xnew = templateLeft;
    if(textalign.compare("left") == 0) {
        // 左对齐：取temptext原来的x坐标
        xnew = templateLeft;
    } else if(textalign.compare("middle") == 0) {
        // 居中对齐：取temptext宽度的中间位置
        xnew = templateLeft + tempW / 2;
    } else if(textalign.compare("right") == 0) {
        // 靠右对齐：取temptext的宽度位置
        xnew = templateLeft + tempW;
    }

    // 纵坐标取lbl的纵坐标
    int y = lbl->y();

    // 创建JSON对象
    QJsonObject jsonObj;
    jsonObj["temp_x"] = xnew;
    jsonObj["temp_y"] = y;
    jsonObj["temp_w"] = tempW;
    jsonObj["temp_h"] = tempH;

    // 将JSON对象转换为字符串
    QJsonDocument jsonDoc(jsonObj);
    QString jsonStr = jsonDoc.toJson(QJsonDocument::Compact);

    // 将JSON字符串赋值给lbl的objectName
    lbl->setObjectName(jsonStr);
}

// 定义一个方法，传入相应的 id，返回相应结构体里面的状态信息字符串
//std::pair<QString, int> getStatusMessage(int id) {
McuAlertInfo getStatusMessage(int id) {
     const QMap<int, McuAlertInfo> statusMap = [] {
        QMap<int, McuAlertInfo> map;
        map.insert(7, {QObject::tr("正在关闭风扇"), 3, ""});
        map.insert(6, {QObject::tr("正在打开风扇"), 3, ""});
        map.insert(5, {QObject::tr("标刻模块正在初始化，请稍等"), 3, ""});
        map.insert(4, {QObject::tr("读取结束标刻的状态"), 3, ""});
        map.insert(3, {QObject::tr("结束标刻"), 3, ""});
        map.insert(2, {QObject::tr("正在移动槽盒"), 3, ""});
        map.insert(1, {QObject::tr("槽盒准备就绪，开始进行标刻"), 3, ""});
        map.insert(0, {QObject::tr("初始化完成，标刻模块准备就绪"), 3, ""});
//        map.insert(-1, {QObject::tr("本地串口发送失败"), 3, "101"});
        map.insert(-1, {QObject::tr("本地串口被占用，请重新启动程序"), 3, "101"});
        map.insert(-2, {QObject::tr("本地串口发送指令正常，但没有该指令的回复(单片机未收到/本地协议解析有误/本地接收异常)"), 3, "102"});
        map.insert(-3, {QObject::tr("无法识别激光控制板，请检查USB连接"), 2, "201"});
        map.insert(-4, {QObject::tr("MCU复位全局电机超时"), 1, "301"});
        map.insert(-5, {QObject::tr("槽盒板位检查位提示错误"), 2, "302"});     //槽盒板位检查位提示错误
        //map.insert(-6, {QObject::tr("槽盒电机堵转"), 1, "307"});  //槽盒电机位置检查位提示错误
        //map.insert(-7, {QObject::tr("运载电机堵转"), 1, "308"});  //运载电机位置检查位提示错误
        //map.insert(-8, {QObject::tr("托盘电机堵转"), 1, "309"});  //翻转电机位置检查位提示错误
        //map.insert(-9, {QObject::tr("卸载电机堵转"), 1, "310"});  //卸载电机位置检查位提示错误
        map.insert(-16, {QObject::tr("槽盒电机复位失败"), 1, "303"});  //槽盒电机位置检查位提示错误
        map.insert(-17, {QObject::tr("运载电机复位失败"), 1, "304"});  //运载电机位置检查位提示错误
        map.insert(-18, {QObject::tr("托盘电机复位失败"), 1, "305"});  //翻转电机位置检查位提示错误
        map.insert(-19, {QObject::tr("卸载电机复位失败"), 1, "306"});  //卸载电机位置检查位提示错误

#ifdef USE_CASSETTE
        map.insert(-6, {QObject::tr("槽盒位移受阻，请检查槽盒运转机构，并排除受阻异物。"), 1, "307"});  //槽盒电机位置检查位提示错误
        map.insert(-7, {QObject::tr("进料滑道阻塞，请检查进料滑道，并清除卡滞的包埋盒。"), 1, "308"});  //运载电机位置检查位提示错误
        map.insert(-8, {QObject::tr("托盘位移受阻，请检查收集托盘，并清除卡滞的包埋盒。"), 1, "309"});  //翻转电机位置检查位提示错误
        map.insert(-9, {QObject::tr("出料滑道阻塞，请检查出料滑道，并清除卡滞的包埋盒。"), 1, "310"});  //卸载电机位置检查位提示错误

        map.insert(-10, {QObject::tr("所选槽盒内组织盒已用完"), 2, ""});
        map.insert(-14, {QObject::tr("托盘收纳盒已满"), 2, ""});
        map.insert(-15, {QObject::tr("卸载组织盒时错误"), 2, ""});
        map.insert(-20, {QObject::tr("托盘收纳盒有耗材，请清理！"), 2, ""});
#elif defined(USE_SLIDE) || defined(USE_SLIDE_DOUBLE)
        map.insert(-7, {QObject::tr("进料滑道阻塞，请检查进料滑道，并清除卡滞的载波片。"), 1, "308"});  //运载电机位置检查位提示错误
        map.insert(-9, {QObject::tr("出料滑道阻塞，请检查出料滑道，并清除卡滞的载波片。"), 1, "310"});  //卸载电机位置检查位提示错误

        map.insert(-10, {QObject::tr("玻片输入盒内玻片已用完"), 2, ""});  //槽盒内载玻片已用完
        map.insert(-14, {QObject::tr("玻片输出盒已满"), 2, ""});
        map.insert(-15, {QObject::tr("卸载载玻片时错误"), 1, "312"});
#endif

        map.insert(-11, {QObject::tr("准备耗材超时"), 2, ""});
        map.insert(-12, {QObject::tr("标刻模块未完成初始化"), 2, "202"});
        map.insert(-13, {QObject::tr("标刻失败，具体失败原因请查看BslModule.log文件"), 3, "203"});
        map.insert(-99, {QObject::tr("准备标刻过程等待回复状态超过15秒"), 3, ""});

       return map;
    }();

    if (statusMap.contains(id)) {
        const auto& info = statusMap[id];
                return {info.displayText, info.soundLevel, info.errorCode};
    }
    else
    {
        return {QObject::tr("未定义的状态 ID"), 1, "204"};
    }
}


QHash<quintptr, qint64> GlobalClickGuard::lastClickTimes;

bool GlobalClickGuard::isAllowedClick(QObject* obj, int intervalMs)
{
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    quintptr key = reinterpret_cast<quintptr>(obj);

    if (lastClickTimes.contains(key))
    {
        qint64 delta = now - lastClickTimes[key];
        if (delta < intervalMs)
        {
            return false;
        }
    }

    lastClickTimes[key] = now;
    return true;
}


SettingDialog::SettingDialog()
{
    setTitle(tr("设置"));
    create_widget();
    setup_layout();
    set_stylesheet();
    connect_signals_and_slots();

    load_settings_from_file();

    //初始化槽组编辑状态
    loadSlotGroupsStateFromIni();
}

QString SettingDialog::read_setting_from_file(const QString &group, const QString &key)
{
    QString read_val;
    QSettings settings_ini(SETTINGS_INI, QSettings::IniFormat);
    settings_ini.beginGroup(group);
    read_val = settings_ini.value(key, "").toString();
    settings_ini.endGroup();

    return read_val;
}

void SettingDialog::create_widget()
{
    //panels
    language_panel = new QWidget(this);
    language_panel->setFixedWidth(780);
    typein_panel = new QWidget(this);
    typein_panel->setFixedWidth(780);
    create_panel = new QWidget(this);
    create_panel->setFixedWidth(780);
    list_panel = new QWidget(this);
    list_panel->setFixedWidth(780);
    print_panel = new QWidget(this);
    print_panel->setFixedWidth(780);
    preset_info_panel = new QWidget(this);
    preset_info_panel->setFixedWidth(780);
    remote_text_panel = new QWidget(this);
    remote_text_panel->setFixedWidth(780);

    remote_image_panel = new QWidget(this);
    remote_image_panel->setFixedWidth(780);
    remote_image_panel->hide();

    sync_print_panel = new QWidget(this);
    sync_print_panel->setFixedWidth(780);

    //language_panel
    language_cbb = new QComboBox(language_panel);
    language_cbb->setFixedSize(180, 40);

    language_cbb->addItem("简体中文");
    language_cbb->addItem("English");

    //typein_panel
    typein_connector_le = new QLineEdit(typein_panel);
    typein_connector_le->setFixedSize(180, 40);
    typein_delimiter_le = new QLineEdit(typein_panel);
    typein_delimiter_le->setFixedSize(180, 40);

    //sub_number_merge_symbol_le = new QLineEdit(create_panel);
    sub_number_merge_symbol_le = new QLineEdit(typein_panel);
    sub_number_merge_symbol_le->setFixedSize(180, 40);

    typein_subnum_front_identifier_le = new QLineEdit(typein_panel);
    typein_subnum_front_identifier_le->setFixedSize(180, 40);
    typein_subnum_front_identifier_le->hide();

    slot_groups_btn = new QPushButton(tr("..."), typein_panel);
    slot_groups_btn->setFixedSize(115, 40);

    preset_input_text_btn = new QPushButton(typein_panel);
    preset_input_text_btn->setFixedSize(115, 40);
    preset_input_text_btn->setCheckable(true);
    preset_input_text_edit_btn = new QPushButton(tr("..."), typein_panel);
    preset_input_text_edit_btn->setFixedSize(115, 40);

    touch_keyboard_show_btn  = new QPushButton(typein_panel);
    touch_keyboard_show_btn->setFixedSize(115, 40);
    touch_keyboard_show_btn->setCheckable(true);

    //create_panel
    create_max_repeat_spb = new QSpinBox(create_panel);
    create_max_repeat_spb->setFixedSize(180, 40);
    create_max_repeat_spb->setRange(1, 99);
    create_repeat_mode_cbb = new QComboBox(create_panel);
    create_repeat_mode_cbb->setFixedSize(180, 40);
    create_repeat_mode_cbb->addItem("123..123..");
    create_repeat_mode_cbb->addItem("11..22..33..");

    text_align_cbb = new QComboBox(create_panel);
    text_align_cbb->setFixedSize(180, 40);
    text_align_cbb->addItem(tr("居左"), QString("left"));
    text_align_cbb->addItem(tr("居中"), QString("middle"));
    text_align_cbb->addItem(tr("居右"), QString("right"));
    //text_align_cbb->hide();

    slot_switch_direct_cbb = new QComboBox(create_panel);
    slot_switch_direct_cbb->setFixedSize(180, 40);
    slot_switch_direct_cbb->addItem(tr("从头查找"), QString("head"));
    slot_switch_direct_cbb->addItem(tr("往后查找"), QString("backward"));
    slot_switch_direct_cbb->hide();

    auto_del_expired_tasks_cbb = new QComboBox(create_panel);
    auto_del_expired_tasks_cbb->setFixedSize(180, 40);
    auto_del_expired_tasks_cbb->setMaxVisibleItems(12);
    auto_del_expired_tasks_cbb->addItem(tr("关闭"), 0);
    //for(int i=0;i<10;i++){auto_del_expired_tasks_cbb->addItem(QString(tr("%1个工作日")).arg(i+1), i+1);}
    auto_del_expired_tasks_cbb->addItem(QString("%1 %2").arg(1).arg(tr("个工作日 ")), 1);//为区分英文复数
    for(int i=1;i<10;i++){auto_del_expired_tasks_cbb->addItem(QString("%1 %2").arg(i+1).arg(tr("个工作日")), i+1);}

    laser_preheat_time_cbb = new QComboBox(create_panel);
    laser_preheat_time_cbb->setFixedSize(180, 40);
    laser_preheat_time_cbb->addItem(tr("关闭"), 0);
    for(int i=30;i<=150;i+=30){laser_preheat_time_cbb->addItem(QString(tr("%1秒")).arg(i), i);}
    laser_preheat_time_cbb->hide();

    laser_auto_sleep_cbb = new QComboBox(create_panel);
    laser_auto_sleep_cbb->setFixedSize(180, 40);
    laser_auto_sleep_cbb->addItem(tr("关闭"), 0);

    for(int i=0;i<5;i++){laser_auto_sleep_cbb->addItem(QString(tr("%1小时")).arg(i+1), i+1);}
    laser_auto_sleep_cbb->addItem(tr("8小时"), 8);
    laser_auto_sleep_cbb->addItem(tr("12小时"), 12);
    laser_auto_sleep_cbb->addItem(tr("24小时"), 24);
    laser_auto_sleep_cbb->hide();

    gas_treatment_auto_sleep_cbb = new QComboBox(create_panel);
    gas_treatment_auto_sleep_cbb->setFixedSize(180, 40);
    gas_treatment_auto_sleep_cbb->addItem(tr("关闭"), 0);
    gas_treatment_auto_sleep_cbb->addItem(tr("3秒"), 3);
    gas_treatment_auto_sleep_cbb->addItem(tr("5秒"), 5);
    gas_treatment_auto_sleep_cbb->addItem(tr("10秒"), 10);
    gas_treatment_auto_sleep_cbb->addItem(tr("20秒"), 20);

    gas_treatment_speed_level_cbb = new QComboBox(create_panel);
    gas_treatment_speed_level_cbb->setFixedSize(180, 40);
    gas_treatment_speed_level_cbb->addItem(QString("1(%1)").arg(tr("最慢")), 0);
    gas_treatment_speed_level_cbb->addItem("2", 1);
    gas_treatment_speed_level_cbb->addItem("3", 2);
    gas_treatment_speed_level_cbb->addItem("4", 3);
    //gas_treatment_speed_level_cbb->addItem(QString("5(%1)").arg(tr("最快")), 4); //最大风扇功率容易引起系统复位，隐藏

    maintenance_remind_cbb = new QComboBox(create_panel);
    maintenance_remind_cbb->setFixedSize(180, 40);
    maintenance_remind_cbb->addItem(tr("关闭"), 0);
    maintenance_remind_cbb->addItem(tr("2000次"), 2000);
    maintenance_remind_cbb->addItem(tr("3000次"), 3000);
    maintenance_remind_cbb->addItem(tr("5000次"), 5000);
    maintenance_remind_cbb->addItem(tr("10000次"), 10000);

    alarm_sound_onoff_btn = new QPushButton(create_panel);
    alarm_sound_onoff_btn->setFixedSize(115, 40);
    alarm_sound_onoff_btn->setCheckable(true);

    tray_empty_confirm_needed_btn = new QPushButton(create_panel);
    tray_empty_confirm_needed_btn->setFixedSize(115, 40);
    tray_empty_confirm_needed_btn->setCheckable(true);

    //list_panel
    task_list_interval_symbol_le = new QLineEdit(list_panel);
    task_list_interval_symbol_le->setFixedSize(180, 40);
    print_list_interval_symbol_le = new QLineEdit(list_panel);
    print_list_interval_symbol_le->setFixedSize(180, 40);
    task_list_show_mode_cbb = new QComboBox(list_panel);
    task_list_show_mode_cbb->setFixedSize(180, 40);
    task_list_show_mode_cbb->addItem(tr("精简"), "simple");
    task_list_show_mode_cbb->addItem(tr("详细"), "detail");
    task_list_show_mode_cbb->hide();

    print_list_show_mode_cbb = new QComboBox(list_panel);
    print_list_show_mode_cbb->setFixedSize(180, 40);
    print_list_show_mode_cbb->addItem(tr("精简"), "simple");
    print_list_show_mode_cbb->addItem(tr("详细"), "detail");
    print_list_show_mode_cbb->hide();

    list_default_show_cbb = new QComboBox(list_panel);
    list_default_show_cbb->setFixedSize(180, 40);
    list_default_show_cbb->addItem(tr("任务列表"), "task");
    list_default_show_cbb->addItem(tr("打印列表"), "print");

    //print_panel
    auto_print_after_task_established_btn = new QPushButton(print_panel);
    auto_print_after_task_established_btn->setFixedSize(115, 40);
    auto_print_after_task_established_btn->setCheckable(true);
    continue_print_after_cleanup_btn = new QPushButton(print_panel);
    continue_print_after_cleanup_btn->setFixedSize(115, 40);
    continue_print_after_cleanup_btn->setCheckable(true);
    continue_print_after_cleanup_btn->hide();

    allow_duplicate_print_task_btn = new QPushButton(print_panel);
    allow_duplicate_print_task_btn->setFixedSize(115, 40);
    allow_duplicate_print_task_btn->setCheckable(true);

    auto_increase_after_blank_enter_btn = new QPushButton(print_panel);
    auto_increase_after_blank_enter_btn->setFixedSize(115, 40);
    auto_increase_after_blank_enter_btn->setCheckable(true);
    auto_increase_after_blank_enter_btn->hide();

    main_num_cb = new QCheckBox(tr("病理号"), print_panel);
    sub_num_cb = new QCheckBox(tr("小号"), print_panel);

    continue_num_auto_add_zero_btn = new QPushButton(print_panel);
    continue_num_auto_add_zero_btn->setFixedSize(115, 40);
    continue_num_auto_add_zero_btn->setCheckable(true);
    continue_num_auto_add_zero_btn->hide();

    auto_print_after_scan_btn = new QPushButton(print_panel);
    auto_print_after_scan_btn->setFixedSize(115, 40);
    auto_print_after_scan_btn->setCheckable(true);

    scan_code_separate_btn = new QPushButton(print_panel);
    scan_code_separate_btn->setFixedSize(115, 40);
    scan_code_separate_btn->setCheckable(true);
    scan_delimiter_rdb = new QRadioButton(tr("分隔符"), print_panel);
    scan_delimiter_le = new QLineEdit(print_panel);
    scan_delimiter_le->setFixedSize(180, 40);

    scan_preset_text_length_rdb = new QRadioButton(tr("预设字段长度(以\"/\"分隔)"), print_panel);
    QButtonGroup *scan_code_btn_group = new QButtonGroup(print_panel);
    scan_code_btn_group->addButton(scan_delimiter_rdb);
    scan_code_btn_group->addButton(scan_preset_text_length_rdb);
    preset_text_length_le = new QLineEdit(print_panel);
    preset_text_length_le->setFixedSize(180, 40);

//    scan_preset_text_length_rdb = new QRadioButton(tr("模板预设字段长度"), print_panel);
//    QButtonGroup *scan_code_btn_group = new QButtonGroup(print_panel);
//    scan_code_btn_group->addButton(scan_delimiter_rdb);
//    scan_code_btn_group->addButton(scan_preset_text_length_rdb);

    //preset_info_panel
    preset_input_enable_btn = new QPushButton(preset_info_panel);
    preset_input_enable_btn->setFixedSize(115, 40);
    preset_input_enable_btn->setCheckable(true);
    preset_input_enable_btn->hide();

    preset_input_list_view_btn = new QPushButton(preset_info_panel);
    preset_input_list_view_btn->setFixedSize(115, 40);
    preset_input_list_view_btn->hide();

    //remote_text_panel
    remote_text_print_btn = new QPushButton(remote_text_panel);
    remote_text_print_btn->setFixedSize(115, 40);
    remote_text_print_btn->setCheckable(true);
    remote_print_text_analyze_lb = new QLabel();
    remote_slot_text_analyze_lb = new QLabel();
    remote_network_name_le = new QLineEdit(remote_text_panel);
    remote_network_name_le->setFixedSize(180, 40);
    //remote_network_name_le->hide();

    remote_scan_send_enable_btn = new QPushButton(remote_text_panel);
    remote_scan_send_enable_btn->setFixedSize(115, 40);
    remote_scan_send_enable_btn->setCheckable(true);

    remote_scan_send_ip_le = new QLineEdit(remote_text_panel);
    remote_scan_send_ip_le->setFixedSize(180, 40);
    QRegularExpressionValidator *ipValidator = new QRegularExpressionValidator(
            QRegularExpression("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"),
            this
        );
    remote_scan_send_ip_le->setValidator(ipValidator);

    remote_scan_send_port_le = new QLineEdit(remote_text_panel);
    remote_scan_send_port_le->setFixedSize(180, 40);
    QIntValidator *portvalidator = new QIntValidator(0, 99999, remote_text_panel);
    remote_scan_send_port_le->setValidator(portvalidator);

    remote_scan_send_find_btn = new QPushButton(tr("..."), remote_text_panel);
    remote_scan_send_find_btn->setFixedSize(115, 40);
//    remote_scan_send_find_btn->hide();

    print_front_symbol_le = new QLineEdit(remote_text_panel);
    print_front_symbol_le->setFixedSize(58, 40);
//    print_front_symbol_le->hide();
    print_delimiter_le = new QLineEdit(remote_text_panel);
    print_delimiter_le->setFixedSize(60, 40);
//    print_delimiter_le->hide();
    print_tail_symbol_le = new QLineEdit(remote_text_panel);
    print_tail_symbol_le->setFixedSize(58, 40);
//    print_tail_symbol_le->hide();
    slots_front_symbol_le = new QLineEdit(remote_text_panel);
    slots_front_symbol_le->setFixedSize(58, 40);
//    slots_front_symbol_le->hide();
    slots_delimiter_le = new QLineEdit(remote_text_panel);
    slots_delimiter_le->setFixedSize(60, 40);
//    slots_delimiter_le->hide();
    slots_tail_symbol_le = new QLineEdit(remote_text_panel);
    slots_tail_symbol_le->setFixedSize(58, 40);
//    slots_tail_symbol_le->hide();

    remote_text_network_port_le = new QLineEdit(remote_text_panel);
    remote_text_network_port_le->setFixedSize(180, 40);

    QIntValidator *validator = new QIntValidator(0, 99999, remote_text_panel);
    remote_text_network_port_le->setValidator(validator);


//    remote_field_1_le = new QLineEdit(remote_text_panel);
//    remote_field_1_le->setFixedSize(180, 40);
//    remote_field_1_le->hide();
//    remote_field_2_le = new QLineEdit(remote_text_panel);
//    remote_field_2_le->setFixedSize(180, 40);
//    remote_field_2_le->hide();
//    remote_field_3_le = new QLineEdit(remote_text_panel);
//    remote_field_3_le->setFixedSize(180, 40);
//    remote_field_3_le->hide();
//    remote_field_4_le = new QLineEdit(remote_text_panel);
//    remote_field_4_le->setFixedSize(180, 40);
//    remote_field_4_le->hide();
//    remote_field_5_le = new QLineEdit(remote_text_panel);
//    remote_field_5_le->setFixedSize(180, 40);
//    remote_field_5_le->hide();
//    remote_field_6_le = new QLineEdit(remote_text_panel);
//    remote_field_6_le->setFixedSize(180, 40);
//    remote_field_6_le->hide();
//    remote_field_7_le = new QLineEdit(remote_text_panel);
//    remote_field_7_le->setFixedSize(180, 40);
//    remote_field_7_le->hide();

    //remote_image_panel
    remote_image_print_btn = new QPushButton(remote_image_panel);
    remote_image_print_btn->setFixedSize(115, 40);
    remote_image_print_btn->setCheckable(true);
    remote_image_print_btn->hide();

    remote_image_network_port_le = new QLineEdit(remote_image_panel);
    remote_image_network_port_le->setFixedSize(180, 40);
    remote_image_network_port_le->hide();

    //sync_print_panel
    sync_print_btn = new QPushButton(sync_print_panel);
    sync_print_btn->setFixedSize(115, 40);
    sync_print_btn->setCheckable(true);

    sync_print_network_addr_le = new QLineEdit(sync_print_panel);
    sync_print_network_addr_le->setFixedSize(200, 40);
    QRegExp rx("^[0-9.]+$");
    sync_print_network_addr_le->setValidator(new QRegExpValidator(rx, sync_print_panel));

    sync_print_network_rdb = new QRadioButton(tr("网络"), sync_print_panel);
    sync_print_network_rdb->hide();
    sync_print_serial_rdb = new QRadioButton(tr("串口"), sync_print_panel);
    sync_print_serial_rdb->hide();

//    QButtonGroup *sync_print_btn_group = new QButtonGroup(sync_print_panel);
//    sync_print_btn_group->addButton(sync_print_network_rdb);
//    sync_print_btn_group->addButton(sync_print_serial_rdb);

    sync_print_network_port_le = new QLineEdit(sync_print_panel);
    sync_print_network_port_le->setFixedSize(200, 40);
    sync_print_network_port_le->setText("9990");
//    sync_print_network_port_le->setValidator(validator);
    sync_print_network_port_le->hide();

    sync_print_search_addr_btn = new QPushButton(sync_print_panel);
    sync_print_search_addr_btn->setFixedSize(115, 40);
    sync_print_search_addr_btn->setCheckable(true);
    sync_print_search_addr_btn->hide();

    //底部按钮
    reset_btn = new QPushButton();
    reset_btn->setFixedSize(180,40);
    reset_btn->setText(tr("恢复出厂设置"));
    reset_btn->hide();

    edit_btn = new QPushButton(this);
    edit_btn->setFixedSize(120,40);
    edit_btn->setText(tr("编辑"));
    save_btn = new QPushButton();
    save_btn->setFixedSize(120,40);
    save_btn->setText(tr("保存"));
    cancel_btn = new QPushButton();
    cancel_btn->setFixedSize(120,40);
    cancel_btn->setText(tr("取消"));

    edit_btn->setVisible(true);
    save_btn->setVisible(false);
    cancel_btn->setVisible(false);

    main_area->setEnabled(false);
}
#if 1
void SettingDialog::setup_layout()
{

    //language_panel
    QVBoxLayout *language_panel_v_layout = new QVBoxLayout(language_panel);
    language_panel_v_layout->setContentsMargins(5,5,5,5);
    language_panel_v_layout->setSpacing(5);
    language_panel_v_layout->addWidget(create_item(tr("语言"), language_cbb));

    //typein_panel
    QVBoxLayout *typein_panel_v_layout = new QVBoxLayout(typein_panel);
    typein_panel_v_layout->setContentsMargins(5,5,5,5);
    typein_panel_v_layout->setSpacing(5);
    typein_panel_v_layout->addWidget(create_item(tr("多号连续符"), typein_connector_le));
    typein_panel_v_layout->addWidget(seprate_line());
    typein_panel_v_layout->addWidget(create_item(tr("多号分隔符"), typein_delimiter_le));
    typein_panel_v_layout->addWidget(seprate_line());
    typein_panel_v_layout->addWidget(create_item(tr("小号合并符"), sub_number_merge_symbol_le));
#ifdef USE_CASSETTE
    typein_panel_v_layout->addWidget(seprate_line());
    typein_panel_v_layout->addWidget(create_item(tr("选槽分组"), slot_groups_btn));
#elif defined(USE_SLIDE) || defined(USE_SLIDE_DOUBLE)
    slot_groups_btn->hide();
#endif

    typein_panel_v_layout->addWidget(seprate_line());
    typein_panel_v_layout->addWidget(create_item(tr("预设输入信息"), preset_input_text_btn));
    typein_panel_v_layout->addWidget(create_item(tr("预设输入信息列表"), preset_input_text_edit_btn));
//   typein_panel_v_layout->addWidget(seprate_line());
//   typein_panel_v_layout->addWidget(create_item(tr("小号前端自动插入标识符"), typein_subnum_front_identifier_le));

    if(!TOUCH_KEYBORAD_DISABLED)
    {
       typein_panel_v_layout->addWidget(seprate_line());
       typein_panel_v_layout->addWidget(create_item(tr("开启触摸键盘"), touch_keyboard_show_btn));
    }
    else
    {
        touch_keyboard_show_btn->hide();
    }

    //create_panel
    QVBoxLayout *create_panel_v_layout = new QVBoxLayout(create_panel);
    create_panel_v_layout->setContentsMargins(5,5,5,5);
    create_panel_v_layout->setSpacing(5);
    create_panel_v_layout->addWidget(create_item(tr("创建任务时的重复次数最大可选值"), create_max_repeat_spb));
    create_panel_v_layout->addWidget(seprate_line());
    create_panel_v_layout->addWidget(create_item(tr("多号任务叠加重复次数后的排列方式"), create_repeat_mode_cbb));
    create_panel_v_layout->addWidget(seprate_line());
    create_panel_v_layout->addWidget(create_item(tr("字段的打印排版"), text_align_cbb));
    create_panel_v_layout->addWidget(seprate_line());
//    create_panel_v_layout->addWidget(create_item(tr("换槽取组织盒的顺序"), slot_switch_direct_cbb));
//    create_panel_v_layout->addWidget(seprate_line());
    create_panel_v_layout->addWidget(create_item(tr("自动删除任务列表内的过期任务"), auto_del_expired_tasks_cbb));
    create_panel_v_layout->addWidget(seprate_line());
//    create_panel_v_layout->addWidget(create_item(tr("激光预热时间"), laser_preheat_time_cbb));
//    create_panel_v_layout->addWidget(seprate_line());
//    create_panel_v_layout->addWidget(create_item(tr("激光器自动休眠"), laser_auto_sleep_cbb));
//    create_panel_v_layout->addWidget(seprate_line());
    create_panel_v_layout->addWidget(create_item(tr("气体处理装置自动休眠"), gas_treatment_auto_sleep_cbb));
    //create_panel_v_layout->addWidget(seprate_line());
    create_panel_v_layout->addWidget(create_item(tr("气体处理装置速度档位"), gas_treatment_speed_level_cbb));
    create_panel_v_layout->addWidget(seprate_line());
    create_panel_v_layout->addWidget(create_item(tr("仪器维护提醒间隔"), maintenance_remind_cbb));
    create_panel_v_layout->addWidget(seprate_line());
    create_panel_v_layout->addWidget(create_item(tr("报警提示音"), alarm_sound_onoff_btn));
#ifdef USE_CASSETTE
    create_panel_v_layout->addWidget(seprate_line());
    create_panel_v_layout->addWidget(create_item(tr("托盘进去前提示"), tray_empty_confirm_needed_btn));
#elif defined(USE_SLIDE) || defined(USE_SLIDE_DOUBLE)
    tray_empty_confirm_needed_btn->hide();
#endif

    //list_panel
    QVBoxLayout *list_panel_v_layout = new QVBoxLayout(list_panel);
    list_panel_v_layout->setContentsMargins(5,5,5,5);
    list_panel_v_layout->setSpacing(5);
    list_panel_v_layout->addWidget(create_item(tr("任务列表各个字段之间的间隔"), task_list_interval_symbol_le));
    list_panel_v_layout->addWidget(seprate_line());
    list_panel_v_layout->addWidget(create_item(tr("打印列表各个字段之间的间隔"), print_list_interval_symbol_le));
//    list_panel_v_layout->addWidget(seprate_line());
//    list_panel_v_layout->addWidget(create_item(tr("任务列表显示模式（重启软件生效）"), task_list_show_mode_cbb));
//    list_panel_v_layout->addWidget(seprate_line());
//    list_panel_v_layout->addWidget(create_item(tr("打印列表显示模式（重启软件生效）"), print_list_show_mode_cbb));
    list_panel_v_layout->addWidget(seprate_line());
    list_panel_v_layout->addWidget(create_item(tr("主页默认显示列表"), list_default_show_cbb));

    //print_panel
    QVBoxLayout *print_panel_v_layout = new QVBoxLayout(print_panel);
    print_panel_v_layout->setContentsMargins(5,5,5,5);
    print_panel_v_layout->setSpacing(5);
    print_panel_v_layout->addWidget(create_item(tr("任务录入以后自动打印"), auto_print_after_task_established_btn));
    print_panel_v_layout->addWidget(seprate_line());
//    print_panel_v_layout->addWidget(create_item(tr("清理输出滑道后自动继续打印"), continue_print_after_cleanup_btn));
//    print_panel_v_layout->addWidget(seprate_line());
    print_panel_v_layout->addWidget(create_item(tr("允许打印列表中任务重复"), allow_duplicate_print_task_btn));
    print_panel_v_layout->addWidget(seprate_line());
//    print_panel_v_layout->addWidget(create_item(tr("空编号回车自动增加"), auto_increase_after_blank_enter_btn));
//    print_panel_v_layout->addWidget(create_item(main_num_cb, sub_num_cb, Qt::AlignLeft));
//    print_panel_v_layout->addWidget(seprate_line());
//    print_panel_v_layout->addWidget(create_item(tr("连号任务高位智能补0"), continue_num_auto_add_zero_btn));
    //print_panel_v_layout->addWidget(seprate_line());
    print_panel_v_layout->addWidget(create_item(tr("编号即扫即打"), auto_print_after_scan_btn));
    print_panel_v_layout->addWidget(seprate_line());

    print_panel_v_layout->addWidget(create_item(tr("扫码分割"), scan_code_separate_btn));
    print_panel_v_layout->addWidget(create_item(scan_delimiter_rdb, scan_delimiter_le, NULL));
    print_panel_v_layout->addWidget(create_item(scan_preset_text_length_rdb, preset_text_length_le, NULL));

//    print_panel_v_layout->addWidget(create_item(tr("扫码分割"), scan_code_separate_btn));
//    print_panel_v_layout->addWidget(create_item(scan_delimiter_rdb, scan_delimiter_le, NULL));
//    print_panel_v_layout->addWidget(create_item(scan_preset_text_length_rdb, NULL, NULL));

    //preset_info_panel
//    QVBoxLayout *preset_info_panel_v_layout = new QVBoxLayout(preset_info_panel);
//    preset_info_panel_v_layout->setContentsMargins(5,5,5,5);
//    preset_info_panel_v_layout->setSpacing(5);
//    preset_info_panel_v_layout->addWidget(create_item(tr("预设输入信息"), preset_input_enable_btn));
//    preset_info_panel_v_layout->addWidget(create_item(tr("预设输入信息列表"), preset_input_list_view_btn));

    //remote_text_panel
    QVBoxLayout *remote_text_panel_v_layout = new QVBoxLayout(remote_text_panel);
    remote_text_panel_v_layout->setContentsMargins(5,5,5,5);
    remote_text_panel_v_layout->setSpacing(5);
    remote_text_panel_v_layout->addWidget(create_item(tr("远程文本/图片打印"), remote_text_print_btn));
    remote_text_panel_v_layout->addWidget(seprate_line());
    remote_text_panel_v_layout->addWidget(create_item(remote_print_text_analyze_lb, print_front_symbol_le, print_delimiter_le, print_tail_symbol_le));
    remote_text_panel_v_layout->addWidget(create_item(remote_slot_text_analyze_lb, slots_front_symbol_le, slots_delimiter_le,slots_tail_symbol_le));

    remote_text_panel_v_layout->addWidget(seprate_line());
    remote_text_panel_v_layout->addWidget(create_item(tr("设备网络名称"), remote_network_name_le));
    remote_text_panel_v_layout->addWidget(create_item(tr("设备网络端口号"), remote_text_network_port_le));
    remote_text_panel_v_layout->addWidget(seprate_line());

    remote_text_panel_v_layout->addWidget(create_item(tr("扫码发送"), remote_scan_send_enable_btn));
//    remote_text_panel_v_layout->addWidget(create_item(tr("IP地址和端口"), remote_scan_send_ip_le, remote_scan_send_port_le));
    remote_text_panel_v_layout->addWidget(create_item(tr("IP地址和端口"), remote_scan_send_ip_le, remote_scan_send_port_le,remote_scan_send_find_btn));

//    remote_text_panel_v_layout->addWidget(create_item(tr("远程字段1"), remote_field_1_le));
//    remote_text_panel_v_layout->addWidget(create_item(tr("远程字段2"), remote_field_2_le));
//    remote_text_panel_v_layout->addWidget(create_item(tr("远程字段3"), remote_field_3_le));
//    remote_text_panel_v_layout->addWidget(create_item(tr("远程字段4"), remote_field_4_le));
//    remote_text_panel_v_layout->addWidget(create_item(tr("远程字段5"), remote_field_5_le));
//    remote_text_panel_v_layout->addWidget(create_item(tr("远程字段6"), remote_field_6_le));
//    remote_text_panel_v_layout->addWidget(create_item(tr("远程字段7"), remote_field_7_le));

    //remote_image_panel
//    QVBoxLayout *remote_image_panel_v_layout = new QVBoxLayout(remote_image_panel);
//    remote_image_panel_v_layout->setContentsMargins(5,5,5,5);
//    remote_image_panel_v_layout->setSpacing(5);
//    remote_image_panel_v_layout->addWidget(create_item(tr("远程标签图片打印（LIS->本仪器）"), remote_image_print_btn));
//    remote_image_panel_v_layout->addWidget(create_item(tr("设备网络端口号"), remote_image_network_port_le));

    //sync_print_panel
    QVBoxLayout *sync_print_panel_v_layout = new QVBoxLayout(sync_print_panel);
    sync_print_panel_v_layout->setContentsMargins(5,5,5,5);
    sync_print_panel_v_layout->setSpacing(5);
    sync_print_panel_v_layout->addWidget(create_item(tr("同步打印"), sync_print_btn));
    sync_print_panel_v_layout->addWidget(create_item(tr("IP地址"), sync_print_network_addr_le));
    //sync_print_network_addr_le->installEventFilter(this);
 //   sync_print_panel_v_layout->addWidget(create_item(tr("设备网络端口号"),sync_print_network_port_le));
    //sync_print_panel_v_layout->addWidget(create_item(sync_print_network_rdb, sync_print_network_addr_le, sync_print_search_addr_btn));
    //sync_print_panel_v_layout->addWidget(create_item(sync_print_serial_rdb, NULL, NULL));

    //add panels to main layout
    main_layout->addStretch();
    main_layout->addWidget(language_panel);
    main_layout->addWidget(typein_panel);
    main_layout->addWidget(create_panel);
    main_layout->addWidget(list_panel);
    main_layout->addWidget(print_panel);
    main_layout->addWidget(preset_info_panel);
    main_layout->addWidget(remote_text_panel);
    //main_layout->addWidget(remote_image_panel);
    main_layout->addWidget(sync_print_panel);
    main_layout->addStretch();

    //bottom_bar
    bottom_bar_layout->addWidget(reset_btn, 0, Qt::AlignVCenter);
    bottom_bar_layout->addStretch();
    bottom_bar_layout->addWidget(edit_btn, 0, Qt::AlignVCenter);
    bottom_bar_layout->addWidget(save_btn, 0, Qt::AlignVCenter);
    bottom_bar_layout->addWidget(cancel_btn, 0, Qt::AlignVCenter);
}
#else
void SettingDialog::setup_layout()
{

    QVBoxLayout *pVLayoutBackgroundPanel = new QVBoxLayout();
    pVLayoutBackgroundPanel->setContentsMargins(20,20,20,20);

    QWidget *pBackgroundPanel = new QWidget();
    pBackgroundPanel->setStyleSheet(SS_ItemPanels);
    pBackgroundPanel->setLayout(pVLayoutBackgroundPanel);

    QVBoxLayout *pVLayoutGroupBoxNormal = new QVBoxLayout();
    pVLayoutGroupBoxNormal->setSpacing(20);
    pVLayoutGroupBoxNormal->addWidget(create_item(tr("语言"), language_cbb));
    pVLayoutGroupBoxNormal->addWidget(create_item(tr("设备网络端口号"), remote_text_network_port_le));
    pVLayoutGroupBoxNormal->setContentsMargins(20,20,20,20);

    QGroupBox *pGroupBoxNormal = new QGroupBox("通用设置");
    pGroupBoxNormal->setLayout(pVLayoutGroupBoxNormal);
//    pGroupBoxNormal->setStyleSheet("background: #ffffff; border: 1px solid #4d4d4d;font-weight: bold;font-size: 20px;font-family: '微软雅黑';color:#898989;");

    pVLayoutBackgroundPanel->addWidget(pGroupBoxNormal);

    //add panels to main layout
    main_layout->addWidget(pBackgroundPanel);
    main_layout->addStretch();

    //bottom_bar
    bottom_bar_layout->addWidget(reset_btn, 0, Qt::AlignVCenter);
    bottom_bar_layout->addStretch();
    bottom_bar_layout->addWidget(edit_btn, 0, Qt::AlignVCenter);
    bottom_bar_layout->addWidget(save_btn, 0, Qt::AlignVCenter);
    bottom_bar_layout->addWidget(cancel_btn, 0, Qt::AlignVCenter);
}
#endif
void SettingDialog::set_stylesheet()
{

    //panels
    language_panel->setStyleSheet(SS_ItemPanels);
    typein_panel->setStyleSheet(SS_ItemPanels);
    create_panel->setStyleSheet(SS_ItemPanels);
    list_panel->setStyleSheet(SS_ItemPanels);
    print_panel->setStyleSheet(SS_ItemPanels);
    preset_info_panel->setStyleSheet(SS_ItemPanels);
    remote_text_panel->setStyleSheet(SS_ItemPanels);
    remote_image_panel->setStyleSheet(SS_ItemPanels);
    sync_print_panel->setStyleSheet(SS_ItemPanels);

    slot_groups_btn->setStyleSheet(SS_ListViewButton);
    //preset_input_text_btn->setStyleSheet(SS_SwitchButton);
    preset_input_text_edit_btn->setStyleSheet(SS_ListViewButton);
    touch_keyboard_show_btn->setStyleSheet(SS_ComboBox);
    remote_scan_send_find_btn->setStyleSheet(SS_ListViewButton);

    //language_panel
    language_cbb->setStyleSheet(SS_ComboBox);
    language_cbb->setView(new QListView());

    //typein_panel

    //create_panel
    create_max_repeat_spb->setStyleSheet(SS_SpinBoxWithLittleArrow);
    create_repeat_mode_cbb->setStyleSheet(SS_ComboBox);
    create_repeat_mode_cbb->setView(new QListView());
    text_align_cbb->setStyleSheet(SS_ComboBox);
    text_align_cbb->setView(new QListView());
    slot_switch_direct_cbb->setStyleSheet(SS_ComboBox);
    slot_switch_direct_cbb->setView(new QListView());
    auto_del_expired_tasks_cbb->setStyleSheet(SS_ComboBox);
    auto_del_expired_tasks_cbb->setView(new QListView());
    laser_preheat_time_cbb->setStyleSheet(SS_ComboBox);
    laser_preheat_time_cbb->setView(new QListView());
    laser_auto_sleep_cbb->setStyleSheet(SS_ComboBox);
    laser_auto_sleep_cbb->setView(new QListView());
    gas_treatment_auto_sleep_cbb->setStyleSheet(SS_ComboBox);
    gas_treatment_auto_sleep_cbb->setView(new QListView());
    gas_treatment_speed_level_cbb->setStyleSheet(SS_ComboBox);
    gas_treatment_speed_level_cbb->setView(new QListView());

    maintenance_remind_cbb->setStyleSheet(SS_ComboBox);
    maintenance_remind_cbb->setView(new QListView());

    //list_panel
    task_list_show_mode_cbb->setStyleSheet(SS_ComboBox);
    task_list_show_mode_cbb->setView(new QListView());
    print_list_show_mode_cbb->setStyleSheet(SS_ComboBox);
    print_list_show_mode_cbb->setView(new QListView());
    list_default_show_cbb->setStyleSheet(SS_ComboBox);
    list_default_show_cbb->setView(new QListView());

    //print_panel
    main_num_cb->setStyleSheet(SS_CheckBox);
    sub_num_cb->setStyleSheet(SS_CheckBox);

    //preset_info_panel
    preset_input_list_view_btn->setStyleSheet(SS_ListViewButton);

    //remote_text_panel

    //remote_image_panel

    //sync_print_panel
    sync_print_search_addr_btn->setStyleSheet(SS_ListViewButton);

    //bottom_bar
    reset_btn->setStyleSheet(SS_Normal_Menu_Btn);
    edit_btn->setStyleSheet(SS_Primary_Menu_Btn);
    save_btn->setStyleSheet(SS_Primary_Menu_Btn);
    cancel_btn->setStyleSheet(SS_Normal_Menu_Btn);
}

void SettingDialog::connect_signals_and_slots()
{
    //所有QLineEdit 安装触摸键盘信号
    typein_connector_le->installEventFilter(this);
    typein_delimiter_le->installEventFilter(this);
    sub_number_merge_symbol_le->installEventFilter(this);
    //typein_subnum_front_identifier_le->installEventFilter(this);

    task_list_interval_symbol_le->installEventFilter(this);
    print_list_interval_symbol_le->installEventFilter(this);
    scan_delimiter_le->installEventFilter(this);
    preset_text_length_le->installEventFilter(this);
    //remote_network_name_le->installEventFilter(this);
    print_front_symbol_le->installEventFilter(this);
    print_delimiter_le->installEventFilter(this);
    print_tail_symbol_le->installEventFilter(this);
    slots_front_symbol_le->installEventFilter(this);
    slots_delimiter_le->installEventFilter(this);
    slots_tail_symbol_le->installEventFilter(this);

    remote_network_name_le->installEventFilter(this);
    remote_text_network_port_le->installEventFilter(this);
    // 为整个remote_text_panel安装事件过滤器，确保面板内所有控件都能正常响应
//    remote_text_panel->installEventFilter(this);
    // 为整个sync_print_panel也安装事件过滤器，保持一致性
//    sync_print_panel->installEventFilter(this);

    //注释掉，干扰别的sync_print_network_addr_le触摸键盘弹出
    //remote_image_network_port_le->installEventFilter(this);
    //sync_print_network_port_le->installEventFilter(this);

    sync_print_network_addr_le->installEventFilter(this);
    remote_scan_send_ip_le->installEventFilter(this);
    remote_scan_send_port_le->installEventFilter(this);

    connect(print_front_symbol_le, &QLineEdit::textChanged, this, &SettingDialog::on_remote_symbol_text_changed);
    connect(print_delimiter_le, &QLineEdit::textChanged, this, &SettingDialog::on_remote_symbol_text_changed);
    connect(print_tail_symbol_le, &QLineEdit::textChanged, this, &SettingDialog::on_remote_symbol_text_changed);
    connect(slots_front_symbol_le, &QLineEdit::textChanged, this, &SettingDialog::on_remote_symbol_text_changed);
    connect(slots_delimiter_le, &QLineEdit::textChanged, this, &SettingDialog::on_remote_symbol_text_changed);
    connect(slots_tail_symbol_le, &QLineEdit::textChanged, this, &SettingDialog::on_remote_symbol_text_changed);
    connect(slot_groups_btn,&QPushButton::clicked, this, &SettingDialog::on_slot_groups_btn_clicked);
    connect(preset_input_text_edit_btn, &QPushButton::clicked, this, &SettingDialog::on_preset_input_text_edit_btn_clicked);

    connect(edit_btn, &QPushButton::clicked, this, &SettingDialog::on_edit_btn_clicked);
    connect(save_btn, &QPushButton::clicked, this, &SettingDialog::on_save_btn_clicked);
    connect(cancel_btn, &QPushButton::clicked, this, &SettingDialog::on_cancel_btn_clicked);

    connect(language_cbb, QOverload<const QString&>::of(&QComboBox::currentIndexChanged),this,&SettingDialog::on_language_selector_changed);

    connect(touch_keyboard_show_btn, &QPushButton::clicked,
            this, &SettingDialog::on_touch_keyboard_show_btn_clicked);


    connect(remote_scan_send_find_btn, &QPushButton::clicked, this, [=]{
        DeviceDiscoveryDialog *diviceDiscoveryDialog = new DeviceDiscoveryDialog();

        if(QDialog::Accepted == diviceDiscoveryDialog->exec())
        {
            DeviceInfo info = diviceDiscoveryDialog->getSelectedDeviceInfo();
            if(!info.ip.isEmpty())
            {
                remote_scan_send_ip_le->setText(info.ip);
                remote_scan_send_port_le->setText(QString::number(info.port));
            }
            qDebug() << "Selected device info:" << info.name << info.ip << info.port;
        }
    });

    // 连接恢复出厂设置按钮信号
    connect(reset_btn, &QPushButton::clicked, this, &SettingDialog::on_reset_btn_clicked);

    this->installWheelBlocker(this);
}



QLabel *SettingDialog::text_label(const QString &text)
{
    QLabel *label = new QLabel();
    label->setText(text);
    return label;
}

QWidget *SettingDialog::create_item(const QString &item_name, QWidget *widget_a, QWidget *widget_b, QWidget *widget_c)
{
    QWidget *item = new QWidget();
    QHBoxLayout *item_h_layout = new QHBoxLayout(item);
    item_h_layout->setContentsMargins(10,0,10,0);
    item_h_layout->setSpacing(2);
    item_h_layout->addWidget(text_label(item_name));
    item_h_layout->addStretch();
    if(widget_a !=nullptr)
        item_h_layout->addWidget(widget_a);
    if(widget_b !=nullptr)
        item_h_layout->addWidget(widget_b);
    if(widget_c !=nullptr)
        item_h_layout->addWidget(widget_c);
    return item;
}

QWidget *SettingDialog::create_item(QWidget *widget_a, QWidget *widget_b, QWidget *widget_c, QWidget *widget_d)
{
    QWidget *item = new QWidget();
    QHBoxLayout *item_h_layout = new QHBoxLayout(item);
    item_h_layout->setContentsMargins(10,0,10,0);
    item_h_layout->setSpacing(2);
    if(widget_a !=nullptr)
        item_h_layout->addWidget(widget_a);
    item_h_layout->addStretch();
    if(widget_b !=nullptr)
        item_h_layout->addWidget(widget_b);
    if(widget_c !=nullptr)
        item_h_layout->addWidget(widget_c);
    if(widget_d !=nullptr)
        item_h_layout->addWidget(widget_d);
    return item;
}

QWidget *SettingDialog::create_item(QWidget *widget_a, QWidget *widget_b, Qt::Alignment align)
{
    QWidget *item = new QWidget();
    QHBoxLayout *item_h_layout = new QHBoxLayout(item);
    item_h_layout->setContentsMargins(10,0,10,0);
    item_h_layout->setSpacing(2);
    item_h_layout->setAlignment(align);
    if(widget_a !=nullptr)
        item_h_layout->addWidget(widget_a);
    if(widget_b !=nullptr)
        item_h_layout->addWidget(widget_b);
    return item;
}

QFrame *SettingDialog::seprate_line()
{
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFixedHeight(1);
    line->setStyleSheet(SS_SeparateLine);
    return line;
}

void SettingDialog::on_remote_symbol_text_changed()
{
    QString print_field_sample = QString("%4%1%5%2%5%3%6").arg(tr("字段1"), tr("字段2"), tr("字段3"))
            .arg(print_front_symbol_le->text(), print_delimiter_le->text(), print_tail_symbol_le->text());
    QString slot_field_sample = slots_front_symbol_le->text() + "1" + slots_delimiter_le->text() + "2" + slots_delimiter_le->text() + "3" +slots_tail_symbol_le->text();

    remote_print_text_analyze_lb->setText(tr("内容字段解析：") + print_field_sample);
    remote_slot_text_analyze_lb->setText(tr("槽盒字段解析：") + slot_field_sample);
}

void SettingDialog::on_language_selector_changed()
{

}

void SettingDialog::on_back_btn_clicked()
{
    //如果正在编辑中弹出提示
    if(save_btn->isVisible())
    {
        //失去焦点
        on_cancel_btn_clicked();
        DialogMsgBox *msg_box = new DialogMsgBox(ASK_DIALOG);
        msg_box->setContent(tr("编辑未保存。\n是否确定要直接退出？"));
        if(msg_box->exec()==QDialog::Accepted)
        {
            //隐藏触摸键盘
            TouchKeyboardManager::suppressTouchKeyboard(this);
            TouchKeyboardManager::hide();
            this->close();
        }
    }
    else
    {
        this->close();
    }

}

void SettingDialog::on_edit_btn_clicked()
{
    main_area->setEnabled(true);
    edit_btn->setVisible(false);
    save_btn->setVisible(true);
    cancel_btn->setVisible(true);

    // 让typein_connector_le获得焦点，模拟FocusIn事件
    //这个操作是避免关闭触摸键盘后，远程解析字段不能编辑的问题
    if (typein_connector_le)
    {
        typein_connector_le->setFocus();
    }
}

void SettingDialog::on_save_btn_clicked()
{
    // 保存所有设置（包括槽组状态）
    write_settings_to_file();
    saveSlotGroupsStateToIni();

    main_area->setEnabled(false);
    edit_btn->setVisible(true);
    save_btn->setVisible(false);
    cancel_btn->setVisible(false);

    //等待完成写入操作
    QThread::msleep(100);

    this->close();
}

void SettingDialog::on_cancel_btn_clicked()
{
    //从文件恢复原设置参数
    load_settings_from_file();
    //重新加载槽组状态
    loadSlotGroupsStateFromIni();

    main_area->setEnabled(false);
    edit_btn->setVisible(true);
    save_btn->setVisible(false);
    cancel_btn->setVisible(false);
}

// 从INI文件加载槽组状态到finalState
void SettingDialog::loadSlotGroupsStateFromIni()
{
    QSettings ini(LAST_STATUS_INI, QSettings::IniFormat);

    // 加载hopper颜色
    ini.beginGroup("last_selected_hopper_color_index");
    for (int i = 0; i < BUTTONS_PER_GROUP; i++) {
        slotGroupsEditState.hopperColors[QString("hopper_%1").arg(i+1)] =
            ini.value(QString("hopper_%1").arg(i+1), 0).toInt();
    }
    ini.endGroup();

    // 加载槽位选择状态
    ini.beginGroup("last_selected_slot_groups");
    for (int group = 0; group < MAX_SLOTS; group++) {
        QString groupKey = QString("group_%1").arg(group + 1);
        QString slotsStr = ini.value(groupKey, "0,0,0,0,0,0").toString();

        // 解析字符串格式 "0,0,1,0,0,1"
        QStringList slotsList = slotsStr.split(",");
        QJsonArray selections;

        for (int i = 0; i < BUTTONS_PER_GROUP && i < slotsList.size(); i++) {
            selections.append(slotsList[i].trimmed().toInt());
        }

        // 如果解析的数量不足，用0补齐
        while (selections.size() < BUTTONS_PER_GROUP) {
            selections.append(0);
        }

        slotGroupsEditState.slotSelections[QString("group_%1").arg(group+1)] = selections;
    }
    ini.endGroup();

    // 加载映射符
    ini.beginGroup("last_slot_groups_mappings");
    for (int group = 0; group < MAX_SLOTS; group++) {
        QString groupKey = QString("group_%1").arg(group + 1);
        QString mapping = ini.value(groupKey, "").toString();
        slotGroupsEditState.mappings[QString("group_%1").arg(group+1)] = mapping;
    }
    ini.endGroup();
}

// 保存槽组状态到INI文件
void SettingDialog::saveSlotGroupsStateToIni()
{
    QSettings ini(LAST_STATUS_INI, QSettings::IniFormat);

    // 保存hopper颜色
    ini.beginGroup("last_selected_hopper_color_index");
    for (int i = 0; i < BUTTONS_PER_GROUP; i++) {
        QString key = QString("hopper_%1").arg(i+1);
        if (slotGroupsEditState.hopperColors.contains(key)) {
            ini.setValue(key, slotGroupsEditState.hopperColors[key].toInt());
        }
    }
    ini.endGroup();

    // 保存槽位选择状态
    ini.beginGroup("last_selected_slot_groups");
    for (int group = 0; group < MAX_SLOTS; group++) {
        QString groupKey = QString("group_%1").arg(group + 1);
        if (slotGroupsEditState.slotSelections.contains(groupKey)) {
            QJsonArray selections = slotGroupsEditState.slotSelections[groupKey].toArray();
            QStringList slotsList;

            for (int i = 0; i < selections.size(); i++) {
                slotsList.append(QString::number(selections[i].toInt()));
            }

            // 生成字符串格式 "0,0,1,0,0,1"
            QString slotsStr = slotsList.join(",");
            ini.setValue(groupKey, slotsStr);
        }
    }
    ini.endGroup();

    // 保存映射符
    ini.beginGroup("last_slot_groups_mappings");
    for (int group = 0; group < MAX_SLOTS; group++) {
        QString groupKey = QString("group_%1").arg(group + 1);
        if (slotGroupsEditState.mappings.contains(groupKey)) {
            ini.setValue(groupKey, slotGroupsEditState.mappings[groupKey].toString());
        }
    }
    ini.endGroup();
}

// 获取槽组编辑状态
SlotGroupsEditState SettingDialog::getSlotGroupsEditState() const
{
    return slotGroupsEditState;
}

// 设置槽组编辑状态
void SettingDialog::setSlotGroupsEditState(const SlotGroupsEditState &state)
{
    slotGroupsEditState = state;
}

void SettingDialog::on_slot_groups_btn_clicked()
{
    SlotGroups_Dialog *slotgroups_dialog = new SlotGroups_Dialog(this);

    // 设置初始状态（从finalState加载，而不是从INI文件）
    slotgroups_dialog->setEditState(slotGroupsEditState);

    // 连接状态改变信号
    connect(slotgroups_dialog, &SlotGroups_Dialog::editStateChanged,
            this, &SettingDialog::onSlotGroupsEditStateChanged);

    if (slotgroups_dialog->Exec() == QDialog::Accepted) {
        // 用户点击了保存，获取最终状态并更新到finalState
        SlotGroupsEditState finalState = slotgroups_dialog->getEditState();
        slotGroupsEditState = finalState;

        qDebug() << "Slot groups state updated:" << finalState.toJsonString();
    } else {
        // 用户取消，保持原有状态不变
        qDebug() << "Slot groups edit cancelled";
    }

    slotgroups_dialog->deleteLater();
}

void SettingDialog::onSlotGroupsEditStateChanged(const SlotGroupsEditState &state)
{
    // 处理状态改变
    qDebug() << "Edit state changed:" << state.toJsonString();
}

void SettingDialog::on_preset_input_text_edit_btn_clicked()
{
    DialogMsgBox *msg_box = new DialogMsgBox(TEXT_EDIT_DIALOG);
    msg_box->setTitle(tr("预设信息"));
    msg_box->setTextEditContent(read_preset_input_text_from_file());

    if(msg_box->exec() == QDialog::Accepted)
    {
        write_preset_input_text_to_file(msg_box->getTextEditContent());
    }
}

void SettingDialog::write_preset_input_text_to_file(const QString &text)
{
    QSettings preset_input_ini(PRESET_INPUT_INI, QSettings::IniFormat);
    preset_input_ini.beginGroup("preset_input");
    preset_input_ini.setValue("text", text);
    preset_input_ini.endGroup();
}

QString SettingDialog::read_preset_input_text_from_file()
{
    QString text;
    QSettings preset_input_ini(PRESET_INPUT_INI, QSettings::IniFormat);
    preset_input_ini.beginGroup("preset_input");
    text = preset_input_ini.value("text", "").toString();
    preset_input_ini.endGroup();
    return text;
}

void SettingDialog::write_settings_to_file()
{
    QSettings settings_ini(SETTINGS_INI, QSettings::IniFormat);

    //language_panel
    QString langStr = language_cbb->currentText();
    ChangeTranslator(langStr);
    retranslateUi();

    settings_ini.beginGroup("language_panel");
    settings_ini.setValue("language", langStr);
    settings_ini.endGroup();

    //typein_panel
    settings_ini.beginGroup("typein_panel");
    settings_ini.setValue("typein_connector_le", typein_connector_le->text());
    settings_ini.setValue("typein_delimiter_le", typein_delimiter_le->text());
    settings_ini.setValue("sub_number_merge_symbol_le", sub_number_merge_symbol_le->text());
    settings_ini.setValue("typein_subnum_front_identifier_le", typein_subnum_front_identifier_le->text());
    settings_ini.setValue("preset_input_text_btn", preset_input_text_btn->isChecked());
    settings_ini.setValue("touch_keyboard_show_btn", touch_keyboard_show_btn->isChecked());

    bool touch_keyboard_show = touch_keyboard_show_btn->isChecked();
    //TouchKeyboardController::setEnabled(touch_keyboard_show);
    TouchKeyboardManager::instance().setEnabled(touch_keyboard_show);

    settings_ini.endGroup();

    //create_panel
    settings_ini.beginGroup("create_panel");
    settings_ini.setValue("create_max_repeat_spb", create_max_repeat_spb->text());
    settings_ini.setValue("create_repeat_mode_cbb", create_repeat_mode_cbb->currentText());
    settings_ini.setValue("text_align_cbb", text_align_cbb->currentData());
    settings_ini.setValue("slot_switch_direct_cbb", slot_switch_direct_cbb->currentData());
    settings_ini.setValue("auto_del_expired_tasks_cbb", auto_del_expired_tasks_cbb->currentData());
    settings_ini.setValue("laser_preheat_time_cbb", laser_preheat_time_cbb->currentData());
    settings_ini.setValue("laser_auto_sleep_cbb", laser_auto_sleep_cbb->currentData());
    settings_ini.setValue("gas_treatment_auto_sleep_cbb", gas_treatment_auto_sleep_cbb->currentData());
    settings_ini.setValue("gas_treatment_speed_level_cbb", gas_treatment_speed_level_cbb->currentData());
    settings_ini.setValue("maintenance_remind_cbb", maintenance_remind_cbb->currentData());
    settings_ini.setValue("alarm_sound_onoff_btn", alarm_sound_onoff_btn->isChecked());
    settings_ini.setValue("tray_empty_confirm_needed_btn", tray_empty_confirm_needed_btn->isChecked());
    settings_ini.endGroup();

    //list_panel
    settings_ini.beginGroup("list_panel");
    settings_ini.setValue("task_list_interval_symbol_le", task_list_interval_symbol_le->text());
    settings_ini.setValue("print_list_interval_symbol_le", print_list_interval_symbol_le->text());
    settings_ini.setValue("task_list_show_mode_cbb", task_list_show_mode_cbb->currentData());
    settings_ini.setValue("print_list_show_mode_cbb", print_list_show_mode_cbb->currentData());
    settings_ini.setValue("list_default_show_cbb", list_default_show_cbb->currentData());
    settings_ini.endGroup();

    //print_panel
    settings_ini.beginGroup("print_panel");
    settings_ini.setValue("auto_print_after_task_established_btn", auto_print_after_task_established_btn->isChecked());
    settings_ini.setValue("continue_print_after_cleanup_btn", continue_print_after_cleanup_btn->isChecked());
    settings_ini.setValue("allow_duplicate_print_task_btn", allow_duplicate_print_task_btn->isChecked());
    settings_ini.setValue("auto_increase_after_blank_enter_btn", auto_increase_after_blank_enter_btn->isChecked());
    settings_ini.setValue("main_num_cb", main_num_cb->isChecked());
    settings_ini.setValue("sub_num_cb", sub_num_cb->isChecked());
    settings_ini.setValue("continue_num_auto_add_zero_btn", continue_num_auto_add_zero_btn->isChecked());
    settings_ini.setValue("auto_print_after_scan_btn", auto_print_after_scan_btn->isChecked());
    settings_ini.setValue("scan_code_separate_btn", scan_code_separate_btn->isChecked());
    settings_ini.setValue("scan_delimiter_rdb", scan_delimiter_rdb->isChecked());
    settings_ini.setValue("scan_delimiter_le", scan_delimiter_le->text());
    settings_ini.setValue("scan_preset_text_length_rdb", scan_preset_text_length_rdb->isChecked());
    settings_ini.setValue("preset_text_length_le", preset_text_length_le->text());
    settings_ini.endGroup();

    //preset_info_panel
    settings_ini.beginGroup("preset_info_panel");
    settings_ini.setValue("preset_input_enable_btn", preset_input_enable_btn->isChecked());
    settings_ini.endGroup();

    //remote_text_panel
    settings_ini.beginGroup("remote_text_panel");
    settings_ini.setValue("remote_text_print_btn", remote_text_print_btn->isChecked());
    settings_ini.setValue("remote_network_name_le", remote_network_name_le->text());
    settings_ini.setValue("print_front_symbol_le", print_front_symbol_le->text());
    settings_ini.setValue("print_delimiter_le", print_delimiter_le->text());
    settings_ini.setValue("print_tail_symbol_le", print_tail_symbol_le->text());
    settings_ini.setValue("slots_front_symbol_le", slots_front_symbol_le->text());
    settings_ini.setValue("slots_delimiter_le", slots_delimiter_le->text());
    settings_ini.setValue("slots_tail_symbol_le", slots_tail_symbol_le->text());
    settings_ini.setValue("remote_text_network_port_le", remote_text_network_port_le->text());

    settings_ini.setValue("remote_scan_send_enable_btn", remote_scan_send_enable_btn->isChecked());
    settings_ini.setValue("remote_scan_send_ip_le", remote_scan_send_ip_le->text());
    settings_ini.setValue("remote_scan_send_port_le", remote_scan_send_port_le->text());

    settings_ini.endGroup();

    //remote_image_panel
    settings_ini.beginGroup("remote_image_panel");
    settings_ini.setValue("remote_image_print_btn", remote_image_print_btn->isChecked());
    settings_ini.setValue("remote_image_network_port_le", remote_image_network_port_le->text());
    settings_ini.endGroup();

    //sync_print_panel
    settings_ini.beginGroup("sync_print_panel");
    settings_ini.setValue("sync_print_btn", sync_print_btn->isChecked());
    settings_ini.setValue("sync_print_network_rdb", sync_print_network_rdb->isChecked());
    settings_ini.setValue("sync_print_serial_rdb", sync_print_serial_rdb->isChecked());
    settings_ini.setValue("sync_print_network_addr_le", sync_print_network_addr_le->text());
    settings_ini.setValue("sync_print_network_port_le", sync_print_network_port_le->text());
    settings_ini.endGroup();

}

void SettingDialog::on_touch_keyboard_show_btn_clicked()
{
    bool touch_keyboard_show = touch_keyboard_show_btn->isChecked();
    //TouchKeyboardController::setEnabled(touch_keyboard_show);
    TouchKeyboardManager::instance().setEnabled(touch_keyboard_show);
}

/*
void SettingDialog::setTouchKeyboardState(bool enable)
{
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\TabletTip\\1.7",
                       QSettings::NativeFormat);

    settings.setValue("EnableDesktopModeAutoInvoke", enable ? 2 : 0);
    settings.setValue("EnableInDesktopMode", enable ? 2 : 0);
    settings.setValue("KeyboardLayoutPreference", enable ? 2 : 0);
    settings.setValue("TouchKeyboardTapInvoke", enable ? 2 : 0);
    settings.sync();

#ifdef Q_OS_WIN
    //broadcastSettingsChange();
    //restartTabTipProcess();
#endif
}

bool SettingDialog::getTouchKeyboardState()
{
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\TabletTip\\1.7",
                       QSettings::NativeFormat);
    int value = settings.value("TouchKeyboardTapInvoke", 0).toInt();
    qDebug() << "getTouchKeyboardState.value ===" << value;
    return (value == 2);
}

#ifdef Q_OS_WIN

void SettingDialog::broadcastSettingsChange()
{
    // 广播系统设置变更消息
     PostMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Software\\Microsoft\\TabletTip\\1.7");
}

void SettingDialog::restartTabTipProcess()
{
#ifdef Q_OS_WIN
    // 强制结束 TabTip.exe（若存在）
    QProcess::execute("taskkill /IM TabTip.exe /F");

    // 稍作延时确保结束
    Sleep(500);

    // 尝试使用 ShellExecute 自动查找路径启动
    HINSTANCE hInst = ShellExecuteW(
        nullptr,
        L"open",
        L"TabTip.exe",
        nullptr,
        nullptr,
        SW_SHOW
    );

    if ((int)hInst <= 32) {
        // 如果 ShellExecute 启动失败，尝试常见路径
        QStringList possiblePaths = {
            R"(C:\Program Files\Common Files\Microsoft Shared\ink\TabTip.exe)",
            R"(C:\Program Files (x86)\Common Files\Microsoft Shared\ink\TabTip.exe)"
        };

        for (const QString &path : possiblePaths) {
            if (QFile::exists(path)) {
                QProcess::startDetached(path);
                return;
            }
        }
    }
#endif
}

#endif

*/

bool SettingDialog::openTouchKeyboardSettingsDialog()
{
    HINSTANCE result = ShellExecuteW(
        NULL,
        L"open",
        L"ms-settings:typing",
        NULL,
        NULL,
        SW_SHOWNORMAL
    );

    return (int)result > 32;
}

void SettingDialog::load_settings_from_file()
{
    MainSettings settings;
    ReadSettings(&settings);

    QSettings settings_ini(SETTINGS_INI, QSettings::IniFormat);

    //language_panel
    LanguageSetting *language_setting = &settings.language_setting;
    settings_ini.beginGroup("language_panel");
    language_cbb->setCurrentText(language_setting->language_cbb);
    settings_ini.endGroup();

    //typein_panel
    TypeinSetting *typein_setting = &settings.typein_setting;
    settings_ini.beginGroup("typein_panel");
    typein_connector_le->setText(typein_setting->typein_connector_le);
    typein_delimiter_le->setText(typein_setting->typein_delimiter_le);
    sub_number_merge_symbol_le->setText(typein_setting->sub_number_merge_symbol_le);
    typein_subnum_front_identifier_le->setText(typein_setting->typein_subnum_front_identifier_le);
    preset_input_text_btn->setChecked(typein_setting->preset_input_text_btn);
    touch_keyboard_show_btn->setChecked(typein_setting->touch_keyboard_show_btn);

    TouchKeyboardManager::instance().setEnabled(typein_setting->touch_keyboard_show_btn);
    settings_ini.endGroup();

    //create_panel
    CreateSetting *create_setting = &settings.create_setting;
    settings_ini.beginGroup("create_panel");
    create_max_repeat_spb->setValue(create_setting->create_max_repeat_spb);
    create_repeat_mode_cbb->setCurrentText(create_setting->create_repeat_mode_cbb);
    text_align_cbb->setCurrentIndex(text_align_cbb->findData(create_setting->text_align_cbb));
    slot_switch_direct_cbb->setCurrentIndex(slot_switch_direct_cbb->findData(create_setting->slot_switch_direct_cbb));
    auto_del_expired_tasks_cbb->setCurrentIndex(auto_del_expired_tasks_cbb->findData(create_setting->auto_del_expired_tasks_cbb));
    laser_preheat_time_cbb->setCurrentIndex(laser_preheat_time_cbb->findData(create_setting->laser_preheat_time_cbb));
    laser_auto_sleep_cbb->setCurrentIndex(laser_auto_sleep_cbb->findData(create_setting->laser_auto_sleep_cbb));
    gas_treatment_auto_sleep_cbb->setCurrentIndex(gas_treatment_auto_sleep_cbb->findData(create_setting->gas_treatment_auto_sleep_cbb));
    gas_treatment_speed_level_cbb->setCurrentIndex(gas_treatment_speed_level_cbb->findData(create_setting->gas_treatment_speed_level_cbb));
    maintenance_remind_cbb->setCurrentIndex(maintenance_remind_cbb->findData(create_setting->maintenance_remind_cbb));
    alarm_sound_onoff_btn->setChecked(create_setting->alarm_sound_onoff_btn);
    tray_empty_confirm_needed_btn->setChecked(create_setting->tray_empty_confirm_needed_btn);
    settings_ini.endGroup();

    //list_panel
    ListSetting *list_setting = &settings.list_setting;
    settings_ini.beginGroup("list_panel");
    task_list_interval_symbol_le->setText(list_setting->task_list_interval_symbol_le);
    print_list_interval_symbol_le->setText(list_setting->print_list_interval_symbol_le);
    task_list_show_mode_cbb->setCurrentIndex(task_list_show_mode_cbb->findData(list_setting->task_list_show_mode_cbb));
    print_list_show_mode_cbb->setCurrentIndex(print_list_show_mode_cbb->findData(list_setting->print_list_show_mode_cbb));
    list_default_show_cbb->setCurrentIndex(list_default_show_cbb->findData(list_setting->list_default_show_cbb));
    settings_ini.endGroup();

    //print_panel
    PrintSetting *print_setting = &settings.print_setting;
    settings_ini.beginGroup("print_panel");
    auto_print_after_task_established_btn->setChecked(print_setting->auto_print_after_task_established_btn);
    continue_print_after_cleanup_btn->setChecked(print_setting->continue_print_after_cleanup_btn);
    allow_duplicate_print_task_btn->setChecked(print_setting->allow_duplicate_print_task_btn);
    auto_increase_after_blank_enter_btn->setChecked(print_setting->auto_increase_after_blank_enter_btn);
    main_num_cb->setChecked(print_setting->main_num_cb);
    sub_num_cb->setChecked(print_setting->sub_num_cb);
    continue_num_auto_add_zero_btn->setChecked(print_setting->continue_num_auto_add_zero_btn);
    auto_print_after_scan_btn->setChecked(print_setting->auto_print_after_scan_btn);
    scan_code_separate_btn->setChecked(print_setting->scan_code_separate_btn);
    scan_delimiter_rdb->setChecked(print_setting->scan_delimiter_rdb);
    scan_delimiter_le->setText(print_setting->scan_delimiter_le);
    scan_preset_text_length_rdb->setChecked(print_setting->scan_preset_text_length_rdb);
    preset_text_length_le->setText(print_setting->preset_text_length_le);
    settings_ini.endGroup();

    //preset_info_panel
    PresetSetting *preset_setting = &settings.preset_setting;
    settings_ini.beginGroup("preset_info_panel");
    preset_input_enable_btn->setChecked(preset_setting->preset_input_enable_btn);
    settings_ini.endGroup();

    //remote_text_panel
    RemoteTextSetting *remote_text_setting = &settings.remote_text_setting;
    settings_ini.beginGroup("remote_text_panel");
    remote_text_print_btn->setChecked(remote_text_setting->remote_text_print_btn);
    remote_network_name_le->setText(remote_text_setting->remote_network_name_le);
    print_front_symbol_le->setText(remote_text_setting->print_front_symbol_le);
    print_delimiter_le->setText(remote_text_setting->print_delimiter_le);
    print_tail_symbol_le->setText(remote_text_setting->print_tail_symbol_le);
    slots_front_symbol_le->setText(remote_text_setting->slots_front_symbol_le);
    slots_delimiter_le->setText(remote_text_setting->slots_delimiter_le);
    slots_tail_symbol_le->setText(remote_text_setting->slots_tail_symbol_le);
    remote_text_network_port_le->setText(remote_text_setting->remote_text_network_port_le);

    remote_scan_send_enable_btn->setChecked(remote_text_setting->remote_scan_send_enable_btn);
    remote_scan_send_ip_le->setText(remote_text_setting->remote_scan_send_ip_le);
    remote_scan_send_port_le->setText(remote_text_setting->remote_scan_send_port_le);

    settings_ini.endGroup();

    //remote_image_panel
    RemoteImageSetting *remote_image_setting = &settings.remote_image_setting;
    settings_ini.beginGroup("remote_image_panel");
    remote_image_print_btn->setChecked(remote_image_setting->remote_image_print_btn);
    remote_image_network_port_le->setText(remote_image_setting->remote_image_network_port_le);
    settings_ini.endGroup();

    //sync_print_panel
    SyncPrintSetting *sync_print_setting = &settings.sync_print_setting;
    settings_ini.beginGroup("sync_print_panel");
    sync_print_btn->setChecked(sync_print_setting->sync_print_btn);
    sync_print_network_rdb->setChecked(sync_print_setting->sync_print_network_rdb);
    sync_print_serial_rdb->setChecked(sync_print_setting->sync_print_serial_rdb);
    sync_print_network_addr_le->setText(sync_print_setting->sync_print_network_addr_le);
    sync_print_network_port_le->setText(sync_print_setting->sync_print_network_port_le);
    settings_ini.endGroup();
}


bool SettingDialog::ReadSettings(MainSettings *settings)
{
    if(settings == NULL)
        return false;

    QSettings settings_ini(SETTINGS_INI, QSettings::IniFormat);

    //language_panel
    LanguageSetting *language = &settings->language_setting;
    settings_ini.beginGroup("language_panel");
    language->language_cbb = settings_ini.value("language", "English").toString();
    settings_ini.endGroup();

    //typein_panel
    TypeinSetting *typein = &settings->typein_setting;
    settings_ini.beginGroup("typein_panel");
    typein->typein_connector_le = settings_ini.value("typein_connector_le", "-").toString();
    typein->typein_delimiter_le =  settings_ini.value("typein_delimiter_le", ".").toString();
    typein->sub_number_merge_symbol_le =  settings_ini.value("sub_number_merge_symbol_le", ",").toString();
    typein->typein_subnum_front_identifier_le =  settings_ini.value("typein_subnum_front_identifier_le", "").toString();
    typein->preset_input_text_btn = settings_ini.value("preset_input_text_btn", "false").toBool();
    typein->touch_keyboard_show_btn = settings_ini.value("touch_keyboard_show_btn", "false").toBool();
    settings_ini.endGroup();

    //create_panel
    CreateSetting *create = &settings->create_setting;
    settings_ini.beginGroup("create_panel");
    create->create_max_repeat_spb = settings_ini.value("create_max_repeat_spb", 20).toInt();
    create->create_repeat_mode_cbb = settings_ini.value("create_repeat_mode_cbb", "123..123..").toString();
    create->text_align_cbb = settings_ini.value("text_align_cbb", "left").toString();
    create->slot_switch_direct_cbb = settings_ini.value("slot_switch_direct_cbb", "backward").toString();
    create->auto_del_expired_tasks_cbb = settings_ini.value("auto_del_expired_tasks_cbb", 0).toInt();
    create->laser_preheat_time_cbb = settings_ini.value("laser_preheat_time_cbb", 60).toInt();
    create->laser_auto_sleep_cbb = settings_ini.value("laser_auto_sleep_cbb", 3).toInt();
    create->gas_treatment_auto_sleep_cbb = settings_ini.value("gas_treatment_auto_sleep_cbb", 5).toInt();
    create->gas_treatment_speed_level_cbb = settings_ini.value("gas_treatment_speed_level_cbb", 0).toInt();
    create->maintenance_remind_cbb = settings_ini.value("maintenance_remind_cbb", 3000).toInt();
    create->alarm_sound_onoff_btn = settings_ini.value("alarm_sound_onoff_btn",false).toBool();
    create->tray_empty_confirm_needed_btn = settings_ini.value("tray_empty_confirm_needed_btn",false).toBool();
    settings_ini.endGroup();

    //list_panel
    ListSetting *list = &settings->list_setting;
    settings_ini.beginGroup("list_panel");
    list->task_list_interval_symbol_le = settings_ini.value("task_list_interval_symbol_le", "#").toString();
    list->print_list_interval_symbol_le = settings_ini.value("print_list_interval_symbol_le", "#").toString();
    list->task_list_show_mode_cbb = settings_ini.value("task_list_show_mode_cbb", "simple").toString();
    list->print_list_show_mode_cbb = settings_ini.value("print_list_show_mode_cbb", "simple").toString();
    list->list_default_show_cbb = settings_ini.value("list_default_show_cbb", "task").toString();
    settings_ini.endGroup();

    //print_panel
    PrintSetting *print = &settings->print_setting;
    settings_ini.beginGroup("print_panel");
    print->auto_print_after_task_established_btn =  settings_ini.value("auto_print_after_task_established_btn", "true").toBool();
    print->continue_print_after_cleanup_btn = settings_ini.value("continue_print_after_cleanup_btn", "true").toBool();
    print->allow_duplicate_print_task_btn = settings_ini.value("allow_duplicate_print_task_btn", "true").toBool();
    print->auto_increase_after_blank_enter_btn = settings_ini.value("auto_increase_after_blank_enter_btn", "false").toBool();
    print->main_num_cb = settings_ini.value("main_num_cb", "true").toBool();
    print->sub_num_cb = settings_ini.value("sub_num_cb", "true").toBool();
    print->continue_num_auto_add_zero_btn = settings_ini.value("continue_num_auto_add_zero_btn", "true").toBool();
    print->auto_print_after_scan_btn = settings_ini.value("auto_print_after_scan_btn", "false").toBool();
    print->scan_code_separate_btn = settings_ini.value("scan_code_separate_btn", "false").toBool();
    print->scan_delimiter_rdb = settings_ini.value("scan_delimiter_rdb", "true").toBool();
    print->scan_delimiter_le = settings_ini.value("scan_delimiter_le", "%").toString();
    print->scan_preset_text_length_rdb = settings_ini.value("scan_preset_text_length_rdb", "false").toBool();
    print->preset_text_length_le = settings_ini.value("preset_text_length_le", "").toString();
    settings_ini.endGroup();

    //preset_info_panel
    PresetSetting *preset = &settings->preset_setting;
    settings_ini.beginGroup("preset_info_panel");
    preset->preset_input_enable_btn = settings_ini.value("preset_input_enable_btn", "false").toBool();
    settings_ini.endGroup();

    //remote_text_panel
    RemoteTextSetting *remote_text = &settings->remote_text_setting;
    settings_ini.beginGroup("remote_text_panel");
    remote_text->remote_text_print_btn = settings_ini.value("remote_text_print_btn", "false").toBool();
    remote_text->remote_network_name_le = settings_ini.value("remote_network_name_le", "01").toString();
    remote_text->print_front_symbol_le = settings_ini.value("print_front_symbol_le", "").toString();
    remote_text->print_delimiter_le = settings_ini.value("print_delimiter_le", "/").toString();
    remote_text->print_tail_symbol_le = settings_ini.value("print_tail_symbol_le", "").toString();
    remote_text->slots_front_symbol_le = settings_ini.value("slots_front_symbol_le", "[").toString();
    remote_text->slots_delimiter_le = settings_ini.value("slots_delimiter_le", "").toString();
    remote_text->slots_tail_symbol_le = settings_ini.value("slots_tail_symbol_le", "]").toString();
    remote_text->remote_text_network_port_le = settings_ini.value("remote_text_network_port_le", "9997").toString();

    remote_text->remote_scan_send_enable_btn = settings_ini.value("remote_scan_send_enable_btn", "false").toBool();
    remote_text->remote_scan_send_ip_le = settings_ini.value("remote_scan_send_ip_le", "0.0.0.0").toString();
    remote_text->remote_scan_send_port_le = settings_ini.value("remote_scan_send_port_le", "9992").toString();

    remote_text->remote_field_1_le = settings_ini.value("remote_field_1_le", "1#").toString();
    remote_text->remote_field_2_le = settings_ini.value("remote_field_2_le", "2#").toString();
    remote_text->remote_field_3_le = settings_ini.value("remote_field_3_le", "3#").toString();
    remote_text->remote_field_4_le = settings_ini.value("remote_field_4_le", "4#").toString();
    remote_text->remote_field_5_le = settings_ini.value("remote_field_5_le", "5#").toString();
    remote_text->remote_field_6_le = settings_ini.value("remote_field_6_le", "6#").toString();
    remote_text->remote_field_7_le = settings_ini.value("remote_field_7_le", "7#").toString();
    settings_ini.endGroup();

    //remote_image_panel
    RemoteImageSetting *remote_image = &settings->remote_image_setting;
    settings_ini.beginGroup("remote_image_panel");
    remote_image->remote_image_print_btn = settings_ini.value("remote_image_print_btn", "true").toBool();
    remote_image->remote_image_network_port_le = settings_ini.value("remote_image_network_port_le", "01").toString();
    settings_ini.endGroup();

    //sync_print_panel
    SyncPrintSetting *sync_print = &settings->sync_print_setting;
    settings_ini.beginGroup("sync_print_panel");
    sync_print->sync_print_btn = settings_ini.value("sync_print_btn", "false").toBool();
    sync_print->sync_print_network_rdb = settings_ini.value("sync_print_network_rdb", "true").toBool();
    sync_print->sync_print_serial_rdb = settings_ini.value("sync_print_serial_rdb", "false").toBool();
    sync_print->sync_print_network_addr_le = settings_ini.value("sync_print_network_addr_le", "").toString();
    sync_print->sync_print_network_port_le = settings_ini.value("sync_print_network_port_le", "65535").toString();
    settings_ini.endGroup();

    return true;
}

void SettingDialog::retranslateUi()
{

}

 void SettingDialog::ChangeTranslator(const QString langStr)
 {
     static QTranslator *m_Translator = nullptr; // 维护一个全局的翻译器指针，避免内存泄漏
     static QTranslator *m_sysTranslator = nullptr; //解决系统弹窗颜色选择窗的翻译问题

     qDebug() << "ChangeTranslator==" << langStr;

     // 先卸载旧的翻译器
     if (m_Translator)
     {
         qApp->removeTranslator(m_Translator);
         qApp->removeTranslator(m_sysTranslator);
         delete m_Translator;
         delete m_sysTranslator;
         m_Translator = nullptr;
         m_sysTranslator = nullptr;
     }

     // 重新加载翻译文件
     m_Translator = new QTranslator();
     m_sysTranslator = new QTranslator();
     bool loadSuccess = false;

     if(langStr == "简体中文")
     {
         loadSuccess = m_Translator->load(":/qm/Translate_CN.qm");
         m_sysTranslator->load(":/qm/qtbase_zh_CN.qm");
         qApp->installTranslator(m_sysTranslator);
     }
     else if(langStr == "English")
     {
         loadSuccess = m_Translator->load(":/qm/Translate_EN.qm");
     }

     if (loadSuccess && qApp->installTranslator(m_Translator))
     {
         qDebug() << "语言翻译器安装成功!";
         // 强制刷新界面
         //emit languageChanged(); // 发送信号，触发界面更新
     }
     else
     {
         qDebug() << "语言翻译器安装失败!";
     }
 }

 // 获取 LAST_STATUS_INI 里所有映射符
 QStringList SettingDialog::getAllMappings()
 {
     QStringList mappings;

     QSettings ini(LAST_STATUS_INI, QSettings::IniFormat);

     // 读取映射符配置
     ini.beginGroup("last_slot_groups_mappings");
     for (int group = 1; group <= MAX_SLOTS; group++)
     {
         QString groupKey = QString("group_%1").arg(group);
         QString mapping = ini.value(groupKey, "").toString().trimmed();
         if (!mapping.isEmpty())
         {
             mappings.append(mapping);
         }
     }
     ini.endGroup();

     qDebug() << "All mappings found:" << mappings;
     return mappings;
 }


 // 只完全匹配映射符
 QString SettingDialog::findSelectedSlotsByKeyword(const QString &keyword)
 {
     if (keyword.isEmpty()) {
         return "";
     }

     QSettings ini(LAST_STATUS_INI, QSettings::IniFormat);
     QSet<int> selectedSlots;

     // 查找完全匹配的映射符分组
     ini.beginGroup("last_slot_groups_mappings");
     int matchedGroup = -1;

     for (int group = 1; group <= MAX_SLOTS; group++) {
         QString groupKey = QString("group_%1").arg(group);
         QString mapping = ini.value(groupKey, "").toString().trimmed();

         if (mapping == keyword) {
             matchedGroup = group;
             qDebug() << "Exact match found: group" << group << "with mapping:" << mapping;
             break;
         }
     }
     ini.endGroup();

     if (matchedGroup == -1) {
         qDebug() << "No exact match found for mapping:" << keyword;
         return "";
     }

     // 读取该分组的槽位选择状态
     ini.beginGroup("last_selected_slot_groups");
     QString groupKey = QString("group_%1").arg(matchedGroup);
     QString slotsStr = ini.value(groupKey, "0,0,0,0,0,0").toString();
     QStringList slotsList = slotsStr.split(",");
     ini.endGroup();

     // 解析选中状态
     for (int btnIndex = 0; btnIndex < slotsList.size() && btnIndex < BUTTONS_PER_GROUP; btnIndex++) {
         if (slotsList[btnIndex].trimmed() == "1") {
             selectedSlots.insert(btnIndex + 1); // btn + 1
         }
     }

     // 返回结果
     QList<int> sortedSlots = selectedSlots.values();
     std::sort(sortedSlots.begin(), sortedSlots.end());

     QStringList resultList;
     for (int slot : sortedSlots) {
         resultList.append(QString::number(slot));
     }

     return resultList.join(",");
 }

 void SettingDialog::installWheelBlocker(QWidget *root)
 {
     const auto combos = root->findChildren<QComboBox*>();
     for (QComboBox* cb : combos)
     {
         cb->installEventFilter(this);
     }

     const auto spins = root->findChildren<QSpinBox*>();
     for (QSpinBox* sb : spins)
     {
         sb->installEventFilter(this);
     }
 }

 bool SettingDialog::eventFilter(QObject *obj, QEvent *event)
 {
     //禁用鼠标滚轮
     if (event->type() == QEvent::Wheel)
     {
         if (qobject_cast<QComboBox*>(obj) || qobject_cast<QSpinBox*>(obj))
         {
             return true;
         }
     }

     // 处理焦点进入事件
     if(event->type() == QEvent::FocusIn)
     {
         if(QLineEdit *lineEdit = qobject_cast<QLineEdit *>(obj))
         {
             TouchKeyboardManager::instance().handleFocusIn(lineEdit);
         }
         // 无论是什么控件，获得焦点时都确保不被阻塞
         obj->setProperty("blocked", false);
         if(QWidget *widget = qobject_cast<QWidget *>(obj))
         {
             // 确保窗口属性正确设置
             if(TouchKeyboardManager::instance().isEnabled())
             {
                 TouchKeyboardManager::allowAutoInvoke(widget);
             }
             widget->update();
             widget->setEnabled(true); // 确保控件启用
         }
     }

     return QDialog::eventFilter(obj, event);
 }

 void SettingDialog::on_reset_btn_clicked()
{
    // 显示确认对话框
    DialogMsgBox *msg_box = new DialogMsgBox(ASK_DIALOG);
    msg_box->setTitle(tr("提示"));
    msg_box->setContent(tr("此操作将清除用户数据，是否继续？"));
//    msg_box->setAcceptBtnText(tr("是"));
//    msg_box->setCancelBtnText(tr("否"));

    int reply = msg_box->Exec(0, false, nullptr);
    if (reply != QDialog::Accepted)
    {
        return;
    }

    QString appDir = QCoreApplication::applicationDirPath();
    QDir appDirectory(appDir);

    // 默认删除的文件列表
    QStringList defaultFilesToDelete = {
        "task_list.ini",
        "print_list.ini",
        "preset_input.ini",
        "edit_list.ini"
    };

    // 删除默认文件
    foreach (const QString &fileName, defaultFilesToDelete)
    {
        QString filePath = appDir + "/" + fileName;
        if (QFile::exists(filePath))
        {
            QFile::remove(filePath);
            qDebug() << "恢复出厂设置：删除文件" << filePath;
        }
    }

    // 清除打印任务历史
    PrintTaskDataManagement::getInstance().clearPrintTaskHistory();

    // 删除templates/user目录
    QString templatesUserDir = appDir + "/templates/user";
    QDir userDir(templatesUserDir);
    if (userDir.exists())
    {
        // 删除目录及其所有内容
        bool success = userDir.removeRecursively();
        if (success)
        {
            qDebug() << "恢复出厂设置：删除目录" << templatesUserDir;
        }
        else
        {
            qDebug() << "恢复出厂设置：删除目录失败" << templatesUserDir;
        }
    }

    // 读取factory_restore.ini文件中的删除列表
    QString restoreIniPath = appDir + "/factory_restore.ini";

    qDebug() << "factory_restore.ini文件===" << restoreIniPath;
    // 如果文件不存在，则自动创建
    if (!QFile::exists(restoreIniPath))
    {
        qDebug() << "恢复出厂设置：创建factory_restore.ini文件";

        // 创建文件并写入基本结构
        QSettings restoreSettings(restoreIniPath, QSettings::IniFormat);
        restoreSettings.beginGroup("FilesToDelete");
        restoreSettings.setValue("1","task_list.ini");
        restoreSettings.setValue("2","print_list.ini");
        restoreSettings.setValue("3","preset_input.ini");
        restoreSettings.setValue("4","edit_list.ini");
        restoreSettings.endGroup();

        // 同步到文件
//        restoreSettings.sync();
    }
    else
    {
        QSettings restoreSettings(restoreIniPath, QSettings::IniFormat);
        restoreSettings.beginGroup("FilesToDelete");

        // 获取所有需要删除的文件键
        QStringList keys = restoreSettings.allKeys();
        foreach (const QString &key, keys)
        {
            QString fileName = restoreSettings.value(key).toString();
            if (!fileName.isEmpty())
            {
                QString filePath = appDir + "/" + fileName;
                if (QFile::exists(filePath))
                {
                    QFile::remove(filePath);
                    qDebug() << "恢复出厂设置：删除配置文件" << filePath;
                }
            }
        }

        restoreSettings.endGroup();
    }

    // 显示完成信息
    DialogMsgBox *info_box = new DialogMsgBox(INFO_DIALOG);
    info_box->setTitle(tr("提示"));
    info_box->setContent(tr("恢复出厂设置完成！\n请重启程序。"));
    info_box->setAcceptBtnText(tr("确定"));
    info_box->Exec(0, false, nullptr);
    delete info_box;

    emit signale_print_task_exit_request();

    //on_back_btn_clicked();
}


bool SettingDialog::registerForRestart(const QString &commandLine)
{
     HRESULT hr = RegisterApplicationRestart(
                 commandLine.toStdWString().c_str(),
                 RESTART_NO_CRASH | RESTART_NO_HANG | RESTART_NO_PATCH | RESTART_NO_REBOOT
                 );
     return SUCCEEDED(hr);
}

void SettingDialog::changeEvent(QEvent *event)
{
    if (NULL != event)
    {
           switch (event->type())
           {
               // this event is send if a translator is loaded
               case QEvent::LanguageChange:
                   qDebug() << "case QEvent::LanguageChange" << endl;
                   retranslateUi();
                   //changePicLanguage();
                   break;
               default:
                   break;
           }
    }
}
