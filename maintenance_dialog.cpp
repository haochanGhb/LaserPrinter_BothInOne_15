#include "maintenance_dialog.h"
#include "menu_dialog_style.h"

MaintenanceDialog::MaintenanceDialog()
{
    setTitle(tr("调试"));

    create_widget();
    setup_layout();
    set_stylesheet();
    connect_signals_and_slots();

    load_mark_area_args_from_file();
    load_color_level_args_from_file();

}

bool MaintenanceDialog::ReadMarkAreaArgs(int index, MarkAreaArgs *args)
{
    if (index < 0 || index >= 2)
    {
        return false; // 索引越界
    }

    //先读出单槽区域的标刻参数
    float x_offset_le,y_offset_le,x_width_le,y_height_le;
    QSettings settings_ini_single(MARK_AREA_INI, QSettings::IniFormat);
    settings_ini_single.beginGroup(QString("MarkArea"));
    x_offset_le = settings_ini_single.value("markX", "2.0").toFloat();
    y_offset_le = settings_ini_single.value("markY", "-2.0").toFloat();
    x_width_le =  settings_ini_single.value("markWidth", "30.0").toFloat();
    y_height_le = settings_ini_single.value("markHeight", "8.0").toFloat();
    settings_ini_single.endGroup();

    QSettings settings_ini(MARK_AREA_INI, QSettings::IniFormat);
    settings_ini.beginGroup(QString("MarkArea_%1").arg(index));

    // 读取参数并存储到 args
    args->x_offset_le = settings_ini.value("markX", index == 0 ? QString::number(x_offset_le,'f',2) : "2.0").toString().toFloat();
    args->y_offset_le = settings_ini.value("markY", index == 0 ? QString::number(y_offset_le,'f',2) : "-2.0").toString().toFloat();
    args->x_width_le = settings_ini.value("markWidth", index == 0 ? QString::number(x_width_le,'f',2) : "30.0").toString().toFloat();
    args->y_height_le = settings_ini.value("markHeight", index == 0 ? QString::number(y_height_le,'f',2) : "8.0").toString().toFloat();

    settings_ini.endGroup();

    return true;
}

bool MaintenanceDialog::ReadColorMarkLevelArgs(const int colorNum, ColorMarkLevelArgs *args)
{
    QSettings settings_ini(COLOR_MARK_LEVEL_INI, QSettings::IniFormat);

    if(colorNum < 0 && colorNum > MAX_SLOT_MENU_ACTIONS)
    {
        return false;
    }

    settings_ini.beginGroup(QString("Color_%1").arg(colorNum));
    args->color_name = settings_ini.value("ColorName", "White").toString();
    args->color_code = settings_ini.value("ColorCode", "#ffffff").toString();
    args->mark_level = settings_ini.value("MarkLevel", "8").toInt();
    settings_ini.endGroup();

    return true;
}

void MaintenanceDialog::create_widget()
{
    mark_area_panel = new QWidget(this);
    mark_area_panel->setFixedWidth(780);
    color_level_panel = new QWidget(this);
    color_level_panel->setFixedWidth(780);


    mark_param_sel_cbb = new QComboBox();
    mark_param_sel_cbb->setFixedSize(180, 40);
    for(int i=1; i<=2; i++)
    {
        mark_param_sel_cbb->addItem(QString(tr("A") + "%1").arg(i), i);
    }

#ifdef USE_CASSETTE
mark_param_sel_cbb->setEnabled(false);
#endif

    x_offset_le = new QLineEdit(mark_area_panel);
    x_offset_le->setFixedSize(180, 40);
    x_offset_le->installEventFilter(this);

    y_offset_le = new QLineEdit(mark_area_panel);
    y_offset_le->setFixedSize(180, 40);
    y_offset_le->installEventFilter(this);

    x_width_le = new QLineEdit(mark_area_panel);
    x_width_le->setFixedSize(180, 40);
    x_width_le->installEventFilter(this);

    y_height_le = new QLineEdit(mark_area_panel);
    y_height_le->setFixedSize(180, 40);
    y_height_le->installEventFilter(this);

    for (int i=0; i<MAX_SLOT_MENU_ACTIONS; i++)
    {
        QLineEdit *color_name_le = new QLineEdit();
        color_name_le->setFixedSize(85, 40);
        color_name_les.append(color_name_le);
        color_name_le->installEventFilter(this);

        QPushButton *color_btn = new QPushButton();
        color_btn->setFixedSize(60,35);
        color_btns.append(color_btn);
        //槽盒颜色设置：前6种颜色默认不可编辑设置
        if(i > 5)
        {
            connect(color_btn, &QPushButton::clicked, this, &MaintenanceDialog::on_color_btns_clicked);
        }

        QLabel *color_code_lb = new QLabel();
        color_code_lb->setFixedSize(80, 40);
        color_code_lbs.append(color_code_lb);
        color_code_lb->hide();

        QComboBox *color_print_level_cbb = new QComboBox();
        color_print_level_cbb->setFixedSize(120, 40);
        color_print_level_cbbs.append(color_print_level_cbb);
        for(int i=1; i<=10; i++)
        {
            color_print_level_cbb->addItem(QString(tr("等级%1")).arg(i), i);
        }
    }

    //底部按钮
    reset_btn = new QPushButton();
    reset_btn->setFixedSize(160,40);
    reset_btn->setText(tr("恢复出厂设置"));
    //reset_btn->hide();
    edit_btn = new QPushButton();
    edit_btn->setFixedSize(120,40);
    edit_btn->setText(tr("编辑"));
    save_btn = new QPushButton();
    save_btn->setFixedSize(120,40);
    save_btn->setText(tr("保存"));
    cancel_btn = new QPushButton();
    cancel_btn->setFixedSize(120,40);
    cancel_btn->setText(tr("取消"));

    main_area->setEnabled(false);
    //edit_btn->setVisible(true);
    save_btn->setVisible(false);
    cancel_btn->setVisible(false);

}

void MaintenanceDialog::setup_layout()
{
    QVBoxLayout *laser_adjust_v_layout = new QVBoxLayout(mark_area_panel);
    laser_adjust_v_layout->setContentsMargins(5,5,5,5);
    laser_adjust_v_layout->setSpacing(5);
    laser_adjust_v_layout->addWidget(create_item(tr("打印区域"),mark_param_sel_cbb));
    laser_adjust_v_layout->addWidget(seprate_line());
    laser_adjust_v_layout->addWidget(create_item(tr("x轴偏移量："), x_offset_le));
    laser_adjust_v_layout->addWidget(seprate_line());
    laser_adjust_v_layout->addWidget(create_item(tr("y轴偏移量："), y_offset_le));
    laser_adjust_v_layout->addWidget(seprate_line());
    laser_adjust_v_layout->addWidget(create_item(tr("x轴宽度"), x_width_le));
    laser_adjust_v_layout->addWidget(seprate_line());
    laser_adjust_v_layout->addWidget(create_item(tr("y轴高度"), y_height_le));


    QVBoxLayout *print_preset_v_layout = new QVBoxLayout(color_level_panel);
    print_preset_v_layout->setContentsMargins(5,5,5,5);
    print_preset_v_layout->setSpacing(5);
    print_preset_v_layout->addWidget(create_item(tr("槽盒颜色-打印等级")));
    print_preset_v_layout->addWidget(seprate_line());
    for (int i=0; i<MAX_SLOT_MENU_ACTIONS; i++)
    {
        print_preset_v_layout->addWidget(create_item(QString(tr("预设颜色%1：").arg(i+1)), color_name_les[i], color_btns[i],  color_print_level_cbbs[i]));
        print_preset_v_layout->addWidget(seprate_line());
    }

    main_layout->addWidget(mark_area_panel, 0, Qt::AlignHCenter);
    main_layout->addWidget(color_level_panel, 0, Qt::AlignHCenter);
    main_layout->addStretch();

    //bottom_bar
    bottom_bar_layout->addWidget(reset_btn, 0, Qt::AlignVCenter);
    bottom_bar_layout->addStretch();
    bottom_bar_layout->addWidget(edit_btn, 0, Qt::AlignVCenter);
    bottom_bar_layout->addWidget(save_btn, 0, Qt::AlignVCenter);
    bottom_bar_layout->addWidget(cancel_btn, 0, Qt::AlignVCenter);
}

void MaintenanceDialog::set_stylesheet()
{

    //panels
    mark_area_panel->setStyleSheet(SS_MaintenanceItemPanels);
    color_level_panel->setStyleSheet(SS_MaintenanceItemPanels);

    mark_param_sel_cbb ->setStyleSheet(SS_ComboBox);
    mark_param_sel_cbb->setView(new QListView());
    for (int i=0; i<MAX_SLOT_MENU_ACTIONS; i++)
    {
        color_btns[i]->setStyleSheet(SS_Color_Btn);
        color_print_level_cbbs[i]->setStyleSheet(SS_ComboBox);
        color_print_level_cbbs[i]->setView(new QListView());
    }

    //bottom_bar
    reset_btn->setStyleSheet(SS_Normal_Menu_Btn);
    edit_btn->setStyleSheet(SS_Primary_Menu_Btn);
    save_btn->setStyleSheet(SS_Primary_Menu_Btn);
    cancel_btn->setStyleSheet(SS_Normal_Menu_Btn);
}

void MaintenanceDialog::connect_signals_and_slots()
{
    connect(edit_btn, &QPushButton::clicked, this, &MaintenanceDialog::on_edit_btn_clicked);
    connect(save_btn, &QPushButton::clicked, this, &MaintenanceDialog::on_save_btn_clicked);
    connect(cancel_btn, &QPushButton::clicked, this, &MaintenanceDialog::on_cancel_btn_clicked);
    connect(mark_param_sel_cbb, QOverload<const QString&>::of(&QComboBox::currentIndexChanged),this,&MaintenanceDialog::on_mark_param_changed);

    // 连接恢复出厂设置按钮信号
    connect(reset_btn, &QPushButton::clicked, this, &MaintenanceDialog::on_reset_btn_clicked);
}

void MaintenanceDialog::on_mark_param_changed()
{
    int current_index = mark_param_sel_cbb->currentIndex();
    if (current_index >= 0 && current_index < 2) {
        update_ui_from_args(current_index);
    }
}

void MaintenanceDialog::write_mark_area_args_to_file()
{
    QSettings settings_ini(MARK_AREA_INI, QSettings::IniFormat);
//    settings_ini.beginGroup("MarkArea");
//    settings_ini.setValue("markX", x_offset_le->text());
//    settings_ini.setValue("markY", y_offset_le->text());
//    settings_ini.setValue("markWidth", x_width_le->text());
//    settings_ini.setValue("markHeight", y_height_le->text());
//    settings_ini.endGroup();

    // 将当前编辑的参数保存到 edit_args 临时变量
    int current_index = mark_param_sel_cbb->currentIndex();
    if (current_index >= 0 && current_index < 2) {
        edit_args[current_index].x_offset_le = x_offset_le->text().toFloat();
        edit_args[current_index].y_offset_le = y_offset_le->text().toFloat();
        edit_args[current_index].x_width_le = x_width_le->text().toFloat();
        edit_args[current_index].y_height_le = y_height_le->text().toFloat();
    }

    // 将 edit_args 全部写入文件
    for (int i = 0; i < 2; i++) {
        settings_ini.beginGroup(QString("MarkArea_%1").arg(i));
        settings_ini.setValue("markX", QString::number(edit_args[i].x_offset_le, 'f', 2));  // 保留2位小数
        settings_ini.setValue("markY", QString::number(edit_args[i].y_offset_le, 'f', 2));
        settings_ini.setValue("markWidth", QString::number(edit_args[i].x_width_le, 'f', 2));
        settings_ini.setValue("markHeight", QString::number(edit_args[i].y_height_le, 'f', 2));
        settings_ini.endGroup();
    }

}

void MaintenanceDialog::load_mark_area_args_from_file()
{
    //先读出单槽区域的标刻参数
    float x_offset_le,y_offset_le,x_width_le,y_height_le;
    QSettings settings_ini_single(MARK_AREA_INI, QSettings::IniFormat);
    settings_ini_single.beginGroup(QString("MarkArea"));
    x_offset_le = settings_ini_single.value("markX", "2.0").toFloat();
    y_offset_le = settings_ini_single.value("markY", "-2.0").toFloat();
    x_width_le =  settings_ini_single.value("markWidth", "30.0").toFloat();
    y_height_le = settings_ini_single.value("markHeight", "8.0").toFloat();
    settings_ini_single.endGroup();

    QSettings settings_ini(MARK_AREA_INI, QSettings::IniFormat);
    // 加载 [MarkArea_0] 和 [MarkArea_1] 两组参数到 edit_args
    for (int i = 0; i < 2; i++) {
        settings_ini.beginGroup(QString("MarkArea_%1").arg(i));
        edit_args[i].x_offset_le = settings_ini.value("markX", i == 0 ? QString::number(x_offset_le,'f',2)  : "2.0").toFloat();
        edit_args[i].y_offset_le = settings_ini.value("markY", i == 0 ? QString::number(y_offset_le,'f',2) : "-2.0").toFloat();
        edit_args[i].x_width_le = settings_ini.value("markWidth", i == 0 ? QString::number(x_width_le,'f',2) : "30.0").toFloat();
        edit_args[i].y_height_le = settings_ini.value("markHeight", i == 0 ? QString::number(y_height_le,'f',2) : "8.0").toFloat();
        settings_ini.endGroup();
    }

    // 默认显示第一组参数（MarkArea_0）
    update_ui_from_args(mark_param_sel_cbb->currentIndex());

}

// 辅助函数：根据当前选择的参数索引更新UI
void MaintenanceDialog::update_ui_from_args(int index) {
    if (index < 0 || index >= 2) return;
    x_offset_le->setText(QString::number(edit_args[index].x_offset_le));
    y_offset_le->setText(QString::number(edit_args[index].y_offset_le));
    x_width_le->setText(QString::number(edit_args[index].x_width_le));
    y_height_le->setText(QString::number(edit_args[index].y_height_le));
}

void MaintenanceDialog::write_color_level_args_to_file()
{
    QSettings settings_ini(COLOR_MARK_LEVEL_INI, QSettings::IniFormat);

    for (int i=0; i<MAX_SLOT_MENU_ACTIONS; i++)
    {
        settings_ini.beginGroup(QString("Color_%1").arg(i+1));
        //前六种槽盒颜色不可保存
        if(i>5)
        {
            settings_ini.setValue("ColorName", color_name_les[i]->text());
        }
        settings_ini.setValue("ColorCode", color_code_lbs[i]->text());
        settings_ini.setValue("MarkLevel", color_print_level_cbbs[i]->currentData());
        settings_ini.endGroup();
    }
}


void MaintenanceDialog::load_color_level_args_from_file()
{
    for (int i=0; i<MAX_SLOT_MENU_ACTIONS; i++)
    {
        ColorMarkLevelArgs args;
        ReadColorMarkLevelArgs(i+1, &args);

        color_name_les[i]->setText(args.color_name);
        color_btns[i]->setStyleSheet(QString("background-color:%1;"
                                           "border:1px solid #bcbcbc;"
                                           "border-radius: 0px;"
                                           "font-size:18px;"
                                           "font-family:'微软雅黑';"
                                           ).arg(args.color_code));
        color_code_lbs[i]->setText(args.color_code);
        color_print_level_cbbs[i]->setCurrentIndex(color_print_level_cbbs[i]->findData(args.mark_level));

        //前六种槽盒颜色不可编辑，要国际化翻译，白、黄、绿、蓝、紫、粉
        QStringList defaultcolorNames,defaultcolorTranslateNames;
        defaultcolorNames << "白色" << "黄色" << "绿色" << "蓝色" << "紫色" << "粉色";
        defaultcolorTranslateNames << tr("白色") << tr("黄色") << tr("绿色") << tr("蓝色") << tr("紫色") << tr("粉色");

        int index = defaultcolorNames.indexOf(args.color_name);
        if(index >= 0)
        {
            color_name_les[i]->setText(defaultcolorTranslateNames[index]);
        }
    }
}

void MaintenanceDialog::color_name_les_enable(bool isEnable)
{
    for (int i=0; i<MAX_SLOT_MENU_ACTIONS; i++)
    {
       color_name_les[i]->setEnabled(false);

       if(isEnable && i>5)
       {
           color_name_les[i]->setEnabled(true);
       }
    }
}

QWidget *MaintenanceDialog::create_item(const QString &item_name, QWidget *widget_a, QWidget *widget_b, QWidget *widget_c)
{
    QLabel *label = new QLabel();
    label->setText(item_name);
    label->setFixedHeight(40);
    QWidget *item = new QWidget();
    QHBoxLayout *item_h_layout = new QHBoxLayout(item);
    item_h_layout->setContentsMargins(0,0,0,0);
    item_h_layout->setSpacing(2);
    item_h_layout->addWidget(label);
    item_h_layout->addStretch();
    if(widget_a !=nullptr)
        item_h_layout->addWidget(widget_a);
    if(widget_b !=nullptr)
        item_h_layout->addWidget(widget_b);
    if(widget_c !=nullptr)
        item_h_layout->addWidget(widget_c);
    return item;

}

QWidget *MaintenanceDialog::create_item(QWidget *widget_a, QWidget *widget_b, QWidget *widget_c, QWidget *widget_d)
{
    QWidget *item = new QWidget();
    QHBoxLayout *item_h_layout = new QHBoxLayout(item);
    item_h_layout->setContentsMargins(0,0,0,0);
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

QFrame *MaintenanceDialog::seprate_line()
{
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFixedHeight(1);
    line->setStyleSheet(SS_SeparateLine);
    return line;
}

void MaintenanceDialog::on_reset_btn_clicked()
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

bool MaintenanceDialog::eventFilter(QObject *obj, QEvent *ev)
{
    // 处理焦点进入事件，支持触摸键盘
    if(ev->type() == QEvent::FocusIn)
    {
        // 对于QLineEdit，调用TouchKeyboardManager处理焦点进入
        if(QLineEdit *lineEdit = qobject_cast<QLineEdit *>(obj))
        {
            TouchKeyboardManager::instance().handleFocusIn(lineEdit);
        }

        // 无论是什么控件，获得焦点时都确保不被阻塞
        obj->setProperty("blocked", false);
        if(QWidget *widget = qobject_cast<QWidget *>(obj))
        {
            // 确保窗口属性正确设置
//            if(TouchKeyboardManager::isEnabled())
//            {
//                TouchKeyboardManager::allowAutoInvoke(widget);
//            }
            widget->update();
            widget->setEnabled(true); // 确保控件启用
        }
    }

    // 处理焦点离开事件，关闭触摸键盘
    else if(ev->type() == QEvent::FocusOut)
    {
//        TouchKeyboardManager::hide();
        // 确保控件不被阻塞
//        obj->setProperty("blocked", false);
//        if(QWidget *widget = qobject_cast<QWidget *>(obj))
//        {
//            widget->update();
//            widget->setEnabled(true);
//        }

        if(QLineEdit *lineEdit = qobject_cast<QLineEdit *>(obj))
        {
            QApplication::setOverrideCursor(Qt::ArrowCursor);
        }
    }

    return QDialog::eventFilter(obj, ev);
}

void MaintenanceDialog::on_color_btns_clicked()
{
    QPushButton *clicked_btn = qobject_cast<QPushButton *>(sender());

    if(!clicked_btn) return;

    int color_btn_index = color_btns.indexOf(clicked_btn);
    QString color_code = color_code_lbs[color_btn_index]->text();

    if(color_code == "")
    {
        color_code = "#ffffff";
    }

    QColorDialog color_dialog;
    color_dialog.setOption(QColorDialog::DontUseNativeDialog);
    QColor color = color_dialog.getColor(color_code, nullptr, tr("选择颜色"));

    if(color.isValid())
    {
        clicked_btn->setStyleSheet(QString("background-color:%1;"
                                           "border:1px solid #bcbcbc;"
                                           "border-radius: 0px;"
                                           "font-size:18px;"
                                           "font-family:'微软雅黑';"
                                           ).arg(color.name()));

        color_code_lbs[color_btn_index]->setText(color.name());
    }

}
void MaintenanceDialog::on_back_btn_clicked()
{
    //推出前把编辑框焦点转移到主窗体，关闭触摸键盘
    this->setFocus();
    //如果正在编辑中弹出提示
    if(save_btn->isVisible())
    {
        DialogMsgBox *msg_box = new DialogMsgBox(ASK_DIALOG);
        msg_box->setContent(tr("编辑未保存。\n是否确定要直接退出？"));
        if(msg_box->exec()==QDialog::Accepted)
        {
            this->close();
        }
    }
    else
    {
        this->close();
    }
}

void MaintenanceDialog::on_edit_btn_clicked()
{
    is_editing = true;
    main_area->setEnabled(true);
    color_name_les_enable(true);
    edit_btn->setVisible(false);
    save_btn->setVisible(true);
    cancel_btn->setVisible(true);
}

void MaintenanceDialog::on_save_btn_clicked()
{
    write_mark_area_args_to_file();
    write_color_level_args_to_file();
    main_area->setEnabled(false);
    color_name_les_enable(false);
    edit_btn->setVisible(true);
    save_btn->setVisible(false);
    cancel_btn->setVisible(false);
}

void MaintenanceDialog::on_cancel_btn_clicked()
{
    //从文件恢复原设置参数
    //load_settings_from_file();
//    main_area->setEnabled(false);
//    edit_btn->setVisible(true);
//    save_btn->setVisible(false);
//    cancel_btn->setVisible(false);
    //this->close();

    if (is_editing) {
        // 取消编辑时恢复文件中的原始数据
        load_mark_area_args_from_file();
        is_editing = false;
    }
    main_area->setEnabled(false);
    edit_btn->setVisible(true);
    save_btn->setVisible(false);
    cancel_btn->setVisible(false);
}
