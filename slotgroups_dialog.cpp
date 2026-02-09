#include "slotgroups_dialog.h"
#include "menu_dialog_style.h"
#include "edit_panel_style.h"

SlotGroups_Dialog::SlotGroups_Dialog(BaseDialog *parent) : BaseDialog(parent)
{
    setTitle(tr("选槽分组"));

    create_widget();
    setup_layout();
    set_stylesheet();
    connect_signals_and_slots();
}

int SlotGroups_Dialog::Exec()
{
    applyEditStateToUI();
    return exec();
}

// 新增：获取编辑状态
SlotGroupsEditState SlotGroups_Dialog::getEditState() const
{
    return currentEditState;
}

// 新增：设置编辑状态
void SlotGroups_Dialog::setEditState(const SlotGroupsEditState &state)
{
    currentEditState = state;
    originalEditState = state;
    applyEditStateToUI();
}

// 修改：从UI加载数据到编辑状态
void SlotGroups_Dialog::loadEditStateFromUI()
{
    // 加载hopper颜色（从UI读取，而不是INI文件）
    ColorMarkLevelArgs args[MAX_SLOT_MENU_ACTIONS];
    for (int i = 0; i < MAX_SLOT_MENU_ACTIONS; i++) {
        MaintenanceDialog::ReadColorMarkLevelArgs(i+1, &args[i]);
    }

    // 通过按钮的背景颜色反推颜色索引
    for (int i = 0; i < BUTTONS_PER_GROUP; i++) {
        QColor currentColor = slot_attr_buttons[i]->getBackgroundColor();
        int color_index = 0;

        // 查找当前颜色对应的索引
        for (int j = 0; j < MAX_SLOT_MENU_ACTIONS; j++) {
            // 将字符串颜色代码转换为 QColor 进行比较
            QColor configColor(args[j].color_code);
            if (configColor.isValid() && configColor == currentColor) {
                color_index = j;
                break;
            }
        }

        currentEditState.hopperColors[QString("hopper_%1").arg(i+1)] = color_index;
    }

    // 加载槽位选择状态（从UI读取）
    for (int group = 0; group < MAX_SLOTS; group++) {
        QJsonArray selections;
        for (int btn = 0; btn < BUTTONS_PER_GROUP; btn++) {
            selections.append(slot_buttons[group][btn]->isChecked() ? 1 : 0);
        }
        currentEditState.slotSelections[QString("group_%1").arg(group+1)] = selections;
    }

    // 加载映射符（从UI读取）
    for (int group = 0; group < MAX_SLOTS; group++) {
        currentEditState.mappings[QString("group_%1").arg(group+1)] = mapping_edits[group]->text();
    }
}

// 将编辑状态应用到UI
void SlotGroups_Dialog::applyEditStateToUI()
{
    // 应用hopper颜色
    ColorMarkLevelArgs args[MAX_SLOT_MENU_ACTIONS];
    for (int i = 0; i < MAX_SLOT_MENU_ACTIONS; i++) {
        MaintenanceDialog::ReadColorMarkLevelArgs(i+1, &args[i]);
    }

    for (int i = 0; i < BUTTONS_PER_GROUP; i++) {
        QString key = QString("hopper_%1").arg(i+1);
        if (currentEditState.hopperColors.contains(key)) {
            int color_index = currentEditState.hopperColors[key].toInt();
            if (color_index >= 0 && color_index < MAX_SLOT_MENU_ACTIONS) {
                // 将字符串颜色代码转换为 QColor
                QColor color(args[color_index].color_code);
                if (color.isValid()) {
                    slot_attr_buttons[i]->setBackgroundColor(color);
                } else {
                    // 如果颜色无效，使用默认颜色
                    slot_attr_buttons[i]->setBackgroundColor(Qt::white);
                }
            }
        }
    }

    // 应用槽位选择状态
    for (int group = 0; group < MAX_SLOTS; group++) {
        QString key = QString("group_%1").arg(group+1);
        if (currentEditState.slotSelections.contains(key)) {
            QJsonArray selections = currentEditState.slotSelections[key].toArray();
            for (int btn = 0; btn < BUTTONS_PER_GROUP && btn < selections.size(); btn++) {
                bool isChecked = (selections[btn].toInt() == 1);
                slot_buttons[group][btn]->setChecked(isChecked);
            }
        }
    }

    // 应用映射符
    for (int group = 0; group < MAX_SLOTS; group++) {
        QString key = QString("group_%1").arg(group+1);
        if (currentEditState.mappings.contains(key)) {
            mapping_edits[group]->setText(currentEditState.mappings[key].toString());
        }
    }
}

void SlotGroups_Dialog::on_edit_btn_clicked()
{
    is_editing = true;
    main_area->setEnabled(true);
    edit_btn->setVisible(false);
    save_btn->setVisible(true);
    cancel_btn->setVisible(true);

    // 保存原始状态
    originalEditState = currentEditState;

    // 清除可能的高亮显示
    clearMappingHighlights();
}

void SlotGroups_Dialog::on_save_btn_clicked()
{
    // 校验映射符唯一性
    if (!validateMappingUniqueness())
    {
        DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG, this);
        msg_box->setTitle(tr("校验失败"));
        msg_box->setContent(tr("存在重复的映射符，请修改后重试！"));
        msg_box->exec();
        return; // 校验失败，不执行保存
    }

    // 从UI加载当前状态到内存（更新currentEditState）
    loadEditStateFromUI();

    // 发送状态改变信号
    emit editStateChanged(currentEditState);

    main_area->setEnabled(false);
    edit_btn->setVisible(true);
    save_btn->setVisible(false);
    cancel_btn->setVisible(false);

    // 接受对话框（表示用户确认保存）
    this->accept();
}

void SlotGroups_Dialog::on_cancel_btn_clicked()
{
    if (is_editing)
    {
        // 恢复原始状态
        currentEditState = originalEditState;
        applyEditStateToUI();
        is_editing = false;
    }
    main_area->setEnabled(false);
    edit_btn->setVisible(true);
    save_btn->setVisible(false);
    cancel_btn->setVisible(false);

    // 清除可能的高亮显示
    clearMappingHighlights();

}

void SlotGroups_Dialog::on_back_btn_clicked()
{
    //如果正在编辑中弹出提示
    if(save_btn->isVisible())
    {
        DialogMsgBox *msg_box = new DialogMsgBox(ASK_DIALOG);
        msg_box->setContent(tr("编辑未保存。\n是否确定要直接退出？"));
        if(msg_box->exec()==QDialog::Accepted)
        {
            // 清除可能的高亮显示
            clearMappingHighlights();
            this->reject();
        }
    }
    else
    {
        // 清除可能的高亮显示
        clearMappingHighlights();
        this->close();
    }
}


void SlotGroups_Dialog::create_widget()
{
    slot_groups_area_panel = new QWidget(this);
    slot_groups_area_panel->setFixedWidth(780);
    slot_groups_area_panel->setFixedHeight(1080);

    // 创建6组按钮组
    for(int group = 0; group < MAX_SLOTS; group++)
    {
        // 创建分组标题
        group_titles[group] = new QLabel(QString("%1_%2").arg(tr("分组")).arg(group + 1));
        group_titles[group]->setFixedHeight(40);

        // 创建按钮组
        slot_groups_btn_group[group] = new QButtonGroup();
        slot_groups_btn_group[group]->setExclusive(false); // 设置为非互斥，允许多选

        // 创建每组6个按钮
        for(int btn = 0; btn < BUTTONS_PER_GROUP; btn++)
        {
            slot_buttons[group][btn] = new QPushButton(QString::number(btn + 1));
            slot_buttons[group][btn]->setFixedSize(60, 40);
            slot_buttons[group][btn]->setCheckable(true); // 设置为可选中
            slot_buttons[group][btn]->setAutoExclusive(false); // 禁用自动互斥

            // 将按钮添加到按钮组
            slot_groups_btn_group[group]->addButton(slot_buttons[group][btn], btn);
        }

        // 创建映射符标签
        mapping_labels[group] = new QLabel(tr("映射符"));
        mapping_labels[group]->setFixedHeight(40);
        mapping_labels[group]->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        // 创建映射符输入框
        mapping_edits[group] = new QLineEdit();
        mapping_edits[group]->setFixedSize(150, 40);
        mapping_edits[group]->setPlaceholderText(tr("请输入映射符"));
        mapping_edits[group]->setMaxLength(10); // 限制最大长度
    }

    // 新增：创建选槽颜色属性组
    slot_attr_title = new QLabel(tr("选槽颜色"));
    slot_attr_title->setFixedHeight(40);

    slot_attr_btn_group = new QButtonGroup();
    slot_attr_btn_group->setExclusive(true); // 颜色选择应该是互斥的

    // 创建6个ColorPushButton
    for(int btn = 0; btn < BUTTONS_PER_GROUP; btn++)
    {
        slot_attr_buttons[btn] = new ColorPushButton();
        slot_attr_buttons[btn]->setFixedSize(60, 80);
        slot_attr_buttons[btn]->setButtonType(TissueCassette); // 设置为组织盒样式
        //slot_attr_buttons[btn]->setBackgroundColor(colors[btn]);
        slot_attr_buttons[btn]->setCheckable(true);

        slot_attr_btn_group->addButton(slot_attr_buttons[btn], btn);

        slot_attr_menu[btn] = new QMenu();
        slot_attr_buttons[btn]->setMenu(slot_attr_menu[btn]);
        slot_attr_buttons[btn]->setObjectName(QString::number(btn+1));
        setup_slots_menu_actions(slot_attr_buttons[btn], slot_attr_menu[btn]);
    }

    //底部按钮
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
    //edit_btn->setVisible(true);//不注释这行，会出现小窗体部件闪现
    save_btn->setVisible(false);
    cancel_btn->setVisible(false);
}

void SlotGroups_Dialog::setup_layout()
{
    QVBoxLayout *slot_groups_v_layout = new QVBoxLayout(slot_groups_area_panel);
    slot_groups_v_layout->setContentsMargins(5,5,5,5);
    slot_groups_v_layout->setSpacing(5);

    // 添加6组按钮
    for(int group = 0; group < MAX_SLOTS; group++)
    {
        // 创建分组容器
        QWidget *group_widget = new QWidget();
        QVBoxLayout *group_layout = new QVBoxLayout(group_widget);
        group_layout->setContentsMargins(0, 0, 0, 0);
        group_layout->setSpacing(10);

        // 添加分组标题
        group_layout->addWidget(group_titles[group]);

        // 创建按钮水平布局
        QWidget *buttons_widget = new QWidget();
        QHBoxLayout *buttons_layout = new QHBoxLayout(buttons_widget);
        buttons_layout->setContentsMargins(0, 0, 0, 0);
        buttons_layout->setSpacing(10);

        // 添加6个按钮
        for(int btn = 0; btn < BUTTONS_PER_GROUP; btn++)
        {
            buttons_layout->addWidget(slot_buttons[group][btn]);
        }
        buttons_layout->addStretch();

        // 添加映射符标签和输入框
        buttons_layout->addWidget(mapping_labels[group]);
        buttons_layout->addWidget(mapping_edits[group]);

        group_layout->addWidget(buttons_widget);
        slot_groups_v_layout->addWidget(group_widget);

        // 在分组之间添加分隔线
        //if(group < MAX_SLOTS - 1)
        //{
            slot_groups_v_layout->addWidget(seprate_line());
        //}
    }

    // 新增：添加选槽颜色属性组
    QWidget *color_group_widget = new QWidget();
    QVBoxLayout *color_group_layout = new QVBoxLayout(color_group_widget);
    color_group_layout->setContentsMargins(0, 0, 0, 0);
    color_group_layout->setSpacing(10);

    // 添加颜色组标题
    color_group_layout->addWidget(slot_attr_title);

    // 创建颜色按钮水平布局容器
    QWidget *color_buttons_widget = new QWidget();
    QHBoxLayout *color_buttons_layout = new QHBoxLayout(color_buttons_widget);
    color_buttons_layout->setContentsMargins(0, 0, 0, 0);
    color_buttons_layout->setSpacing(10);

    // 添加6个颜色按钮
    for(int btn = 0; btn < BUTTONS_PER_GROUP; btn++)
    {
        color_buttons_layout->addWidget(slot_attr_buttons[btn]);
    }

    // 添加弹性空间，让按钮居中显示
    color_buttons_layout->addStretch();

    color_group_layout->addWidget(color_buttons_widget);
    slot_groups_v_layout->addWidget(color_group_widget);
    slot_groups_v_layout->addWidget(seprate_line());

    slot_groups_v_layout->addStretch();

    main_layout->addWidget(slot_groups_area_panel, 0, Qt::AlignHCenter);
    main_layout->addStretch();

    bottom_bar_layout->addStretch();
    bottom_bar_layout->addWidget(edit_btn, 0, Qt::AlignVCenter);
    bottom_bar_layout->addWidget(save_btn, 0, Qt::AlignVCenter);
    bottom_bar_layout->addWidget(cancel_btn, 0, Qt::AlignVCenter);
}

void SlotGroups_Dialog::set_stylesheet()
{
    slot_groups_area_panel->setStyleSheet(SS_MaintenanceItemPanels);
    // 设置按钮样式
    for(int group = 0; group < MAX_SLOTS; group++)
    {
        group_titles[group]->setStyleSheet(SS_slot_title);
        for(int btn = 0; btn < BUTTONS_PER_GROUP; btn++)
        {
            slot_buttons[group][btn]->setStyleSheet(SS_slot_btn);
        }

         // 设置映射符标签样式
         mapping_labels[group]->setStyleSheet(SS_slot_title);

         // 设置映射符输入框样式
         mapping_edits[group]->setStyleSheet(
                     R"(QLineEdit{
                        border: 1px solid #bdbdbd;
                        border-radius: 2px;
                        font-size: 20px;
                        font-family: '微软雅黑';
                        text-align: left;
                        color: #2a2d32;
                        padding-left:10px;
                        padding-right:10px;
                     }
                     QLineEdit:focus{
                        border: 2px solid #63d0f7;
                     }
                     QLineEdit:disabled{
                         background-color: #f5f5f7;
                     }
                     )"
         );

         slot_attr_buttons[group]->setStyleSheet(SS_slot_attr_btn);
         slot_attr_menu[group]->setStyleSheet(SS_slot_attr_menu);
    }

    slot_attr_title->setStyleSheet(SS_slot_title);
    edit_btn->setStyleSheet(SS_Primary_Menu_Btn);
    save_btn->setStyleSheet(SS_Primary_Menu_Btn);
    cancel_btn->setStyleSheet(SS_Normal_Menu_Btn);
}

void SlotGroups_Dialog::connect_signals_and_slots()
{
    connect(edit_btn, &QPushButton::clicked, this, &SlotGroups_Dialog::on_edit_btn_clicked);
    connect(save_btn, &QPushButton::clicked, this, &SlotGroups_Dialog::on_save_btn_clicked);
    connect(cancel_btn, &QPushButton::clicked, this, &SlotGroups_Dialog::on_cancel_btn_clicked);

    // 连接所有映射符编辑框的编辑完成信号
    for (int group = 0; group < MAX_SLOTS; group++) {
        connect(mapping_edits[group], &QLineEdit::editingFinished,
                this, &SlotGroups_Dialog::on_mapping_edit_finished);
    }
}

QWidget *SlotGroups_Dialog::create_item(const QString &item_name, QWidget *widget_a, QWidget *widget_b, QWidget *widget_c)
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

QWidget *SlotGroups_Dialog::create_item(QWidget *widget_a, QWidget *widget_b, QWidget *widget_c, QWidget *widget_d)
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

QFrame *SlotGroups_Dialog::seprate_line()
{
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFixedHeight(1);
    line->setStyleSheet(SS_SeparateLine);
    return line;
}

void SlotGroups_Dialog::setup_slots_menu_actions(ColorPushButton *menu_btn, QMenu *menu)
{
    ColorMarkLevelArgs args[MAX_SLOT_MENU_ACTIONS];
    for(int i=0; i<MAX_SLOT_MENU_ACTIONS; i++)
    {
        MaintenanceDialog::ReadColorMarkLevelArgs(i+1, &args[i]);
    }
    QAction *actions[MAX_SLOT_MENU_ACTIONS];

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
            // 只更新UI和内存中的状态，不写入INI文件
            menu_btn->setBackgroundColor(args[i].color_code);

            // 更新内存中的编辑状态
            QString hopperKey = QString("hopper_%1").arg(menu_btn->objectName());
            currentEditState.hopperColors[hopperKey] = i;
        });
    }
}

void SlotGroups_Dialog::set_last_selected_slot_attr_btn_color()
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
        slot_attr_buttons[i]->setBackgroundColor(args[color_index].color_code);
    }
    main_win_cfg_ini.endGroup();
}

// 映射符编辑完成信号槽
void SlotGroups_Dialog::on_mapping_edit_finished()
{
    validateMappingUniqueness();
}

// 校验映射符唯一性
bool SlotGroups_Dialog::validateMappingUniqueness()
{
    clearMappingHighlights();
    QMap<QString, QList<int>> mappingGroups; // 映射符 -> 分组索引列表

    // 收集所有非空的映射符
    for (int group = 0; group < MAX_SLOTS; group++)
    {
        QString mapping = mapping_edits[group]->text().trimmed();
        if (!mapping.isEmpty())
        {
            mappingGroups[mapping].append(group);
        }
    }

    bool hasDuplicates = false;

    // 检查重复的映射符并高亮显示
    for (auto it = mappingGroups.begin(); it != mappingGroups.end(); ++it)
    {
        if (it.value().size() > 1)
        {
            hasDuplicates = true;
            // 高亮所有重复的分组
            for (int groupIndex : it.value())
            {
                mapping_edits[groupIndex]->setStyleSheet(
                    "QLineEdit {"
                    "    border: 2px solid #ff0000;"
                    "    border-radius: 4px;"
                    "    padding: 5px;"
                    "    font-size: 20px;"
                    "    background-color: #fff0f0;"
                    "}"
                );
            }
        }
    }

    return !hasDuplicates;
}

// 高亮重复的映射符
void SlotGroups_Dialog::highlightDuplicateMappings()
{
    validateMappingUniqueness();
}

// 清除高亮显示
void SlotGroups_Dialog::clearMappingHighlights()
{
    for (int group = 0; group < MAX_SLOTS; group++)
    {
        mapping_edits[group]->setStyleSheet(
                    R"(QLineEdit{
                       border: 1px solid #bdbdbd;
                       border-radius: 2px;
                       font-size: 20px;
                       font-family: '微软雅黑';
                       text-align: left;
                       color: #2a2d32;
                       padding-left:10px;
                       padding-right:10px;
                    }
                    QLineEdit:focus{
                       border: 2px solid #63d0f7;
                    }
                    QLineEdit:disabled{
                        background-color: #f5f5f7;
                    }

                    )"
        );
    }
}
