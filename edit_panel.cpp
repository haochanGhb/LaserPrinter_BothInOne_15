#include "edit_panel.h"


QStringList processBarcodeList(QBarcode::BarCodeType type, const QStringList &bcList)
{
    QStringList result = bcList;

    switch (type) {
    case QBarcode::EAN_8:
    case QBarcode::EAN_13:
    case QBarcode::ITF25:
        // 只保留数字
        result[0].remove(QRegularExpression("[^0-9]"));
        if (result[0].isEmpty())
        {
            result[0] = "0";
        }
        break;

    case QBarcode::Code_39:
    case QBarcode::Code_93:
    case QBarcode::EAN_128C:
    case QBarcode::Code_128A:
    case QBarcode::Code_128B:
    case QBarcode::Code_128C:
        // 只保留数字和字母
        result[0].remove(QRegularExpression("[^0-9A-Za-z]"));
        if (result[0].isEmpty())
        {
            result[0] = "0";
        }
        break;

    default:
        // QR_Code, Data_Matrix 等其他类型不做处理
        break;
    }

    return result;
}

EditPanel::EditPanel(QWidget *parent) : QWidget(parent)
{
    create_widget_v2();
    setup_layout_v2();
    set_stylesheet();
    connect_signals_and_slots();
    set_last_selected_slots();
    set_last_selected_slot_attr_btn_color();

    QTimer::singleShot(1, [=]()
    {
        setFirstInputBoxFocus();
    });
}

void EditPanel::retranslateUi()
{
    create_btn->setText(QString("%1(&+)").arg(tr("创建")));

    create_task->setText(tr("创建任务"));
    create_task_and_print->setText(tr("创建任务并打印"));
    create_task_and_print_instant->setText(tr("创建任务并打印(插队)"));
    clear_all_input_box->setText(tr("清空录入框"));

    create_task_from_file->setText(tr("从文件创建任务"));

    retract_box_btn->setText(QString("%1").arg(tr("托盘进")));
    expand_box_btn->setText(QString("%1").arg(tr("托盘出")));

    slot_title->setText(tr("槽盒："));
    repeat_title->setText(tr("重复次数："));

//    for (int i=0; i<MAX_INPUT_PANEL; i++)
//    {
//       input_panel[i]->refreshPropertyText();
//    }

    template_title->setText(tr("模板："));
    template_edit_btn->setText(tr("编辑"));

    updateSlotColorMenuArgs();

    m_pKeyboard->setKeyboardAlias(SimpleKeyboard::DigitKeyboard, tr("数字键盘"));
    m_pKeyboard->setKeyboardAlias(SimpleKeyboard::FullKeyboard, tr("全键盘"));
    updateMarkPreviewTextAlign();

}

void EditPanel::changeEvent(QEvent *event)
{
    if (NULL != event)
    {
           switch (event->type())
           {
               // this event is send if a translator is loaded
               case QEvent::LanguageChange:
                   qDebug() << "case QEvent::LanguageChange" << endl;
                   retranslateUi();
                   break;
               default:
                   break;
           }
    }
}

void EditPanel::setInputPanelName(const int panelId, const QString &panelName)
{
    input_panel[panelId]->setPanelName(panelName);
}

void EditPanel::setInputPanelVisible(const int panelId, bool isVisible)
{
    input_panel[panelId]->setProperty("enabledByTemplate", isVisible);
    input_panel[panelId]->setVisible(isVisible);
}

QString EditPanel::GetCurrentTemplateName()
{
    return template_selector->currentText();
}

int EditPanel::GetRepeatTimes()
{
    return repeat_selector->currentText().toInt();
}

qulonglong EditPanel::GetTextListCount()
{
    //获取输入数量
    qulonglong total_cnt = 1;
    qulonglong text_cnt;
    for (int i=0; i<MAX_INPUT_PANEL; i++)
    {
        text_cnt = input_panel[i]->getTextCount();

        if(text_cnt > 0)
        {
            total_cnt *= text_cnt;
        }
    }
    qDebug() << "Total text cnt:" << total_cnt;

    return total_cnt;
}

QStringList EditPanel::GetTextList()
{
    QStringList text_list;
    QList<QStringList> all_panel_text_lists;
    for (int i=0; i<MAX_INPUT_PANEL; i++)
    {
        all_panel_text_lists.append(input_panel[i]->getText());
    }

    //排列组合（分隔符结尾）
    GenerateCombinations(all_panel_text_lists, text_list);

    for(int i=0; i< text_list.size();i++)
    {

        //TEXT,TEMPLATE,SLOTS
        text_list[i].append(template_selector->currentText());
        text_list[i].append(SPLIT_SYMBOL);
        text_list[i].append(GetSelectSlots());

    }
    return text_list;
}

bool  EditPanel::GetAlternatingPrint()
{
    if(print_order_button_group->checkedId() == 1)
    {
        //qDebug() << "交替打印被选中";
        return true;
    }
    return false;
}

void EditPanel::clearInputPanelsFocus()
{
    for(int i=0; i< MAX_INPUT_PANEL-1;i++)
    {
        input_panel[i]->clearFocus();
    }
}

QImage EditPanel::createMarkPicture(QList<QLabel*> list,const TemplateAttr  & temp_attr,const QStringList &bcList)
{

     QImage image = m_pMarkPreview->createMarkPicture(
                  list
                 ,QBarcode::BarCodeType(temp_attr.code_attr.type.toInt())
                 ,QPoint(temp_attr.code_attr.pos_x * 2, temp_attr.code_attr.pos_y * 2)
                 ,QSize(temp_attr.code_attr.width * 2,temp_attr.code_attr.height * 2)
                 ,bcList
                 ,temp_attr.code_attr.separator_ischecked?temp_attr.code_attr.separator:""
                 );
     return image;
}

BarcodeInfo EditPanel::makeMarkBarCodeInfo(const TemplateAttr  & temp_attr,const QStringList &bcList)
{
   BarcodeInfo bcInfo;
   bcInfo.bcType = QBarcode::BarCodeType(temp_attr.code_attr.type.toInt());
   bcInfo.bcPos =  QPoint(temp_attr.code_attr.pos_x, temp_attr.code_attr.pos_y);
   bcInfo.bcSize = QSize(temp_attr.code_attr.width,temp_attr.code_attr.height);

   //过滤二维码特殊字符处理
   //QStringList resultBcList ;
   //resultBcList = processBarcodeList(bcInfo.bcType,bcList);
   bcInfo.bcStrList = bcList;
   bcInfo.bcStrSeparator = temp_attr.code_attr.separator_ischecked?temp_attr.code_attr.separator:"";
   return bcInfo;
}

QString EditPanel::GetSelectSlots()
{
    QString slot_num;
    QStringList slot_num_list;
    for (int i=0; i<MAX_SLOTS; i++)
    {
        if(slot_btn[i]->isChecked())
        {
            slot_num_list.append(QString::number(i+1));
        }
    }
    slot_num = slot_num_list.join(",");
    return slot_num;
}

int EditPanel::GetSlotColorMarkLevel(const int slotNumber)
{
    int color_index=0;
    QSettings last_status_ini(LAST_STATUS_INI, QSettings::IniFormat);
    last_status_ini.beginGroup(QString("last_selected_hopper_color_index"));
    color_index = last_status_ini.value(QString("hopper_%1").arg(slotNumber), 0).toInt();
    last_status_ini.endGroup();

    ColorMarkLevelArgs args;
    MaintenanceDialog::ReadColorMarkLevelArgs(color_index+1, &args);

    return args.mark_level;
}

void EditPanel::setTemplateSelector(const QStringList &tempNames)
{
    QString last_selected_name = read_last_selected_template_name();
    template_selector->clear();
    template_selector->addItems(tempNames);
    template_selector->setCurrentText(last_selected_name);
}

void EditPanel::setRepeatSelector(const int maxRepeatTimes)
{
    QString last_selected_times = read_last_selected_repeat_times();
    repeat_selector->clear();
    for (int i=0; i<maxRepeatTimes; i++)
    {
        repeat_selector->addItem(QString("%1").arg(i+1));
    }
    repeat_selector->setCurrentText(last_selected_times);
}

void EditPanel::setLaserInfoText(const QString &title, const QString &sta)
{
    //laser_info->setText(QString("%1%2").arg(title, sta));
}

void EditPanel::set_mark_preview_text(int inputId,const TemplateAttr  & temp_attr)
{
    m_pMarkPreview->setTextPosition(inputId, QPoint(current_select_temp_attr.panel_attr[inputId].pos_x, current_select_temp_attr.panel_attr[inputId].pos_y));
    qDebug() << QString("font(%1)=%2").arg(inputId).arg(current_select_temp_attr.panel_attr[inputId].font);
    qDebug() << QString("fontsize(%1)=%2").arg(inputId).arg(current_select_temp_attr.panel_attr[inputId].font_size);
    m_pMarkPreview->setTextFont(inputId, QFont(current_select_temp_attr.panel_attr[inputId].font, current_select_temp_attr.panel_attr[inputId].font_size, QFont::Normal));
    //获取截取长度后的模版文本
    int max_width = current_select_temp_attr.panel_attr[inputId].min_width;
    QString tempStr = current_select_temp_attr.panel_attr[inputId].input_text.left(max_width);
    TempTextAttr tempAttr;
    tempAttr.tempText = tempStr;
    tempAttr.textAlign = m_textAlignment;
    tempAttr.textPos = QPoint(current_select_temp_attr.panel_attr[inputId].pos_x,current_select_temp_attr.panel_attr[inputId].pos_y);
    m_pMarkPreview->setText(inputId, current_select_temp_attr.panel_attr[inputId].input_text,tempAttr);
}

void EditPanel::load_text_to_markPreview(const QStringList & text_list)
{
    for (int i=0; i < MAX_INPUT_PANEL-1; i++)
    {
        qDebug() << QString("text_list[%1]==%2").arg(i).arg(text_list[i]);

        //跳过不可用标签，不加载
        if(current_select_temp_attr.panel_attr[i].sta.compare("hidden")==0)
        {
            continue;
        }

        int max_width = current_select_temp_attr.panel_attr[i].min_width;
        //获取截取长度后的模版文本
        QString tempStr = current_select_temp_attr.panel_attr[i].input_text.left(max_width);
        TempTextAttr tempAttr;
        tempAttr.tempText = tempStr;
        tempAttr.textAlign = m_textAlignment;
        tempAttr.textPos = QPoint(current_select_temp_attr.panel_attr[i].pos_x,current_select_temp_attr.panel_attr[i].pos_y);

        //对隐藏字段的预览要放在可是区域以外，但是要加入二维码里面
        if(current_select_temp_attr.panel_attr[i].sta.compare("invisible")==0)
        {
            tempAttr.textPos = QPoint(-9999,-9999);
        }

        m_pMarkPreview->setText(i, text_list[i].left(max_width),tempAttr);

        //加载固定标签
        if(current_select_temp_attr.panel_attr[i].sta.compare("fixed")==0)
        {
            m_pMarkPreview->setText(i, current_select_temp_attr.panel_attr[i].input_text.left(max_width),tempAttr);
        }

    }

    QString dateformatStr = current_select_temp_attr.panel_attr[MAX_INPUT_PANEL-1].input_text;
    QString datetimeStr= convertDateTime(text_list[TaskTextPos::TASK_TIME],dateformatStr);
    qDebug() << "datetimeStr==================================" << datetimeStr;

    //获取截取长度后的模版文本
    QString tempStr = current_select_temp_attr.panel_attr[MAX_INPUT_PANEL-1].input_text;
    TempTextAttr tempAttr;
    tempAttr.tempText = tempStr;
    tempAttr.textAlign = m_textAlignment;
    tempAttr.textPos = QPoint(current_select_temp_attr.panel_attr[MAX_INPUT_PANEL-1].pos_x,current_select_temp_attr.panel_attr[MAX_INPUT_PANEL-1].pos_y);

    if(current_select_temp_attr.panel_attr[MAX_INPUT_PANEL-1].sta.compare("display")==0)
    {
        m_pMarkPreview->setText(MAX_INPUT_PANEL-1, datetimeStr,tempAttr);
    }
    else
    {
        //这里判断时间是否勾选,如果有勾选时间，位置设置到(-9999,-9999)
        QString text_index = current_select_temp_attr.code_attr.text_index;
        QString time_alias_index = QString("%1").arg(TIME_ALIAS_INDEX);
        qDebug() << "load_text_to_markPreview.text_index======>>>>>>" << text_index;
        qDebug() << "load_text_to_markPreview.time_alias_index======>>>>>>" << time_alias_index;
        if(text_index.contains(time_alias_index))
        {
            m_pMarkPreview->setText(MAX_INPUT_PANEL-1, datetimeStr,tempAttr);
            QPoint textPos(-9999,-9999);
            m_pMarkPreview->setTextPosition(MAX_INPUT_PANEL-1, textPos);
        }
        else
        {
            m_pMarkPreview->setText(MAX_INPUT_PANEL-1, "");
        }
    }

    if(current_select_temp_attr.code_attr.type.toInt() < 0)
    {
        m_pMarkPreview->setBarCodeVisible(false);
    }
    else
    {
        m_pMarkPreview->setBarCodeVisible(true);
    }
}

void EditPanel::load_image_to_markPreview(const QImage & image)
{
     m_pMarkPreview->setPic(image);
}

void EditPanel::load_template_attr_To_markPreview()
{
    //设置Barcode
    m_pMarkPreview->setBarCodeType(QBarcode::BarCodeType(current_select_temp_attr.code_attr.type.toInt()));
    m_pMarkPreview->setBarCodePosition(QPoint(current_select_temp_attr.code_attr.pos_x,current_select_temp_attr.code_attr.pos_y));
    m_pMarkPreview->setBarCodeSize(current_select_temp_attr.code_attr.width, current_select_temp_attr.code_attr.height);

    //设置Barcode_text_index
    QVector<int> vec;

    QString text_index = current_select_temp_attr.code_attr.text_index;

    //条形码在非QR_Code和Data_Matrix情况下，条形码固定附加第一个标签
    if(current_select_temp_attr.code_attr.type.toInt() < 0)
    {
        text_index = "";
    }
    else
    {
        if ((QBarcode::QR_Code != current_select_temp_attr.code_attr.type.toInt())
                && (QBarcode::Data_Matrix != current_select_temp_attr.code_attr.type.toInt()))
        {
            text_index = "0";
        }
    }

    QStringList stringList = text_index.split(';');

    for (const QString& s : stringList) {
        bool ok;
        int num = s.trimmed().toInt(&ok);
        if (ok) {
            vec << num;
        } else {
            qWarning() << "Conversion error for:" << s;
        }
    }
    m_pMarkPreview->setBarCodeTextIndex(vec);

    QString separatorStr = current_select_temp_attr.code_attr.separator;
    //根据二维码配置是否加入分隔符

    if(!current_select_temp_attr.code_attr.separator_ischecked)
    {
        separatorStr = "";
    }

     m_pMarkPreview->setBarCodeTextIndexSeparator(separatorStr);

    //设置Panel 1-7
    for (int i=0; i<MAX_INPUT_PANEL-1; i++)
    {

        //获取截取长度后的模版文本
        QString tempStr = current_select_temp_attr.panel_attr[i].input_text;
        TempTextAttr tempAttr;
        tempAttr.tempText = tempStr;
        tempAttr.textAlign = m_textAlignment;
        tempAttr.textPos = QPoint(current_select_temp_attr.panel_attr[i].pos_x,current_select_temp_attr.panel_attr[i].pos_y);

        m_pMarkPreview->setTextPosition(i, QPoint(current_select_temp_attr.panel_attr[i].pos_x, current_select_temp_attr.panel_attr[i].pos_y));
        m_pMarkPreview->setTextFont(i, QFont(current_select_temp_attr.panel_attr[i].font, current_select_temp_attr.panel_attr[i].font_size, QFont::Normal));
        m_pMarkPreview->setText(i, current_select_temp_attr.panel_attr[i].input_text,tempAttr);

        m_pMarkPreview->setText(i,"");
        //判断是否固定显示状态
        if(current_select_temp_attr.panel_attr[i].sta.compare("fixed")==0)
        {
            m_pMarkPreview->setText(i, current_select_temp_attr.panel_attr[i].input_text,tempAttr);
        }
    }

    //获取截取长度后的模版文本
    QString tempStr = current_select_temp_attr.panel_attr[MAX_INPUT_PANEL-1].input_text;
    TempTextAttr tempAttr;
    tempAttr.tempText = tempStr;
    tempAttr.textAlign = m_textAlignment;
    tempAttr.textPos = QPoint(current_select_temp_attr.panel_attr[MAX_INPUT_PANEL-1].pos_x,current_select_temp_attr.panel_attr[MAX_INPUT_PANEL-1].pos_y);

    //datetime
    m_pMarkPreview->setTextPosition(MAX_INPUT_PANEL-1, QPoint(current_select_temp_attr.panel_attr[MAX_INPUT_PANEL-1].pos_x, current_select_temp_attr.panel_attr[MAX_INPUT_PANEL-1].pos_y));
    m_pMarkPreview->setTextFont(MAX_INPUT_PANEL-1, QFont(current_select_temp_attr.panel_attr[MAX_INPUT_PANEL-1].font, current_select_temp_attr.panel_attr[MAX_INPUT_PANEL-1].font_size, QFont::Normal));
    QString dt_format = current_select_temp_attr.panel_attr[MAX_INPUT_PANEL -1].input_text;
    QString task_datetime = QDateTime::currentDateTime().toString(dt_format);

    if(current_select_temp_attr.panel_attr[MAX_INPUT_PANEL-1].sta.compare("display")==0)
    {
        m_pMarkPreview->setText(MAX_INPUT_PANEL-1, task_datetime,tempAttr);
    }
    else
    {
        //这里判断时间是否勾选,如果有勾选时间，位置设置到(-9999,-9999)
        QString text_index = current_select_temp_attr.code_attr.text_index;
        QString time_alias_index = QString("%1").arg(TIME_ALIAS_INDEX);
        qDebug() << "load_template_attr_To_markPreview.text_index======>>>>>>" << text_index;
        qDebug() << "load_template_attr_To_markPreview.time_alias_index======>>>>>>" << time_alias_index;

        if(text_index.contains(time_alias_index))
        {
            m_pMarkPreview->setText(MAX_INPUT_PANEL-1, task_datetime);
            QPoint textPos(-9999,-9999);
            m_pMarkPreview->setTextPosition(MAX_INPUT_PANEL-1, textPos);
        }
        else
        {
            m_pMarkPreview->setText(MAX_INPUT_PANEL-1, "");
        }
    }

    if(current_select_temp_attr.code_attr.type.toInt() < 0)
    {
        m_pMarkPreview->setBarCodeVisible(false);
    }
    else
    {
        m_pMarkPreview->setBarCodeVisible(true);
    }

    int preview_direct = current_select_temp_attr.code_attr.direct_val;
    m_pMarkPreview->setDirect(MarkPreview::Direct(preview_direct));

    if (MarkPreview::Up == MarkPreview::Direct(preview_direct))
    {
        m_pMarkPreview->move(13, 10);
    }
    else if (MarkPreview::Down == MarkPreview::Direct(preview_direct))
    {
        m_pMarkPreview->move(13, 10);
    }

    #if defined(USE_SLIDE) || defined(USE_SLIDE_DOUBLE)
    else if (MarkPreview::Left == MarkPreview::Direct(preview_direct))
    {
        m_pMarkPreview->move(3, 30);
    }
    else if (MarkPreview::Right == MarkPreview::Direct(preview_direct))
    {
        m_pMarkPreview->move(-18, 30);
    }
    #endif

    set_logo_to_markPreview(current_select_temp_attr);
}

void EditPanel::set_logo_to_markPreview(const TemplateAttr  & temp_attr)
{
    QPoint logoPos(temp_attr.logo_attr.pos_x, temp_attr.logo_attr.pos_y);
    int logo_min_width = 16 , logo_min_height = 16;
    int logo_width, logo_height;
    logo_width = logo_min_width * (1 + (temp_attr.logo_attr.width_factor - 1) * 0.2);
    logo_height = logo_min_height * (1 + (temp_attr.logo_attr.height_factor-1) * 0.2);
    //logo_width = temp_attr.logo_attr.width;
    //logo_height = temp_attr.logo_attr.height;

    m_pMarkPreview->setLogoSize(logo_width, logo_height);
    //QString current_temp_name = template_selector->currentText();
    QString current_temp_name = temp_attr.name;
    QString temp_logo_file = QString("%1/%2/%3")
            .arg(USER_TEMPLATES_DIR)
            .arg(current_temp_name)
            .arg(temp_attr.logo_attr.file_name);
    qDebug() << "set_logo_to_markPreview.temp_logo_file====>>>>" << temp_logo_file;
    m_pMarkPreview->setLogoPixMap(temp_logo_file);

    m_pMarkPreview->setLogoPosition(logoPos);
    m_pMarkPreview->setLogoDisplay(temp_attr.logo_attr.is_display);

}

void EditPanel::setCurrentTempAttr(TemplateAttr &temp_attr)
{
    current_select_temp_attr = temp_attr;

    load_template_attr_To_markPreview();
}

void EditPanel::updateSlotColorMenuArgs()
{
    for (int i=0;i<MAX_SLOTS;i++)
    {
        slot_attr_menu[i]->clear();
        setup_slots_menu_actions(slot_attr_btn[i], slot_attr_menu[i]);
    }
    set_last_selected_slot_attr_btn_color();
}

void EditPanel::updateMarkPreviewTextAlign()
{
    MainSettings settings;
    SettingDialog::ReadSettings(&settings);
    m_textAlignment = settings.create_setting.text_align_cbb;
}

void EditPanel::save_last_selected_template()
{
    //保存最后选择的模板名
    QSettings last_status_ini(LAST_STATUS_INI, QSettings::IniFormat);
    last_status_ini.beginGroup(QString("last_selected_template"));
    last_status_ini.setValue(QString("name"), template_selector->currentText());
    last_status_ini.endGroup();
}

void EditPanel::save_last_selected_repeat()
{
    //保存最后选择的重复次数
    QSettings last_status_ini(LAST_STATUS_INI, QSettings::IniFormat);
    last_status_ini.beginGroup(QString("last_selected_repeat"));
    last_status_ini.setValue(QString("times"), repeat_selector->currentText());
    last_status_ini.endGroup();
}

void EditPanel::save_last_selected_slots()
{
    QSettings last_status_ini(LAST_STATUS_INI, QSettings::IniFormat);
    last_status_ini.beginGroup(QString("last_selected_slots"));
    last_status_ini.setValue(QString("slots"), GetSelectSlots());
    last_status_ini.endGroup();

//    last_status_ini.beginGroup(QString("last_selected_slots_attr"));
//    last_status_ini.setValue(QString("index"), GetSelectSlots());
//    last_status_ini.endGroup();
}

void EditPanel::create_widget()
{
    this->setFixedWidth(780);

    //编辑区域
    edit_area = new QWidget();
    edit_area->setFixedSize(780, 500); //430

    //编辑区域-预览部分（左边）
    preview_part = new QWidget(edit_area);
    preview_part->setGeometry(2, 2, 328, 496); //2, 2, 328, 496

    //编辑区域-输入部分（右边）
    input_part = new QWidget(edit_area);
    input_part->setGeometry(330, 2, 448, 496); //

    //键盘区域
    keyboard_area = new QWidget();
    keyboard_area->setFixedSize(780, 210);
    keyboard_area->setVisible(false);

    //编辑区域-预览部分（左边）内容
    preview_top_panel = new QWidget(edit_area);
    preview_top_panel->setFixedSize(318, 346);
    preview_top_panel->move(5,50);

    separation_line = new QFrame();
    separation_line->setFrameShape(QFrame::HLine);
    separation_line->setFixedSize(310,1);
    separation_line->hide();

    preview_bottom_panel = new QWidget();
    preview_bottom_panel->setFixedHeight(85);

    laser_info = new QLabel();
    laser_info->setText(QString("%1：<span style='color:#e62129'><B>%2</B></span>").arg(tr("激光模块"), tr("离线")));
    laser_info->setAlignment(Qt::AlignCenter);
    laser_info->setFixedHeight(60);
    laser_info->hide();

    keyboard_btn = new QPushButton();
    keyboard_btn->setCheckable(true);
    keyboard_btn->setFixedSize(42,60);
    keyboard_btn->hide();

    property_btn = new QPushButton();
    property_btn->setCheckable(true);
    property_btn->setFixedSize(42,60);
    property_btn->hide();

    //编辑区域-输入部分（右边）内容
    input_top_panel = new QWidget(input_part);
    input_top_panel->setGeometry(2, 2, 448, 48);

    expand_box_btn = new QPushButton();
    expand_box_btn->setFixedSize(90,40);

    retract_box_btn = new QPushButton();
    retract_box_btn->setFixedSize(90,40);

    #if defined(USE_SLIDE) || defined(USE_SLIDE_DOUBLE)
       expand_box_btn->hide();
       retract_box_btn->hide();
    #endif

    input_mid_panel = new QWidget(input_part);
    input_mid_panel->setGeometry(2, 50, 448, 400); //2, 42, 448, 342
    input_bottom_panel = new QWidget(input_part);
    input_bottom_panel->setGeometry(2, 450, 448, 65);  //2, 361, 448, 65

    template_part_panel = new QWidget();
    template_title = new QLabel();
    template_title->setFixedHeight(40);
    template_title->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

    template_selector = new QComboBox();
    template_selector->setFixedSize(170,40);
    template_selector->setMaxVisibleItems(20);

    template_edit_btn  = new QPushButton();
    template_edit_btn->setFixedSize(60,40);
    template_edit_btn->setText(tr("编辑"));

    repeat_title = new QLabel();
    repeat_title->setFixedHeight(40);
    repeat_title->setText(tr("重复次数："));

    repeat_selector = new QComboBox();
    repeat_selector->setFixedSize(80,40);
    repeat_selector->setMaxVisibleItems(20);

    create_btn = new QPushButton();
    create_btn->setFixedSize(118,40);
    create_btn->setText(QString("%1(&+)").arg(tr("创建")));

    create_menu = new QMenu();
    create_menu->setFixedWidth(444);
    create_menu_btn = new QPushButton("...");
    create_menu_btn->setFixedSize(38,40);
    setup_create_menu();

    //创建输入框
    for (int i=0; i<MAX_INPUT_PANEL; i++)
    {
        input_panel[i] = new InputPanel(i);
        if(i == 1)
          input_panel[i]->setSingleCoupleButtonVisible(true);
        else
          input_panel[i]->setSingleCoupleButtonVisible(false);
    }

    slot_title = new QLabel();
    slot_title->setFixedHeight(60);
    slot_title->setText(tr("槽盒："));

    for (int i=0; i<MAX_SLOTS; i++)
    {
        slot_btn_container[i] = new QWidget();
        slot_btn_container[i]->setFixedSize(35, 85);
        slot_btn[i] = new QPushButton(slot_btn_container[i]);
        slot_btn[i]->setText(QString::number(i+1));
        slot_btn[i]->setCheckable(true);
        slot_btn[i]->setGeometry(0,0,35,35);
        slot_attr_btn[i] = new ColorPushButton(slot_btn_container[i]);
        slot_attr_btn[i]->setGeometry(2,40,35,40);
        #ifdef USE_CASSETTE
        slot_attr_btn[i]->setButtonType(ButtonType::TissueCassette);
        #elif defined(USE_SLIDE) || defined(USE_SLIDE_DOUBLE)
        slot_attr_btn[i]->setButtonType(ButtonType::Slider);
        #endif
        slot_attr_menu[i] = new QMenu();
        slot_attr_btn[i]->setMenu(slot_attr_menu[i]);
        slot_attr_btn[i]->setObjectName(QString::number(i+1));
        setup_slots_menu_actions(slot_attr_btn[i], slot_attr_menu[i]);
    }

    //打印顺序选项面板与按钮
    print_order_button_panel = new QWidget();
    print_order_button_panel->setFixedWidth(110);
    print_order_button_group = new QButtonGroup();

    #if defined(USE_CASSETTE) || defined(USE_SLIDE)
    print_order_button_panel->hide();
    #endif

    //keyboard
    m_pKeyboard = new SimpleKeyboard(this);

    m_pMarkPreview = new MarkPreview(preview_top_panel);
    m_pMarkPreview->move(13, 10);
}

void EditPanel::create_widget_v2()
{
    this->setFixedWidth(780);

    //编辑区域
    edit_area = new QWidget();
    edit_area->setFixedSize(780, 500);

    //编辑区域-预览部分（左边）
    preview_part = new QWidget(edit_area);
    preview_part->setGeometry(0, 0, 330, 496); //2, 2, 328, 496

    //编辑区域-输入部分（右边）
    input_part = new QWidget(edit_area);
    input_part->setGeometry(330, 0, 450, 496); //330, 2, 448, 496

    //键盘区域
    keyboard_area = new QWidget();
    keyboard_area->setFixedSize(780, 210);
    keyboard_area->setVisible(false);

    // 左边 preview_top_panel
    preview_top_panel = new QWidget(edit_area);
    preview_top_panel->setFixedSize(318, 348);
    preview_top_panel->move(5, 48);

    preview_bottom_panel = new QWidget();
    preview_bottom_panel->setFixedHeight(85);

    // === 输入部分内容 ===
    input_top_panel = new QWidget(input_part);

    expand_box_btn = new QPushButton();
    expand_box_btn->setFixedSize(90, 40);
    expand_box_btn->setFocusPolicy(Qt::NoFocus); // 设置按钮不可聚焦，避免点击后焦点停留

    retract_box_btn = new QPushButton();
    retract_box_btn->setFixedSize(90, 40);
    retract_box_btn->setFocusPolicy(Qt::NoFocus); // 设置按钮不可聚焦，避免点击后焦点停留

#if defined(USE_SLIDE) || defined(USE_SLIDE_DOUBLE)
    expand_box_btn->hide();
    retract_box_btn->hide();
#endif

    // === 折叠展开按钮（新） ===
    toggle_expand_btn = new QPushButton(tr("展开更多"));
    toggle_expand_btn->setCheckable(true);
    toggle_expand_btn->hide(); // 是否显示由 updateInputPanelLayout() 决定

    // 原来的 input_mid_panel = new QWidget(input_part);
    // 改成 scrollArea
    input_scroll_area = new QScrollArea(input_part);
    input_scroll_area->setWidgetResizable(true);
    input_scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 真正的中间容器
    input_mid_container = new QWidget();
    input_mid_panel_layout = new QVBoxLayout(input_mid_container);
    input_mid_panel_layout->setContentsMargins(0,0,0,0);
    input_mid_panel_layout->setSpacing(4);

    // 把容器放进 scrollArea
    input_scroll_area->setWidget(input_mid_container);
    input_scroll_area->setStyleSheet(SS_input_scroll_area);
    input_mid_container->setObjectName("input_mid_container");
    input_mid_container->setFocusPolicy(Qt::NoFocus);


    input_bottom_panel = new QWidget(input_part);

    template_part_panel = new QWidget();
    template_title = new QLabel();
    template_title->setFixedHeight(40);
    template_title->setAlignment(Qt::AlignVCenter | Qt::AlignRight);

    template_selector = new QComboBox();
    template_selector->setFixedSize(170, 40);
    template_selector->setMaxVisibleItems(20);

    template_edit_btn  = new QPushButton();
    template_edit_btn->setFixedSize(60, 40);
    template_edit_btn->setText(tr("编辑"));
    template_edit_btn->setFocusPolicy(Qt::NoFocus); // 设置按钮不可聚焦，避免点击后焦点停留

    repeat_title = new QLabel();
    repeat_title->setFixedHeight(40);
    repeat_title->setText(tr("重复次数："));

    repeat_selector = new QComboBox();
    repeat_selector->setFixedSize(80, 40);
    repeat_selector->setMaxVisibleItems(20);

    create_btn = new QPushButton();
    create_btn->setFixedSize(118, 40);
    create_btn->setText(QString("%1(&+)").arg(tr("创建")));
    create_btn->setFocusPolicy(Qt::NoFocus); // 设置按钮不可聚焦，避免点击

    create_menu = new QMenu();
    create_menu->setFixedWidth(444);
    create_menu_btn = new QPushButton("...");
    create_menu_btn->setFixedSize(38, 40);
    setup_create_menu();

    // === 创建输入框（扩展到 MAX_INPUT_PANEL=16） ===
    for (int i = 0; i < MAX_INPUT_PANEL; i++)
    {
        input_panel[i] = new InputPanel(i);

        if (i == 1)
            input_panel[i]->setSingleCoupleButtonVisible(true);
        else
            input_panel[i]->setSingleCoupleButtonVisible(false);

        if (i >= MAX_INPUT_PANEL_SHOW) {
            input_panel[i]->setSingleCoupleButtonVisible(false);
        }

        // 模板可见性标记，默认 false
        input_panel[i]->setProperty("enabledByTemplate", false);
        
        // 提前将所有InputPanel添加到布局中
        input_panel[i]->setParent(input_mid_container);
        input_mid_panel_layout->addWidget(input_panel[i]);
        input_panel[i]->setVisible(false); // 默认隐藏
    }
//    input_mid_panel_layout->addStretch();

    // === 槽盒部分 ===
    slot_title = new QLabel();
    slot_title->setFixedHeight(60);
    slot_title->setText(tr("槽盒："));

    for (int i = 0; i < MAX_SLOTS; i++)
    {
        slot_btn_container[i] = new QWidget();
        slot_btn_container[i]->setFixedSize(35, 85);

        slot_btn[i] = new QPushButton(slot_btn_container[i]);
        slot_btn[i]->setText(QString::number(i+1));
        slot_btn[i]->setCheckable(true);
        slot_btn[i]->setGeometry(0, 0, 35, 35);

        slot_attr_btn[i] = new ColorPushButton(slot_btn_container[i]);
        slot_attr_btn[i]->setGeometry(2, 40, 35, 40);

    #ifdef USE_CASSETTE
        slot_attr_btn[i]->setButtonType(ButtonType::TissueCassette);
    #elif defined(USE_SLIDE) || defined(USE_SLIDE_DOUBLE)
        slot_attr_btn[i]->setButtonType(ButtonType::Slider);
    #endif

        slot_attr_menu[i] = new QMenu();
        slot_attr_btn[i]->setMenu(slot_attr_menu[i]);
        slot_attr_btn[i]->setObjectName(QString::number(i+1));
        setup_slots_menu_actions(slot_attr_btn[i], slot_attr_menu[i]);
    }

    // 打印顺序选项面板与按钮
    print_order_button_panel = new QWidget();
    print_order_button_panel->setFixedWidth(110);
    print_order_button_group = new QButtonGroup();

#if defined(USE_CASSETTE) || defined(USE_SLIDE)
    print_order_button_panel->hide();
#endif

    // keyboard
    m_pKeyboard = new SimpleKeyboard(this);

    m_pMarkPreview = new MarkPreview(preview_top_panel);
    m_pMarkPreview->move(13, 10);
}


void EditPanel::setup_layout()
{
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(0,0,0,0);
    main_layout->setSpacing(8);
    main_layout->setAlignment(Qt::AlignTop);
    main_layout->addWidget(edit_area);
    main_layout->addWidget(keyboard_area);

    QHBoxLayout *template_part_layout = new QHBoxLayout(template_part_panel);
    template_part_layout->setContentsMargins(5,5,5,5);
    template_part_layout->addWidget(template_title);
    template_part_layout->addWidget(template_selector);
    template_part_layout->addWidget(template_edit_btn);

    //edit_area-preview_part 编辑区域-预览部分（左边）
    QVBoxLayout *preview_part_layout = new QVBoxLayout(preview_part);
    preview_part_layout->setContentsMargins(0,5,0,15);
    preview_part_layout->setSpacing(1);
    preview_part_layout->addWidget(template_part_panel);
    preview_part_layout->addStretch();
    preview_part_layout->addWidget(preview_bottom_panel);

    QHBoxLayout *preview_part_bottom_layout = new QHBoxLayout(preview_bottom_panel);
    preview_part_bottom_layout->setContentsMargins(5,5,5,0);

    preview_part_bottom_layout->addWidget(slot_title);
    for (int i=0; i<MAX_SLOTS; i++)
    {
        preview_part_bottom_layout->addWidget(slot_btn_container[i]);
    }

    //按钮组垂直布局
    QVBoxLayout *print_order_button_group_layout = new QVBoxLayout(print_order_button_panel);
    print_order_button_group_layout->setContentsMargins(5,0,5,0);
    print_order_button_group_layout->setSpacing(0);
    QStringList buttonTexts = {tr("顺序打印"), tr("交替打印")};
    int id = 0;
    for (const QString &text : buttonTexts)
    {
        QPushButton *button = new QPushButton(text);
        button->setFixedHeight(40);
        button->setStyleSheet(SS_Order_Btn);
        print_order_button_group_layout->addWidget(button);
        print_order_button_group->addButton(button,id);
        button->setCheckable(true);
        id++;
    }
    preview_part_bottom_layout->addWidget(print_order_button_panel);
    preview_part_bottom_layout->addStretch();

    //edit_area-input_part -输入部分（右边）
    QHBoxLayout *input_top_panel_layout = new QHBoxLayout(input_top_panel);
    input_top_panel_layout->setContentsMargins(0,5,10,5);
    input_top_panel_layout->setSpacing(5);
    input_top_panel_layout->addStretch();
    input_top_panel_layout->addWidget(expand_box_btn);
    input_top_panel_layout->addWidget(retract_box_btn);

    QVBoxLayout *input_mid_panel_layout = new QVBoxLayout(input_mid_panel);
    input_mid_panel_layout->setContentsMargins(0,0,0,0);
    input_mid_panel_layout->setAlignment(Qt::AlignCenter);
    input_mid_panel_layout->addStretch();
    for (int i=0; i<MAX_INPUT_PANEL; i++)
    {
        input_mid_panel_layout->addWidget(input_panel[i]);
    }
    input_mid_panel_layout->addStretch();

    QHBoxLayout *input_bottom_panel_layout = new QHBoxLayout(input_bottom_panel);
    input_bottom_panel_layout->setContentsMargins(0,0,10,20);
    input_bottom_panel_layout->setSpacing(5);
    input_bottom_panel_layout->addWidget(repeat_title);
    input_bottom_panel_layout->addWidget(repeat_selector);
    input_bottom_panel_layout->addStretch();

    input_bottom_panel_layout->addWidget(create_btn);
    input_bottom_panel_layout->addWidget(create_menu_btn);

    QVBoxLayout *keyboard_area_layout = new QVBoxLayout(keyboard_area);
    keyboard_area_layout->setContentsMargins(0,0,0,0);
    keyboard_area_layout->setAlignment(Qt::AlignCenter);
    keyboard_area_layout->addWidget(m_pKeyboard);

}

void EditPanel::setup_layout_v2()
{
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(0, 0, 0, 0);
    main_layout->setSpacing(8);
    main_layout->setAlignment(Qt::AlignTop);
    main_layout->addWidget(edit_area);
    main_layout->addWidget(keyboard_area);

    auto * template_part_layout = new QHBoxLayout(template_part_panel);
    template_part_layout->setContentsMargins(0, 0, 10, 0);
    template_part_layout->setSpacing(5);
    template_part_layout->addWidget(template_title);
    template_part_layout->addWidget(template_selector);
    template_part_layout->addWidget(template_edit_btn);

    //edit_area-preview_part 编辑区域-预览部分（左边）
    QVBoxLayout *preview_part_layout = new QVBoxLayout(preview_part);
    preview_part_layout->setContentsMargins(0,5,0,15);
    preview_part_layout->setSpacing(1);
    preview_part_layout->addWidget(template_part_panel);
    preview_part_layout->addStretch();
    preview_part_layout->addWidget(preview_bottom_panel);

    QHBoxLayout *preview_part_bottom_layout = new QHBoxLayout(preview_bottom_panel);
    preview_part_bottom_layout->setContentsMargins(5,5,5,0);

    preview_part_bottom_layout->addWidget(slot_title);
    for (int i=0; i<MAX_SLOTS; i++)
    {
        preview_part_bottom_layout->addWidget(slot_btn_container[i]);
    }

    //按钮组垂直布局
    QVBoxLayout *print_order_button_group_layout = new QVBoxLayout(print_order_button_panel);
    print_order_button_group_layout->setContentsMargins(5,0,5,0);
    print_order_button_group_layout->setSpacing(0);
    QStringList buttonTexts = {tr("顺序打印"), tr("交替打印")};
    int id = 0;
    for (const QString &text : buttonTexts)
    {
        QPushButton *button = new QPushButton(text);
        button->setFixedHeight(40);
        button->setStyleSheet(SS_Order_Btn);
        print_order_button_group_layout->addWidget(button);
        print_order_button_group->addButton(button,id);
        button->setCheckable(true);
        id++;
    }
    preview_part_bottom_layout->addWidget(print_order_button_panel);
    preview_part_bottom_layout->addStretch();

    // === input_part ===
    auto *input_part_layout = new QVBoxLayout(input_part);
    input_part_layout->setContentsMargins(0, 0, 0, 0);
    input_part_layout->setSpacing(6);

    // 顶部
    auto *input_top_panel_layout = new QHBoxLayout(input_top_panel);
    input_top_panel_layout->setContentsMargins(0, 5, 10, 5);
    input_top_panel_layout->setSpacing(5);
    input_top_panel_layout->addStretch();
    input_top_panel_layout->addWidget(expand_box_btn);
    input_top_panel_layout->addWidget(retract_box_btn);

    // 中部
//    input_mid_panel_layout = new QVBoxLayout(input_mid_panel);
//    input_mid_panel_layout->setContentsMargins(0, 0, 0, 0);
//    input_mid_panel_layout->setSpacing(4);

    // 顺序：top → mid → bottom
//    input_part_layout->addWidget(input_top_panel);
//    input_part_layout->addWidget(input_scroll_area);   // 注意这里换成 scrollArea
//    input_part_layout->addWidget(input_bottom_panel);

    // 中部（scrollArea 固定展开在中间，自动拉伸）
    input_part_layout->addWidget(input_top_panel, 0, Qt::AlignTop);
    input_part_layout->addWidget(input_scroll_area, 1); // 给 1 的伸展因子，始终占据中间
    input_part_layout->addWidget(input_bottom_panel, 0, Qt::AlignBottom);


    // 底部
    auto *input_bottom_panel_layout = new QHBoxLayout(input_bottom_panel);
    input_bottom_panel_layout->setContentsMargins(0, 0, 10, 20);
    input_bottom_panel_layout->setSpacing(5);
    input_bottom_panel_layout->addWidget(repeat_title);
    input_bottom_panel_layout->addWidget(repeat_selector);
    input_bottom_panel_layout->addStretch();
    input_bottom_panel_layout->addWidget(toggle_expand_btn); // 加在底部，控制显示/隐藏
    input_bottom_panel_layout->addWidget(create_btn);
    input_bottom_panel_layout->addWidget(create_menu_btn);

    // === 信号槽 ===
    connect(toggle_expand_btn, &QPushButton::toggled, this, [this](bool){
        updateInputPanelLayout();
    });

}


void EditPanel::updateInputPanelLayout()
{
    if (!input_mid_panel_layout) return;

    // 添加一个stretch到布局末尾，确保所有可见的InputPanel都能正常显示
    if (input_mid_panel_layout->count() > MAX_INPUT_PANEL) {
        QLayoutItem *stretch = input_mid_panel_layout->takeAt(MAX_INPUT_PANEL);
        delete stretch;
    }
    input_mid_panel_layout->addStretch();

    /*
    // 是否显示展开按钮
    bool needToggle = (enabledIdx.size() > MAX_INPUT_PANEL_SHOW);
    //toggle_expand_btn->setVisible(needToggle);
    toggle_expand_btn->setVisible(false);

    // scrollArea 高度策略
    int panelHeight = input_panel[0]->sizeHint().height(); // 取第一个面板高度
    int baseCount = qMin(MAX_INPUT_PANEL_SHOW, 6);          // 至少按 6 个算
    int minHeight = panelHeight * baseCount + 20;

    if (toggle_expand_btn->isChecked() && needToggle) {
        input_scroll_area->setMaximumHeight(QWIDGETSIZE_MAX);
        toggle_expand_btn->setText(tr("收起"));
    } else {
        if (enabledIdx.size() <= baseCount) {
            // 少于等于6个，保持标准高度
            input_scroll_area->setMaximumHeight(minHeight);
        } else {
            // 超过6个，显示前 MAX_INPUT_PANEL_SHOW 高度
            int h = 0;
            for (int i = 0; i < MAX_INPUT_PANEL_SHOW && i < enabledIdx.size(); i++) {
                h += input_panel[enabledIdx[i]]->sizeHint().height();
            }
            input_scroll_area->setMaximumHeight(h + 20);
        }
        toggle_expand_btn->setText(tr("展开更多"));
    }
    */

    // 每次刷新后，把滚动条回到最顶
     if (input_scroll_area->verticalScrollBar()) {
         input_scroll_area->verticalScrollBar()->setValue(0);
     }
}

void EditPanel::scheduleRelayout()
{
    if (!m_relayoutTimer) {
        m_relayoutTimer = new QTimer(this);
        m_relayoutTimer->setSingleShot(true);
        connect(m_relayoutTimer, &QTimer::timeout, this, &EditPanel::updateInputPanelLayout);
    }
    if (!m_relayoutTimer->isActive())
        m_relayoutTimer->start(0);
}

void EditPanel::set_stylesheet()
{
    //edit_area->setStyleSheet(SS_EditPanel);
    preview_part->setStyleSheet(SS_preview_part);
    //separation_line->setStyleSheet(SS_separation_line);
    //keyboard_btn->setStyleSheet(SS_keyboard_btn);
    //property_btn->setStyleSheet(SS_param_btn);

    //laser_info->setStyleSheet(SS_slot_title);
    template_title->setStyleSheet(SS_slot_title);
    repeat_title->setStyleSheet(SS_slot_title);

    input_part->setStyleSheet(SS_input_part);

    template_edit_btn->setStyleSheet(SS_create_btn);
    template_selector->setStyleSheet(SS_template_seletor_main);
    template_selector->setView(new QListView());
    repeat_selector->setStyleSheet(SS_template_seletor_main);
    repeat_selector->setView(new QListView());
    create_btn->setStyleSheet(SS_create_btn);
    create_menu_btn->setStyleSheet(SS_create_menu_btn);
    create_menu->setStyleSheet(SS_create_menu);

    expand_box_btn->setStyleSheet(SS_create_btn);
    retract_box_btn->setStyleSheet(SS_create_btn);

    slot_title->setStyleSheet(SS_slot_title);
    for (int i=0; i<MAX_SLOTS; i++)
    {
        slot_btn[i]->setStyleSheet(SS_slot_btn);
        slot_attr_btn[i]->setStyleSheet(SS_slot_attr_btn);
        slot_attr_menu[i]->setStyleSheet(SS_slot_attr_menu);
    }

}

void EditPanel::connect_signals_and_slots()
{
    //connect(keyboard_btn, &QPushButton::clicked, this, &EditPanel::on_keyboard_btn_clicked);
    //connect(property_btn, &QPushButton::clicked, this, &EditPanel::on_attr_btn_clicked);
    connect(create_btn, &QPushButton::clicked, this, &EditPanel::on_create_btn_clicked);
    connect(create_menu_btn, &QPushButton::clicked, this, &EditPanel::on_create_menu_btn_clicked);
    connect(template_edit_btn, &QPushButton::clicked, this, [=](){
        //让输入编辑框失去焦点，避免受打开的触摸键盘信号干扰
        input_part->setFocus();

        emit template_edit_btn_Clicked();

    });
    connect(expand_box_btn, &QPushButton::clicked, this, [=](){emit expand_box_btn_Clicked();});
    connect(retract_box_btn, &QPushButton::clicked, this, [=](){emit retract_box_btn_Clicked();});

    for (int i=0; i<MAX_SLOTS; i++)
    {
        connect(slot_btn[i], &QPushButton::clicked, this, &EditPanel::on_slots_btn_clicked);
    }

    for (int i=0; i<MAX_INPUT_PANEL; i++)
    {
        connect(input_panel[i], &InputPanel::property_btn_Clicked, this, &EditPanel::on_panel_prop_btn_clicked);
        connect(input_panel[i], &InputPanel::property_btn_saved, this, &EditPanel::on_panel_prop_btn_saved);
        connect(input_panel[i], &InputPanel::input_text_Changed, this, &EditPanel::on_input_text_changed);
        connect(input_panel[i], &InputPanel::signal_edit_focusIn, this, &EditPanel::on_input_edit_focusIn);
    }
    connect(template_selector, QOverload<const QString&>::of(&QComboBox::currentIndexChanged),this,&EditPanel::on_template_selector_changed);
    connect(repeat_selector, QOverload<const QString&>::of(&QComboBox::currentIndexChanged),this,&EditPanel::on_repeat_selector_changed);

    connect(m_pKeyboard, &SimpleKeyboard::signalKeyPress, this, &EditPanel::slotOnSimpleKeyboard);

}

void EditPanel::setup_create_menu()
{
    create_task = new QAction(tr("创建任务"));                                   //创建任务 Ctrl+/
    create_task->setShortcut(Qt::CTRL + Qt::Key_Slash);
    create_task_and_print = new QAction(tr("创建任务并打印"));                    //创建任务并打印 Ctrl+*
    create_task_and_print->setShortcut(Qt::CTRL + Qt::Key_Asterisk);
    create_task_and_print_instant = new QAction(tr("创建任务并打印(插队)"));      //创建任务并打印(插队) Ctrl+-
    create_task_and_print_instant->setShortcut(Qt::CTRL + Qt::Key_Minus);
    clear_all_input_box = new QAction(tr("清空录入框"));                         //清空录入框 Ctrl+=
    clear_all_input_box->setShortcut(Qt::CTRL + Qt::Key_Equal);

    create_task_from_file = new QAction(tr("从文件创建任务"));                    //从文件创建任务 Ctrl++
    create_task_from_file->setShortcut(Qt::CTRL + Qt::Key_Plus);

    create_menu->addAction(create_task);
    create_menu->addAction(create_task_and_print);
    create_menu->addAction(create_task_and_print_instant);
    create_menu->addSeparator();
    create_menu->addAction(create_task_from_file);
    create_menu->addSeparator();
    create_menu->addAction(clear_all_input_box);

    //connect relative slot function here
    connect(create_task, &QAction::triggered, this, [=](){emit create_task_action_Triggered();});
    connect(create_task_and_print, &QAction::triggered, this, [=](){emit create_task_and_print_action_Triggered();});
    connect(create_task_and_print_instant, &QAction::triggered, this, [=](){emit create_task_and_print_instant_action_Triggered();});
    connect(create_task_from_file, &QAction::triggered, this, [=](){emit create_task_from_file_action_Triggered();});
    connect(clear_all_input_box, &QAction::triggered, this, [=](){clear_all_input_panel_text();});

    QShortcut *create_task_shortcut = new QShortcut(QKeySequence(Qt::CTRL+ Qt::Key_Slash), this);
    QShortcut *create_task_and_print_shortcut = new QShortcut(QKeySequence(Qt::CTRL+ Qt::Key_Asterisk), this);
    QShortcut *create_task_and_print_instant_shortcut = new QShortcut(QKeySequence(Qt::CTRL+ Qt::Key_Minus), this);
    QShortcut *clear_all_input_box_shortcut = new QShortcut(QKeySequence(Qt::CTRL+ Qt::Key_Equal), this);

    QShortcut *create_task_from_file_shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus), this);

    connect(create_task_shortcut, &QShortcut::activated, this, [=](){emit create_task_action_Triggered();});
    connect(create_task_and_print_shortcut, &QShortcut::activated, this, [=](){emit create_task_and_print_action_Triggered();});
    connect(create_task_and_print_instant_shortcut, &QShortcut::activated, this, [=](){emit create_task_and_print_instant_action_Triggered();});
    connect(create_task_from_file_shortcut, &QShortcut::activated, this, [=](){emit create_task_from_file_action_Triggered();});
    connect(clear_all_input_box_shortcut, &QShortcut::activated, this, [=](){clear_all_input_panel_text();});

}

void EditPanel::setup_slots_menu_actions(ColorPushButton *menu_btn, QMenu *menu)
{
    ColorMarkLevelArgs args[MAX_SLOT_MENU_ACTIONS];
    for(int i=0; i<MAX_SLOT_MENU_ACTIONS; i++)
    {
        MaintenanceDialog::ReadColorMarkLevelArgs(i+1, &args[i]);
    }

    QAction *actions[MAX_SLOT_MENU_ACTIONS];
//    QString cassette_name[MAX_SLOT_MENU_ACTIONS] = {tr("白色"),tr("粉色"),tr("黄色"),tr("蓝色"),tr("绿色"),tr("紫色"),tr("自定义1"),tr("自定义2")};
//    QString cassette_color[MAX_SLOT_MENU_ACTIONS] = {"#ffffff","#f5cdd6","#f5de87","#91bfe0","#ace6da","#cbb2d4","#ffffff","#ffffff"};

    for (int i=0; i<MAX_SLOT_MENU_ACTIONS; i++)
    {
        actions[i] = new QAction(menu_btn);
        actions[i]->setText(QString(args[i].color_name));

        //前六种槽盒颜色不可编辑，要国际化翻译，白、黄、绿、蓝、紫、粉
        QStringList defaultcolorNames,defaultcolorTranslateNames;
        defaultcolorNames << "白色" << "黄色" << "绿色" << "蓝色" << "紫色" << "粉色";
        defaultcolorTranslateNames << tr("白色") << tr("黄色") << tr("绿色") << tr("蓝色") << tr("紫色") << tr("粉色");

        int index = defaultcolorNames.indexOf(args[i].color_name);
        if(index >= 0)
        {
            actions[i]->setText(defaultcolorTranslateNames[index]);
        }

        QPixmap pixmap(60,60);
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        painter.setBrush(QColor(args[i].color_code));
        painter.setPen(QColor(QString("#bcbcbc")));
        painter.drawEllipse(0,0,60,60);
        painter.end();

        QIcon color_icon(pixmap);
        actions[i]->setIcon(color_icon);
        menu->addAction(actions[i]);

        connect(actions[i], &QAction::triggered, this, [=](){
            slot_attr_menu_selected_index = i;
            /*所选择项写入配置文件*/

            QSettings last_status_ini(LAST_STATUS_INI, QSettings::IniFormat);
            last_status_ini.beginGroup(QString("last_selected_hopper_color_index"));
            last_status_ini.setValue(QString("hopper_%1").arg(menu_btn->objectName()), i);
            last_status_ini.endGroup();

            menu_btn->setBackgroundColor(args[i].color_code);
        });

    }
}
QString EditPanel::read_last_selected_template_name()
{
    //读取最后选择的模板名
    QSettings main_win_cfg_ini(LAST_STATUS_INI, QSettings::IniFormat);
    main_win_cfg_ini.beginGroup(QString("last_selected_template"));
    QString name = main_win_cfg_ini.value("name").toString();
    main_win_cfg_ini.endGroup();
    return name;
}

QString EditPanel::read_last_selected_repeat_times()
{
    //读取最后选择的重复次数
    QSettings main_win_cfg_ini(LAST_STATUS_INI, QSettings::IniFormat);
    main_win_cfg_ini.beginGroup(QString("last_selected_repeat"));
    QString times = main_win_cfg_ini.value("times").toString();
    main_win_cfg_ini.endGroup();
    return times;
}


void EditPanel::set_last_selected_slots()
{
    ColorMarkLevelArgs args[MAX_SLOT_MENU_ACTIONS];
    for(int i=0; i<MAX_SLOT_MENU_ACTIONS; i++)
    {
        MaintenanceDialog::ReadColorMarkLevelArgs(i+1, &args[i]);
    }

    //读取最后选择的重复次数
    QSettings main_win_cfg_ini(LAST_STATUS_INI, QSettings::IniFormat);
    main_win_cfg_ini.beginGroup(QString("last_selected_slots"));
    QString slots_str = main_win_cfg_ini.value("slots", "").toString();
    main_win_cfg_ini.endGroup();


#ifdef USE_CASSETTE
    if(slots_str.isEmpty())
    {
        return;
    }
    QStringList slots_list = slots_str.split(",");
    for (QString &slot : slots_list)
    {
        slot_btn[slot.toInt()-1]->setChecked(true);
        slot_btn[slot.toInt()-1]->setEnabled(true);
    }
#elif defined(USE_SLIDE) || defined(USE_SLIDE_DOUBLE)

    int slot_num = 0;
    #ifdef USE_SLIDE_DOUBLE
        slot_num = 1;
    #endif

        //载玻片分单槽、双槽
        for (int i=0; i<MAX_SLOTS; i++)
        {
            slot_btn[i]->setChecked(false);
            slot_btn[i]->setEnabled(false);
            if(i>slot_num)
            {
                slot_btn_container[i]->hide();
            }
            else
            {
                //slot_num = 0,1
                slot_btn[i]->setChecked(true);
                slot_btn[i]->setEnabled(true);
                if(slot_num > i)//双槽
                {
                    slot_btn[slot_num]->setChecked(true);
                    slot_btn[slot_num]->setEnabled(true);
                }
            }
        }
#endif
}

void EditPanel::set_last_selected_slot_attr_btn_color()
{
    //读取对应选取的菜单项和对应的颜色
    ColorMarkLevelArgs args[MAX_SLOT_MENU_ACTIONS];
    for(int i=0; i<MAX_SLOT_MENU_ACTIONS; i++)
    {
        MaintenanceDialog::ReadColorMarkLevelArgs(i+1, &args[i]);
    }
    QSettings main_win_cfg_ini(LAST_STATUS_INI, QSettings::IniFormat);
    main_win_cfg_ini.beginGroup(QString("last_selected_hopper_color_index"));
    for (int i=0; i<MAX_SLOTS; i++)
    {
        int color_index = main_win_cfg_ini.value(QString("hopper_%1").arg(i+1), 0).toInt();
        slot_attr_btn[i]->setBackgroundColor(args[color_index].color_code);
//        slot_attr_btn[i]->setStyleSheet(QString(
//                                                 "QPushButton{"
//                                                 "    border:1px solid #bcbcbc;"
//                                                 "    background-color:%1;"
//                                                 "    border-radius: 5px;"
//                                                 "}"
//                                                 "QPushButton:pressed{"
//                                                 "    border:2px solid #0061b0;"
//                                                 "}"
//                                                 "QPushButton::menu-indicator {"
//                                                 "    image:none;"
//                                                 "}"
//                                                 ).arg(args[color_index].color_code));
    }
    main_win_cfg_ini.endGroup();
}

void EditPanel::clear_all_input_panel_text()
{
    for (int i=0; i<MAX_INPUT_PANEL; i++)
    {
        input_panel[i]->clearInputText();
    }
}

void EditPanel::on_create_btn_clicked()
{
    emit create_btn_Clicked();
}

void EditPanel::on_create_menu_btn_clicked()
{
    QPoint menuPos = create_menu_btn->mapToGlobal(QPoint(-(444-create_menu_btn->width()), 2+create_menu_btn->height()));
    create_menu_btn->setDown(true);
    create_menu->exec(menuPos);
    create_menu_btn->setDown(false);
}

/*
void EditPanel::on_keyboard_btn_clicked()
{
    if(keyboard_btn->isChecked())
    {
        keyboard_area->setVisible(true);
    }
    else
    {
        keyboard_area->setVisible(false);
    }
}

void EditPanel::on_attr_btn_clicked()
{
    if(property_btn->isChecked())
    {
        for (int i=0; i<MAX_INPUT_PANEL; i++)
        {
            input_panel[i]->setPropBtnVisible(true);
        }
    }
    else
    {
        for (int i=0; i<MAX_INPUT_PANEL; i++)
        {
            input_panel[i]->setPropBtnVisible(false);
        }
    }
}
*/

void EditPanel::on_panel_prop_btn_clicked()
{
    //property_btn->setChecked(false);

    for (int i=0; i<MAX_INPUT_PANEL; i++)
    {
        input_panel[i]->setPropBtnVisible(false);
    }
}

void EditPanel::on_panel_prop_btn_saved()
{
       clear_all_input_panel_text();
}

void EditPanel::on_input_text_changed(QString text)
{
    InputPanel::setIsScannerInput(true);

    InputPanel *input_panel = qobject_cast<InputPanel*>(QObject::sender());
    int panel_id = input_panel->getPanelId();
    qDebug() << "panel"<< input_panel->getPanelId() <<"text changed:" << text;

    if(
        (current_select_temp_attr.panel_attr[panel_id].sta == "editable")
        ||
        (current_select_temp_attr.panel_attr[panel_id].sta == "invisible")
      )
    {
        int max_width = current_select_temp_attr.panel_attr[panel_id].min_width;
        //获取截取长度后的模版文本
        QString tempStr = current_select_temp_attr.panel_attr[panel_id].input_text.left(max_width);
        TempTextAttr tempAttr;
        tempAttr.tempText = tempStr;
        tempAttr.textAlign = m_textAlignment;
        tempAttr.textPos = QPoint(current_select_temp_attr.panel_attr[panel_id].pos_x,current_select_temp_attr.panel_attr[panel_id].pos_y);
        m_pMarkPreview->setText(panel_id, this->input_panel[panel_id]->getEditText().left(max_width),tempAttr);
    }

    //内容分割处理
    if(input_separate_mode == 1)
    {//分割符
        if (text.endsWith(input_separate_symbol))
        {
            // 删除结尾的指定字符串
            text.chop(input_separate_symbol.length());

            //先清除左右双框内容
            input_panel->clearInputText();
            input_panel->setText(text);

            //切换下一下Focus，最后一个不循环
            setNextInputPanalFocus(false);
        }
    }
    else if(input_separate_mode == 2)
    {//预设长度
        if(text.length() >= input_preset_length[panel_id] && input_preset_length[panel_id] > 0)
        {
            //切换下一下Focus，最后一个不循环
            setNextInputPanalFocus(false);
        }
    }

    // 延迟重置扫描枪输入标志，确保焦点切换完成
    QTimer::singleShot(100, this, [](){ InputPanel::setIsScannerInput(false); });
}

void EditPanel::on_input_edit_focusIn()
{
    //如果标刻模版不是当前选用模版，切换回当前选用模版

    if(0!=current_select_temp_attr.name.compare(template_selector->currentText()))
    {
       qDebug() << "current_select_temp_attr.name ==" << current_select_temp_attr.name;
       on_template_selector_changed();
    }

    for(int i=0; i<MAX_INPUT_PANEL-1; i++)
    {
        if((current_select_temp_attr.panel_attr[i].sta == "editable")
            ||
            (current_select_temp_attr.panel_attr[i].sta == "invisible"))
        {
            int max_width = current_select_temp_attr.panel_attr[i].min_width;
            //获取截取长度后的模版文本
            QString tempStr = current_select_temp_attr.panel_attr[i].input_text.left(max_width);
            TempTextAttr tempAttr;
            tempAttr.tempText = tempStr;
            tempAttr.textAlign = m_textAlignment;
            tempAttr.textPos = QPoint(current_select_temp_attr.panel_attr[i].pos_x,current_select_temp_attr.panel_attr[i].pos_y);

            m_pMarkPreview->setText(i, input_panel[i]->getEditText().left(max_width),tempAttr);
        }
    }

}

void EditPanel::on_template_selector_changed()
{
//    save_last_selected_template();
//    clear_all_input_panel_text();
//    emit template_selector_Changed(template_selector->currentText());

    QTimer::singleShot(50, this, [this]() {

      on_template_selector_OnThread();
    });
}

void EditPanel::on_template_selector_OnThread()
{
    //线程加载数据
    QThread* thread = new QThread();
    connect(thread, &QThread::started, thread, [=]()mutable{
        qDebug()<<"started!";
        thread->quit();
    }, Qt::DirectConnection);
    connect(thread, &QThread::finished, this, [=]()mutable{
        thread->wait();
        thread->deleteLater();
        qDebug()<<"finished!";

        save_last_selected_template();
        clear_all_input_panel_text();
        emit template_selector_Changed(template_selector->currentText());
    });
    thread->start();
}

void EditPanel::on_repeat_selector_changed()
{
    save_last_selected_repeat();
}

void EditPanel::on_slots_btn_clicked()
{
    save_last_selected_slots();
}

void EditPanel::slotOnSimpleKeyboard(Qt::Key val, QString text)
{
    qDebug() << "SimpleKeyboard:" << val << "," << text;

    if (Qt::Key_Return == val)  // 对回车键进行特殊处理
    {
        return;
    }

    // 没有特殊处理的按键，统一调用SimpleKeyboard类的sendKeyEvent函数，将按键信息发送给当前的焦点对象。
    m_pKeyboard->sendKeyEvent(qApp->focusWidget(), val, text);
}

void EditPanel::setFirstInputPanalFocus()
{
    for (int i=0; i<MAX_INPUT_PANEL; i++)
    {
        if(input_panel[i]->isVisible())
        {
            input_panel[i]->setFocus();
            input_scroll_area->ensureWidgetVisible(input_panel[i],0,0);
            break;
        }
    }
}

void EditPanel::setNextInputPanalFocus(bool isLoop)
{
    for (int i=0; i<MAX_INPUT_PANEL; i++)
    {
        if(input_panel[i]->isVisible() && input_panel[i]->isFocus())
        {
            for(int j=i+1; j<MAX_INPUT_PANEL; j++)
            {
                if(input_panel[j]->isVisible())
                {
                    input_panel[j]->setFocus();
                    input_scroll_area->ensureWidgetVisible(input_panel[j],0,0);
                    return;
                }
            }
        }
    }

    //Focus到了最后一个，是否循环
    if(isLoop)
    {
        setFirstInputPanalFocus();
    }
}

void EditPanel::setFirstInputBoxFocus()
{
    for (int i=0; i<MAX_INPUT_PANEL; i++)
    {
        if(input_panel[i]->isVisible())
        {
            input_panel[i]->setLeftBoxFocus();
            input_scroll_area->ensureWidgetVisible(input_panel[i],0,0);
            break;
        }
    }
}

void EditPanel::setNextInputBoxFocus(bool isLoop)
{
    for (int i=0; i<MAX_INPUT_PANEL; i++)
    {
        if(input_panel[i]->isVisible() && input_panel[i]->isLeftBoxFocus())
        {
            input_panel[i]->setRightBoxFocus();
            input_scroll_area->ensureWidgetVisible(input_panel[i],0,0);
            return;
        }
        else if(input_panel[i]->isVisible() && input_panel[i]->isRightBoxFocus())
        {
            for(int j=i+1; j<MAX_INPUT_PANEL; j++)
            {
                if(input_panel[j]->isVisible())
                {
                    input_panel[j]->setLeftBoxFocus();
                    input_scroll_area->ensureWidgetVisible(input_panel[j],0,0);
                    return;
                }
            }
        }
    }

    //Focus到了最后一个，是否循环
    if(isLoop)
    {
        setFirstInputBoxFocus();
    }
}

void EditPanel::setTypeinConnectorSymbol(const QString &connectorSymbol)
{
    for(int i=0; i<MAX_INPUT_PANEL; i++)
    {
        input_panel[i]->setConnectorSymbol(connectorSymbol);
    }
}

void EditPanel::setTypeinDelimiterSymbol(const QString &dilimiterSymbol)
{
    for(int i=0; i<MAX_INPUT_PANEL; i++)
    {
        input_panel[i]->setDelimiterSymbol(dilimiterSymbol);
    }
}

void EditPanel::setPresetTextVisible(bool isVisible)
{
    for(int i=0; i<MAX_INPUT_PANEL; i++)
    {
        input_panel[i]->setPresetTextVisible(isVisible);
    }
}

void EditPanel::setInputSeparateMode(int separateMode)
{
    input_separate_mode = separateMode;
}

void EditPanel::setInputSeparateSymbol(const QString &seprateSymbol)
{
    input_separate_symbol = seprateSymbol;
}

void EditPanel::setInputPresetLengthStr(const QString &lengthStr)
{
    QStringList len_list;

    for(int i=0; i<MAX_INPUT_PANEL; i++)
    {
        input_preset_length[i] = 0;
    }

    if(lengthStr.contains("/"))
    {
        len_list = lengthStr.split("/");
        for(int i=0; i<MAX_INPUT_PANEL && i<len_list.size(); i++)
        {
            input_preset_length[i] = len_list[i].toUInt();
        }
    }
    else
    {
        input_preset_length[0] = lengthStr.toUInt();
    }
}

bool EditPanel::isLastInputPanalFocus()
{
    for (int i=MAX_INPUT_PANEL-1; i>=0; i--)
    {
        if(input_panel[i]->isVisible())
        {
            return input_panel[i]->isFocus();
        }
    }
    return false;
}

void EditPanel::incFocusInputBox()
{
    for (int i=0; i<MAX_INPUT_PANEL; i++)
    {
        if(input_panel[i]->isFocus())
        {
            input_panel[i]->incText();
            return ;
        }
    }
    return ;
}

void EditPanel::decFocusInputBox()
{
    for (int i=0; i<MAX_INPUT_PANEL; i++)
    {
        if(input_panel[i]->isFocus())
        {
            input_panel[i]->decText();
            return ;
        }
    }
    return ;
}

void EditPanel::setMergeSymbol(const QString &mergeSymbol)
{
    for(int i=0; i<MAX_INPUT_PANEL; i++)
    {
        input_panel[i]->setMergeSymbol(mergeSymbol);
    }
}

void EditPanel::setInputPanelText(const int panelId, const QString &text)
{
    if(input_panel[panelId]->isVisible())
    {
        input_panel[panelId]->clearInputText();
        input_panel[panelId]->setText(text);
    }
    else
    {
        input_panel[panelId]->clearInputText();
    }
}

void EditPanel::setExpand_box_btn_enble(bool isEnable)
{
    expand_box_btn->setEnabled(isEnable);
}

void EditPanel::setRetract_box_btn_enble(bool isEnable)
{
    retract_box_btn->setEnabled(isEnable);
}


