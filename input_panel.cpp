#include "input_panel.h"
#include <Windows.h>
#include <QDebug>
#include <QValidator>
#include <QRegExp>
#include <QListView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QEvent>
#include <QPainter>
#include <QProxyStyle>
#include "edit_panel.h"


InputPanel::InputPanel(int panelId, QWidget *parent) : QWidget(parent)
{
    QApplication::setStyle(new QProxyStyle());
    attr.id = panelId;

    create_widget();

    setup_layout();
    set_stylesheet();
    connect_singals_and_slots();

    read_all_config();

    edit_list_menu_init();
}

InputPanel::~InputPanel()
{

//    // 确保所有输入框失去焦点，避免与触摸键盘的关联
//    input_line_edit_left->clearFocus();
//    input_line_edit_right->clearFocus();
//    prefix_line_edit->clearFocus();
//    suffix_line_edit->clearFocus();

//    // 断开所有事件过滤器
//    removeEventFilter(this);

//    // 清理定时器资源
//    if (m_menuTimer) {
//        m_menuTimer->stop();
//        delete m_menuTimer;
//        m_menuTimer = nullptr;
//    }

//    // 清理固定图标资源
//    if (fixed_icon) {
//        delete fixed_icon;
//        fixed_icon = nullptr;
//    }
}

void InputPanel::setPanelId(int panelId)
{
    attr.id = panelId;
    write_all_config();
}

void InputPanel::setPanelName(QString panelName)
{
    panel_name->setText(panelName);
    attr.name = panelName;
    write_all_config();
}



void InputPanel::setIsScannerInput(bool isScanner)
{
    s_isScannerInput = isScanner;
}

bool InputPanel::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::FocusIn)
    {

        qDebug() << "[InputPanel] QEvent::FocusIn======>>>>>>>";
        if(QLineEdit *lineEdit = qobject_cast<QLineEdit *>(obj))
        {

            // 只有在非扫描枪输入的情况下才显示触摸键盘
            if(!s_isScannerInput)
            {
                TouchKeyboardManager::instance().handleFocusIn(lineEdit);
                //TouchKeyboardManager::instance().requestShow(lineEdit);
            }

            // 无论是什么控件，获得焦点时都确保不被阻塞
            obj->setProperty("blocked", false);
            if(QWidget *widget = qobject_cast<QWidget *>(obj))
            {
                // 注意：不要在这里无条件调用allowAutoInvoke，否则会覆盖suppressTouchKeyboard的设置
                // 触摸键盘的自动调用设置应该由外部根据需要来控制
                widget->update();
                widget->setEnabled(true); // 确保控件启用
            }
        }
    }
    else if(event->type() == QEvent::FocusOut)
    {
        qDebug() << "[InputPanel] QEvent::FocusOut======>>>>>>>";
        // 当QLineEdit失去焦点时，与触摸键盘脱钩
        if(QLineEdit *lineEdit = qobject_cast<QLineEdit *>(obj))
        {
//            lineEdit->clearFocus();
//            TouchKeyboardController::handleFocusOut(lineEdit);
//            TouchKeyboardController::hide();

            //鼠标移动事件 时恢复正常鼠标
    //        while (QApplication::overrideCursor())
    //               QApplication::restoreOverrideCursor();

               QApplication::setOverrideCursor(Qt::ArrowCursor);
        }

    }

    if(event->type() == QEvent::MouseButtonPress)
    {
        emit signal_edit_focusIn();
    }

    return QWidget::eventFilter(obj, event);
}

void InputPanel::hideTouchKeyboard()
{

    // Touch keyboard modern UI window class
        HWND hwnd = FindWindow(L"IPTIP_Main_Window", nullptr);

        if (hwnd)
        {
            PostMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
            qDebug() << "关闭触摸键盘窗口成功";
        }
        else
        {
            qDebug() << "未找到触摸键盘窗口";
        }
}


void InputPanel::suppressTouchKeyboard()
{
        // 调用TouchKeyboardManager的静态方法为当前面板设置属性
        TouchKeyboardManager::suppressTouchKeyboard(this);
        
        // 为所有输入框设置相同的属性
        TouchKeyboardManager::suppressTouchKeyboard(input_line_edit_left);
        TouchKeyboardManager::suppressTouchKeyboard(input_line_edit_right);
        TouchKeyboardManager::suppressTouchKeyboard(prefix_line_edit);
        TouchKeyboardManager::suppressTouchKeyboard(suffix_line_edit);
}

void InputPanel::allowTouchKeyboard()
{
        // 调用TouchKeyboardController的静态方法为当前面板设置属性
        TouchKeyboardManager::allowTouchKeyboard(this);
        
        // 为所有输入框设置相同的属性
        TouchKeyboardManager::allowTouchKeyboard(input_line_edit_left);
        TouchKeyboardManager::allowTouchKeyboard(input_line_edit_right);
        TouchKeyboardManager::allowTouchKeyboard(prefix_line_edit);
        TouchKeyboardManager::allowTouchKeyboard(suffix_line_edit);
}

void InputPanel::create_widget()
{
    this->setFixedWidth(PANEL_SIZE_W);

    main_panel = new QWidget();
    main_panel->setFixedWidth(PANEL_SIZE_W);

    left_part = new QWidget();
    left_part->setFixedWidth(125);
    right_part = new QWidget();
    right_part->setFixedWidth(295);

    left_top_container = new QWidget();
    left_top_container->setFixedWidth(125);
    left_bottom_container = new QWidget();
    left_bottom_container->setFixedWidth(125);

    right_top_container = new QWidget();
    right_top_container->setFixedWidth(295);
    right_bottom_container = new QWidget();
    right_bottom_container->setFixedWidth(295);

    single_or_couple_btn = new QPushButton();
    single_or_couple_btn->setCheckable(true);
//    single_or_couple_btn->setText("[1] [2]");
    single_or_couple_btn->setFixedHeight(16);
    single_or_couple_btn->installEventFilter(this);

    prefix_line_edit = new QLineEdit();
    prefix_line_edit->setFixedSize(105,25);
    prefix_line_edit->setAlignment(Qt::AlignCenter);
    prefix_line_edit->setEnabled(false);
    prefix_line_edit->installEventFilter(this);

    suffix_line_edit = new QLineEdit();
    suffix_line_edit->setFixedSize(105,25);
    suffix_line_edit->setAlignment(Qt::AlignCenter);
    suffix_line_edit->setEnabled(false);
    suffix_line_edit->installEventFilter(this);

    prefix_btn = new QPushButton();
    prefix_btn->setCheckable(true);
    prefix_btn->setFixedSize(40,25);
    prefix_btn->setText(tr("前缀"));//前缀Prefix
    prefix_btn->setFocusPolicy(Qt::NoFocus);
    prefix_btn->installEventFilter(this);

    suffix_btn = new QPushButton();
    suffix_btn->setCheckable(true);
    suffix_btn->setFixedSize(40,25);
    suffix_btn->setText(tr("后缀"));//后缀Suffix
    suffix_btn->setFocusPolicy(Qt::NoFocus);
    suffix_btn->installEventFilter(this);

    ime_btn = new QPushButton();
    ime_btn->setCheckable(true);
    ime_btn->setFixedSize(20,15);
    ime_btn->setText("IME");
    ime_btn->setFocusPolicy(Qt::NoFocus);
    ime_btn->installEventFilter(this);

    panel_name = new QPushButton();
    panel_name->setFixedHeight(35);
    panel_name->setFocusPolicy(Qt::NoFocus);
    panel_name->installEventFilter(this);

    property_menu = new QMenu();
    property_menu->setFixedWidth(350);
    setup_property_menu();

    panel_name->setMenu(property_menu);

    retain_btn = new QPushButton();
    retain_btn->setCheckable(true);
    retain_btn->setFixedSize(15,15);
    retain_btn->setText("*");
    retain_btn->setFocusPolicy(Qt::NoFocus);
    retain_btn->installEventFilter(this);

    input_line_edit_left = new QLineEdit();
    input_line_edit_left->setFixedHeight(35);
    input_line_edit_left->setAlignment(Qt::AlignCenter);
    input_line_edit_left->installEventFilter(this);

    input_line_hyphen = new QLabel();
    //input_line_hyphen->setFixedSize(16, 35);
    input_line_hyphen->setText("-");
    input_line_hyphen->setFixedHeight(35);
    input_line_hyphen->setMaximumWidth(80);
    input_line_hyphen->setAlignment(Qt::AlignCenter);

    input_line_edit_right = new QLineEdit();
    input_line_edit_right->setFixedHeight(35);
    input_line_edit_right->setAlignment(Qt::AlignCenter);
    input_line_edit_right->installEventFilter(this);


    plus_btn = new QPushButton("+1");
    plus_btn->setFixedSize(30,35);
    plus_btn->setFocusPolicy(Qt::NoFocus);
    plus_btn->installEventFilter(this);

    auto_plus_btn = new QPushButton("+1\nAuto");
    auto_plus_btn->setCheckable(true);
    auto_plus_btn->setFixedSize(30,35);
    auto_plus_btn->setFocusPolicy(Qt::NoFocus);
    auto_plus_btn->installEventFilter(this);

    input_list_menu = new QMenu();
    input_list_menu->setFixedWidth(295);

    // 创建容器Widget
    QWidget* input_list_container = new QWidget(input_list_menu);
    QVBoxLayout* input_list_widget_layout = new QVBoxLayout(input_list_container);
    input_list_widget_layout->setContentsMargins(0, 0, 0, 0);

    // 创建QListWidget
    input_list_widget = new QListWidget(input_list_container);
    input_list_widget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    input_list_widget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    input_list_widget->setSelectionMode(QAbstractItemView::SingleSelection);
    input_list_widget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    input_list_widget->setSelectionMode(QListView::NoSelection);
    input_list_widget_layout->addWidget(input_list_widget);

    // 创建WidgetAction
    QWidgetAction* widget_action = new QWidgetAction(input_list_menu);
    widget_action->setDefaultWidget(input_list_container);
    input_list_menu->addAction(widget_action);

    input_list_btn = new QPushButton();
    input_list_btn->setFixedSize(30,35);
    input_list_btn->setFocusPolicy(Qt::NoFocus);
    input_list_btn->setIcon(QIcon(":/image/down_arrow_.png"));
    //input_list_btn->installEventFilter(this);

    property_btn = new QPushButton(tr("点击设置该录入框属性"), main_panel);
    property_btn->setGeometry(0, 0, main_panel->width(), main_panel->height());
    property_btn->setFocusPolicy(Qt::NoFocus);
    property_btn->setVisible(false);

    input_line_edit_left->setMaxLength(50);
    input_line_edit_right->setMaxLength(50);
}

void InputPanel::refreshPropertyText()
{
    property_btn->setText(tr("点击设置该录入框属性"));
    //setup_property_menu();
}

void InputPanel::setup_layout()
{
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(0,0,0,0);
    main_layout->setSpacing(0);
    main_layout->addWidget(main_panel);

    QHBoxLayout *main_panel_layout = new QHBoxLayout(main_panel);
    main_panel_layout->setContentsMargins(5,5,5,5);
    main_panel_layout->setSpacing(5);
    main_panel_layout->addWidget(left_part);
    main_panel_layout->addWidget(right_part);

    QVBoxLayout *left_part_layout = new QVBoxLayout(left_part);
    left_part_layout->setContentsMargins(0,0,0,0);
    left_part_layout->setSpacing(0);
    left_part_layout->addWidget(left_top_container);
    left_part_layout->addWidget(left_bottom_container);

    QVBoxLayout *right_part_layout = new QVBoxLayout(right_part);
    right_part_layout->setContentsMargins(0,0,0,0);
    right_part_layout->setSpacing(2);
    right_part_layout->addWidget(right_top_container);
    right_part_layout->addWidget(right_bottom_container);

    QHBoxLayout *left_top_container_layout = new QHBoxLayout(left_top_container);
    left_top_container_layout->setContentsMargins(0,0,0,0);
    left_part_layout->setSpacing(0);
    left_top_container_layout->addWidget(single_or_couple_btn);

//    QHBoxLayout *left_bottom_container_layout = new QHBoxLayout(left_bottom_container);
//    left_bottom_container_layout->setContentsMargins(0,0,0,0);
//    left_part_layout->setSpacing(0);
//    left_bottom_container_layout->addWidget(ime_btn);
//    left_bottom_container_layout->addWidget(panel_name);
//    left_bottom_container_layout->addWidget(retain_btn);

    QVBoxLayout *left_bottom_container_left_layout = new QVBoxLayout(right_part);
    left_bottom_container_left_layout->setContentsMargins(0,0,0,0);
    left_bottom_container_left_layout->setSpacing(0);
    left_bottom_container_left_layout->addWidget(retain_btn);
    left_bottom_container_left_layout->addWidget(ime_btn);

    QHBoxLayout *left_bottom_container_layout = new QHBoxLayout(left_bottom_container);
    left_bottom_container_layout->setContentsMargins(0,0,0,0);
    left_bottom_container_layout->setSpacing(0);
    left_bottom_container_layout->addLayout(left_bottom_container_left_layout);
    left_bottom_container_layout->addWidget(panel_name);

    QHBoxLayout *right_top_container_layout = new QHBoxLayout(right_top_container);
    right_top_container_layout->setContentsMargins(0,0,0,0);
    right_top_container_layout->setSpacing(0);
    right_top_container_layout->addWidget(prefix_btn);
    right_top_container_layout->addWidget(prefix_line_edit);
    right_top_container_layout->addStretch();
    right_top_container_layout->addWidget(suffix_line_edit);
    right_top_container_layout->addWidget(suffix_btn);

    QHBoxLayout *input_line_edit_layout = new QHBoxLayout();
    input_line_edit_layout->setContentsMargins(0,0,0,0);
    input_line_edit_layout->setSpacing(0);
    input_line_edit_layout->addWidget(input_line_edit_left);
    input_line_edit_layout->addWidget(input_line_hyphen);
    input_line_edit_layout->addWidget(input_line_edit_right);

    QHBoxLayout *right_bottom_container_layout = new QHBoxLayout(right_bottom_container);
    right_bottom_container_layout->setContentsMargins(0,0,0,0);
    right_bottom_container_layout->setSpacing(2);
    right_bottom_container_layout->addLayout(input_line_edit_layout);
    right_bottom_container_layout->addWidget(plus_btn);
    right_bottom_container_layout->addWidget(auto_plus_btn);
    right_bottom_container_layout->addWidget(input_list_btn);
}
void InputPanel::set_stylesheet()
{
    main_panel->setStyleSheet(SS_InputPanel);
    left_part->setStyleSheet(SS_left_part);
    right_part->setStyleSheet(SS_right_part);

    single_or_couple_btn->setStyleSheet(SS_single_or_couple_btn);
    prefix_line_edit->setStyleSheet(SS_prefix_line_edit_disable);
    suffix_line_edit->setStyleSheet(SS_suffix_line_edit_disable);
    prefix_btn->setStyleSheet(SS_prefix_btn);

    suffix_btn->setStyleSheet(SS_suffix_btn);
    ime_btn->setStyleSheet(SS_ime_btn);
    panel_name->setStyleSheet(SS_panel_name);
    retain_btn->setStyleSheet(SS_retain_btn);

    input_line_edit_left->setStyleSheet(SS_input_line_edit_left);
    input_line_hyphen->setStyleSheet(SS_input_line_hyphen);
    input_line_edit_right->setStyleSheet(SS_input_line_edit);

    plus_btn->setStyleSheet(SS_plus_btn);
    auto_plus_btn->setStyleSheet(SS_auto_plus_btn);
    input_list_btn->setStyleSheet(SS_input_list_btn);
    input_list_menu->setStyleSheet(SS_input_list_menu);
    input_list_widget->setStyleSheet(SS_input_list_widget);
    property_btn->setStyleSheet(SS_property_btn);

    property_menu->setStyleSheet(SS_input_list_menu);
}

void InputPanel::connect_singals_and_slots()
{
    connect(single_or_couple_btn, &QPushButton::clicked, this, &InputPanel::on_single_or_couple_btn_clicked);
    connect(plus_btn, &QPushButton::clicked, this, &InputPanel::on_plus_btn_clicked);
    connect(auto_plus_btn, &QPushButton::clicked, this, &InputPanel::on_auto_plus_btn_clicked);
    connect(prefix_btn, &QPushButton::clicked, this, &InputPanel::on_prefix_btn_clicked);
    connect(suffix_btn, &QPushButton::clicked, this, &InputPanel::on_suffix_btn_clicked);
    connect(prefix_line_edit, &QLineEdit::textChanged, this, &InputPanel::on_input_text_changed);
    connect(suffix_line_edit, &QLineEdit::textChanged, this, &InputPanel::on_input_text_changed);
    connect(input_line_edit_left, &QLineEdit::textChanged, this, &InputPanel::on_input_text_changed);
    connect(input_line_edit_right, &QLineEdit::textChanged, this, &InputPanel::on_input_text_changed);

    connect(retain_btn, &QPushButton::clicked, this, &InputPanel::on_retain_btn_clicked);
    connect(ime_btn, &QPushButton::toggled, this, &InputPanel::on_ime_btn_clicked);

    connect(property_btn, &QPushButton::clicked, this, &InputPanel::on_property_btn_clicked);

    connect(input_list_btn, &QPushButton::clicked, this, &InputPanel::on_edit_list_btn_clicked);


}

void InputPanel::on_single_or_couple_btn_clicked()
{
    attr.couple_enable = single_or_couple_btn->isChecked();
    write_config("couple_enable", attr.couple_enable);
}

void InputPanel::on_ime_btn_clicked()
{
    if(ime_btn->isChecked())
    {
        input_line_edit_left->setAttribute(Qt::WA_InputMethodEnabled, true);
        input_line_edit_right->setAttribute(Qt::WA_InputMethodEnabled, true);

    }
    else
    {
        input_line_edit_left->setAttribute(Qt::WA_InputMethodEnabled, false);
        input_line_edit_right->setAttribute(Qt::WA_InputMethodEnabled, false);
    }

    attr.ime_enable = ime_btn->isChecked();
    ime_action->setChecked(attr.ime_enable);
    write_config("ime_enable", attr.ime_enable);
}

void InputPanel::on_ime_btn_toggled(bool checked)
{
    if(checked)
    {
        input_line_edit_left->setAttribute(Qt::WA_InputMethodEnabled, true);
        input_line_edit_right->setAttribute(Qt::WA_InputMethodEnabled, true);

    }
    else
    {
        input_line_edit_left->setAttribute(Qt::WA_InputMethodEnabled, false);
        input_line_edit_right->setAttribute(Qt::WA_InputMethodEnabled, false);
    }

    attr.ime_enable = ime_btn->isChecked();
    ime_action->setChecked(attr.ime_enable);
    write_config("ime_enable", attr.ime_enable);
}

void InputPanel::on_retain_btn_clicked()
{
    attr.retain_enable = retain_btn->isChecked();
    retain_action->setChecked(attr.retain_enable);
    write_config("retain_enable", attr.retain_enable);
}

void InputPanel::setPropBtnVisible(bool isVisible)
{
    property_btn->setFixedSize(main_panel->width(), main_panel->height());
    property_btn->setVisible(isVisible);
    property_btn->raise();

}

void InputPanel::clearInputText()
{
    input_line_edit_left->setText("");
    input_line_edit_right->setText("");
}

int InputPanel::getPanelId()
{
    return attr.id;
}

void InputPanel::on_property_btn_clicked()
{
    emit property_btn_Clicked();

    InputPanelPropDialog *property_dialog = new InputPanelPropDialog(&attr);
    if(property_dialog->exec() == QDialog::Accepted)
    {
        setPanelAttribute();
        write_all_config();
        emit property_btn_saved();
    }
}

void InputPanel::on_prefix_btn_clicked()
{
    if(prefix_btn->isChecked())
    {
        prefix_line_edit->setEnabled(true);
        prefix_line_edit->setStyleSheet(SS_prefix_line_edit_enable);
        prefix_line_edit->setFocus();
    }
    else
    {
        prefix_line_edit->setEnabled(false);
        prefix_line_edit->setText("");
        prefix_line_edit->setStyleSheet(SS_prefix_line_edit_disable);
    }
}

void InputPanel::on_suffix_btn_clicked()
{

    if(suffix_btn->isChecked())
    {
        suffix_line_edit->setEnabled(true);
        suffix_line_edit->setStyleSheet(SS_suffix_line_edit_enable);
        suffix_line_edit->setFocus();
    }
    else
    {
        suffix_line_edit->setEnabled(false);
        suffix_line_edit->setText("");
        suffix_line_edit->setStyleSheet(SS_suffix_line_edit_disable);
    }

}

void InputPanel::on_input_text_changed()
{
    QString text;

    text.append(prefix_line_edit->text());
    text.append(input_line_edit_left->text());
    if(input_line_edit_left->text()!="" && input_line_edit_right->text()!="")
    {
        text.append(connector_symbol);
    }
    text.append(input_line_edit_right->text());
    text.append(suffix_line_edit->text());

    emit input_text_Changed(text);
}

void InputPanel::on_plus_btn_clicked()
{

    if(this->input_line_edit_left->text() == "" && this->input_line_edit_right->text() == "")
        return;

    QLineEdit *current_line_edit;
    QString currentText;
    QChar last_char;

    if(input_line_edit_right->text() != "")
    {
        current_line_edit = input_line_edit_right;
    }
    else
    {
        current_line_edit = input_line_edit_left;
    }

    currentText = current_line_edit->text();
    last_char = currentText.back();

    if(last_char.isDigit())
    {//是数字
        current_line_edit->setText(incTailNumber(currentText));
    }
    else if(isAlpha(last_char))
    {//是字母
        current_line_edit->setText(incTailAlpha(currentText));
    }
}

void InputPanel::on_auto_plus_btn_clicked()
{
    attr.auto_plus_btn_enable = auto_plus_btn->isChecked();
    write_config("auto_plus_btn_enable", attr.auto_plus_btn_enable);
}


void InputPanel::on_edit_list_btn_clicked()
{    

    int list_widget_height = 0;
    for (int i = input_list_widget->count()-1; i >= 0; i--)
    {
        list_widget_height += input_list_widget->sizeHintForRow(0);
    }

    input_list_widget->setFixedHeight(list_widget_height > MAX_EDIT_LIST_HEIGHT? MAX_EDIT_LIST_HEIGHT: list_widget_height);
    input_list_menu->setFixedHeight(4 + (list_widget_height > MAX_EDIT_LIST_HEIGHT? MAX_EDIT_LIST_HEIGHT: list_widget_height));

    QPoint menuPos = input_list_btn->mapToGlobal(QPoint(-(295-input_list_btn->width()), 2+input_list_btn->height()));
    input_list_btn->setDown(true);
    input_list_menu->exec(menuPos);
    input_list_btn->setDown(false);
}

/*
void InputPanel::on_edit_list_btn_clicked()
{
    static bool menuVisible = false;

    if (menuVisible)
    {
        input_list_menu->hide();
        input_list_btn->setDown(false);
        menuVisible = false;
        return;
    }

    // 计算列表高度
    int list_widget_height = 0;
    int itemCount = input_list_widget->count();

    if (itemCount > 0)
    {
        list_widget_height = input_list_widget->sizeHintForRow(0) * itemCount;
    }

    int finalHeight = qMin(list_widget_height, MAX_EDIT_LIST_HEIGHT);
    input_list_widget->setFixedHeight(finalHeight);
    input_list_menu->setFixedHeight(4 + finalHeight);

    QPoint menuPos = input_list_btn->mapToGlobal(QPoint(-(295 - input_list_btn->width()), 2 + input_list_btn->height()));

    input_list_btn->setDown(true);
    input_list_menu->exec(menuPos);
    input_list_btn->setDown(false);
    menuVisible = true;
}
*/


void InputPanel::read_all_config()
{
    QSettings config_ini(QString("./input_panel_config.ini"), QSettings::IniFormat);
    config_ini.beginGroup(QString("INPUT_PANEL_%1").arg(attr.id));
    attr.name = config_ini.value("name", "Name").toString();
    attr.ime_enable = config_ini.value("ime_enable", false).toBool();
    attr.retain_enable = config_ini.value("retain_enable", false).toBool();
    attr.pre_suf_visible = config_ini.value("pre_suf_visible", false).toBool();
    attr.single_or_couple_visible = config_ini.value("single_or_couple_visible", false).toBool();
    attr.couple_enable = config_ini.value("couple_enable", false).toBool();
    attr.two_input_box_enable = config_ini.value("two_input_box_enable", false).toBool();
    attr.plus_btn_type = config_ini.value("plus_btn_type", 0).toInt();
    attr.auto_plus_btn_enable = config_ini.value("auto_plus_btn_enable", false).toBool();
    config_ini.endGroup();

    setPanelAttribute();
}

void InputPanel::write_all_config()
{
    QSettings config_ini("input_panel_config.ini", QSettings::IniFormat);
    config_ini.beginGroup(QString("INPUT_PANEL_%1").arg(attr.id));
    config_ini.setValue("name", QString(attr.name));
    config_ini.setValue("ime_enable", attr.ime_enable);
    config_ini.setValue("retain_enable", attr.retain_enable);
    config_ini.setValue("pre_suf_visible", attr.pre_suf_visible);
    config_ini.setValue("single_or_couple_visible", attr.single_or_couple_visible);
    config_ini.setValue("couple_enable", attr.couple_enable);
    config_ini.setValue("two_input_box_enable", attr.two_input_box_enable);
    config_ini.setValue("plus_btn_type", attr.plus_btn_type);
    config_ini.setValue("auto_plus_btn_enable", attr.auto_plus_btn_enable);
    config_ini.endGroup();
}

void InputPanel::write_config(const QString &key, const QVariant &value)
{
    QSettings config_ini("input_panel_config.ini", QSettings::IniFormat);
    config_ini.beginGroup(QString("INPUT_PANEL_%1").arg(attr.id));
    config_ini.setValue(key, value);
    config_ini.endGroup();
}

void InputPanel::setPanelAttribute()
{

    //单双号规则按钮（只有载玻片打印机有）
    single_or_couple_btn->setVisible(attr.single_or_couple_visible);


    single_or_couple_btn->setChecked(attr.couple_enable);

    //有无前后导输入框
    right_top_container->setVisible(attr.pre_suf_visible);
    pre_suf_box_action->setChecked(attr.pre_suf_visible);
    if(!attr.pre_suf_visible)
    {
        prefix_line_edit->setText("");
        suffix_line_edit->setText("");
    }

    //显示名称
    panel_name->setText(attr.name);

    //是否允许输入法
    ime_btn->setChecked(attr.ime_enable);
    ime_action->setChecked(attr.ime_enable);
    if(attr.ime_enable)
    {
        input_line_edit_left->setAttribute(Qt::WA_InputMethodEnabled, true);
        input_line_edit_right->setAttribute(Qt::WA_InputMethodEnabled, true);
    }
    else
    {
        input_line_edit_left->setAttribute(Qt::WA_InputMethodEnabled, false);
        input_line_edit_right->setAttribute(Qt::WA_InputMethodEnabled, false);
    }


    //是否保持上次输入内容
    retain_btn->setChecked(attr.retain_enable);
    retain_action->setChecked(attr.retain_enable);

    //单双输入框
    if(!attr.two_input_box_enable)
    {
        input_line_edit_left->setVisible(false);
        input_line_hyphen->setVisible(false);
        input_line_edit_right->setVisible(true);
        input_line_edit_right->setStyleSheet(SS_input_line_edit);
    }
    else
    {
        input_line_edit_left->setVisible(true);
        input_line_hyphen->setVisible(true);
        input_line_edit_right->setVisible(true);
        input_line_edit_right->setStyleSheet(SS_input_line_edit_right);
    }
    two_input_box_action->setChecked(attr.two_input_box_enable);

    //加一按钮类型（0:无按钮, 1:"+1", 2:"+1 Auto"）
    if(attr.plus_btn_type == 1)
    {
        plus_btn->setVisible(true);
        auto_plus_btn->setVisible(false);
        plus_btn_action->setChecked(true);
    }
    else if(attr.plus_btn_type == 2)
    {
        auto_plus_btn->setChecked(attr.auto_plus_btn_enable);
        auto_plus_btn->setVisible(true);
        plus_btn->setVisible(false);
        plus_auto_btn_action->setChecked(true);
    }
    else
    {
        plus_btn->setVisible(false);
        auto_plus_btn->setVisible(false);
        plus_btn_action->setChecked(false);
        plus_auto_btn_action->setChecked(false);
    }
}

qulonglong InputPanel::getTextCount()
{
    qulonglong cnt = 0;
    QString     input_box_text;
    QStringList input_text_list;

    if(!attr.two_input_box_enable)
    {//单输入框
        input_box_text = input_line_edit_right->text();
    }
    else
    {//双输入框
        if(input_line_edit_left->text() == "" || input_line_edit_right->text() == "")
        {
            input_box_text = QString("%1%2").arg(input_line_edit_left->text(), input_line_edit_right->text());
        }
        else
        {
            input_box_text = QString("%1%2%3").arg(input_line_edit_left->text(), connector_symbol, input_line_edit_right->text());
        }
    }
    if(input_box_text == "")
    {
        return 0;
    }

    //字符识别,先以分隔符"."分隔，再以"-"识别是否是多号连续
    QStringList text_split_list;
    if(delimiter_symbol != "")
    {
        text_split_list = input_box_text.split(delimiter_symbol);
    }
    else
    {
        text_split_list.append(input_box_text);
    }

    for (const QString &str:text_split_list)
    {
        if(str.count(connector_symbol) == 1)
        {//多号连续
            QStringList text_split = str.split(connector_symbol);
            QChar last_char_left = text_split[0].back();
            QChar last_char_right = text_split[1].back();
            qulonglong seq_cnt = 0;

            if(isAlpha(last_char_left) && isAlpha(last_char_right))
            {//结尾都是字母
                seq_cnt = countAlphaTailSequence(text_split[0], text_split[1]);
            }
            else if (last_char_left.isDigit() && last_char_right.isDigit())
            {//结尾都是数字
                seq_cnt = countNumberTailSequence(text_split[0], text_split[1]);
            }

            if(seq_cnt > 0)
            {
                cnt += seq_cnt;
            }
            else
            {
                cnt ++;
            }
        }
        else
        {//非连续号
            cnt ++;
        }
    }
    return cnt;
}

QStringList InputPanel::getText()
{
    QString     input_box_text;
    QStringList input_text_list;

    if(!attr.two_input_box_enable)
    {//单输入框
        input_box_text = input_line_edit_right->text();
    }
    else
    {//双输入框
        if(input_line_edit_left->text() == "" || input_line_edit_right->text() == "")
        {
            input_box_text = QString("%1%2").arg(input_line_edit_left->text(), input_line_edit_right->text());
        }
        else
        {
            input_box_text = QString("%1%2%3").arg(input_line_edit_left->text(), connector_symbol, input_line_edit_right->text());
        }
    }

    //输入记录增加到下拉列表并保存到文件
    if(input_box_text != "" && input_box_text != connector_symbol)
    {
        add_edit_list_item(input_box_text, HISTORY_ITEM_TYPE);
        save_history_list_item_to_file(input_box_text);
    }

    //字符识别,先以分隔符"."分隔，再以"-"识别是否是多号连续
    QStringList text_split_list;
    if(delimiter_symbol != "")
    {
        text_split_list = input_box_text.split(delimiter_symbol);
    }
    else
    {
        text_split_list.append(input_box_text);
    }

    for (const QString &str : text_split_list)
    {
        if(str.count(connector_symbol) == 1)
        {//多号连续
            QStringList text_split = str.split(connector_symbol);
            QChar last_char_left = text_split[0].back();
            QChar last_char_right = text_split[0].back();
            QStringList seq_list;

            if(isAlpha(last_char_left) && isAlpha(last_char_right))
            {//结尾都是字母
                seq_list = generateAlphaTailSequence(text_split[0], text_split[1]);
            }
            else if (last_char_left.isDigit() && last_char_right.isDigit())
            {//结尾都是数字
                seq_list = generateNumberTailSequence(text_split[0], text_split[1]);
            }

            if(!seq_list.isEmpty())
            {
                input_text_list.append(seq_list);
            }
            else
            {
                input_text_list.append(str);
            }
        }
        else
        {//非连续号
            input_text_list.append(str);
        }
    }

    if(attr.single_or_couple_visible && attr.couple_enable)
    {//两号合并
        for (int i=0; i<input_text_list.size()-1 ; i++)
        {
            input_text_list[i] = input_text_list[i] + merge_symbol + input_text_list[i+1];
            input_text_list.removeAt(i+1);
        }
    }

    if(attr.pre_suf_visible)
    {//增加前导后导内容
        for (QString &str:input_text_list)
        {
            str.prepend(prefix_line_edit->text());
            str.append(suffix_line_edit->text());
        }
    }

    if(attr.plus_btn_type == 2 && auto_plus_btn->isChecked())
    {//被获取后自动+1回填
        on_plus_btn_clicked();
    }
    else
    {
        if(!attr.retain_enable)
        {//不保留输入框内容
            input_line_edit_left->setText("");
            input_line_edit_right->setText("");
        }
    }

    qDebug() << input_text_list;
    return input_text_list;
}

QString InputPanel::getEditText()
{
    QString text;

    text.append(prefix_line_edit->text());
    text.append(input_line_edit_left->text());
    if(input_line_edit_left->text()!="" && input_line_edit_right->text()!="")
    {
        text.append(connector_symbol);
    }
    text.append(input_line_edit_right->text());
    text.append(suffix_line_edit->text());

    return text;
}

//void InputPanel::edit_list_menu_init()
//{
//    //增加清空项
//    clear_action = new QAction(tr("<--------清空-------->"), this);
//    connect(clear_action, &QAction::triggered, this, [this]{
//        QList <QAction *> actions = input_list_menu->actions();
//        for (QList<QAction *>::iterator it = actions.begin(); it!=actions.end(); ++it)
//        {
//            QAction * action = *it;
//            if(action->text() != clear_action->text())
//            {
//                input_list_menu->removeAction(action);
//                delete action;
//            }
//        }
//        //清空文件内容
//        clear_edit_list_to_file();
//    });
//    input_list_menu->addAction(clear_action);

//    //读取保存的文件
//    load_edit_list_from_file();
//}

void InputPanel::edit_list_menu_init()
{
    // FIXED_ICON init
    fixed_icon = new QPixmap(16, 16);
    fixed_icon->fill(Qt::transparent);
    QPainter painter;
    painter.begin(fixed_icon);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(QString("#bdbdbd")));
    painter.drawEllipse(4,4,8,8);
    painter.end();

    clear_item = new QListWidgetItem(tr("<--------清空-------->"));
    clear_item->setData(Qt::UserRole, CLEAR_ITEM_TYPE);
    input_list_widget->addItem(clear_item);

    connect(input_list_widget, &QListWidget::itemClicked, [this](QListWidgetItem *item)
    {
        if(CLEAR_ITEM_TYPE == item->data(Qt::UserRole).toUInt())
        {
            for(int i = input_list_widget->count()-1; i >= 0; i--)
            {
                QListWidgetItem *item = input_list_widget->item(i);
                if(item->data(Qt::UserRole).toUInt() == HISTORY_ITEM_TYPE)
                {
                    delete input_list_widget->takeItem(i);  // 正确删除方法
                }
            }
            //清空文件内容
            clear_history_list_to_file();
        }
        else
        {
            if(attr.two_input_box_enable && item->text().count(connector_symbol) == 1)
            {//双输入框&双输入框记录
                QStringList text_split = item->text().split(connector_symbol);
                input_line_edit_left->setText(text_split.at(0));
                input_line_edit_right->setText(text_split.at(1));
            }
            else
            {//其它情况直接右框追加记录
                if(!input_line_edit_right->text().isEmpty())
                {
                    input_line_edit_right->setText(input_line_edit_right->text() + delimiter_symbol + item->text());
                }
                else
                {
                    input_line_edit_right->setText(item->text());
                }
            }
        }
        input_list_menu->close();
    });

    // 读取保存的文件
    load_history_list_from_file();
}


//void InputPanel::add_edit_list_item(const QString &text)
//{
//    //有重复的删除再插入
//    QList<QAction*> actions = input_list_menu->actions();
//    for ( QAction* action : actions)
//    {
//        if(action->text() == text)
//        {
//            input_list_menu->removeAction(action);
//            delete action;
//        }
//    }

//    //超出量大数量删除最早的再插入
//    if(actions.size() >= MAX_EDIT_LIST + 1)
//    {
//        input_list_menu->removeAction(actions.at(actions.size()-2));
//        delete actions.at(actions.size()-2);
//    }

//    QAction *action = new QAction(text, this);
//    connect(action, &QAction::triggered, [this, action]
//    {
//        if(attr.two_input_box_enable && action->text().count(connector_symbol) == 1)
//        {//双输入框&双输入框记录
//            QStringList text_split = action->text().split(connector_symbol);
//            input_line_edit_left->setText(text_split.at(0));
//            input_line_edit_right->setText(text_split.at(1));
//        }
//        else
//        {//其它情况直接右框追加记录
//            if(!input_line_edit_right->text().isEmpty())
//            {
//                input_line_edit_right->setText(input_line_edit_right->text() + delimiter_symbol + action->text());
//            }
//            else
//            {
//                input_line_edit_right->setText(action->text());
//            }
//        }
//    });

//    if(!input_list_menu->actions().isEmpty())
//    {
//        input_list_menu->insertAction(input_list_menu->actions().first(), action);
//    }
//    else
//    {
//        input_list_menu->addAction(action);
//    }
//}
void InputPanel::add_edit_list_item(const QString &text, ITEM_TYPE type)
{
    QListWidgetItem *new_item = new QListWidgetItem(text);
    new_item->setData(Qt::UserRole, type);

    if(FIXED_ITEM_TYPE == type)
    {
        new_item->setIcon(QIcon(*fixed_icon));      // 置顶内容加ICON
        input_list_widget->insertItem(0, new_item);
    }
    else if(HISTORY_ITEM_TYPE == type)
    {
        // 有重复，先删除
        for (int i = input_list_widget->count()-1; i>=0; i--)
        {
            if(text == input_list_widget->item(i)->text()
                && HISTORY_ITEM_TYPE == input_list_widget->item(i)->data(Qt::UserRole).toInt())
            {
                delete input_list_widget->takeItem(i);
            }
        }
        input_list_widget->insertItem(fixed_items_cnt, new_item);
    }

    // 超出最大数量限制，删除最早的
    if(input_list_widget->count() > MAX_EDIT_LIST + 1)  // 算上删除按钮
    {
        delete input_list_widget->takeItem(input_list_widget->count() - 1 - 1); // 列表倒数第二个
    }
}

void InputPanel::load_preset_list_from_file()
{
    QSettings preset_input_ini(PRESET_INPUT_INI, QSettings::IniFormat);
    preset_input_ini.beginGroup("preset_input");
    QString preset_text = preset_input_ini.value("text", "").toString();
    preset_input_ini.endGroup();

    fixed_items_cnt = 0;
    QStringList preset_list;
    if(preset_text.size() != 0)
    {
        preset_list = preset_text.split("\n");
        fixed_items_cnt = preset_list.size();
        if(fixed_items_cnt > MAX_FIXED_ITEM)
        {
            fixed_items_cnt = MAX_FIXED_ITEM;
        }

        for(int i = fixed_items_cnt-1; i >= 0; --i)
        {
            add_edit_list_item(preset_list[i], FIXED_ITEM_TYPE);
        }
    }
}

void InputPanel::clear_preset_list()
{
    for(int i = input_list_widget->count()-1; i >= 0; i--)
    {
        QListWidgetItem *item = input_list_widget->item(i);
        if(item->data(Qt::UserRole).toUInt() == FIXED_ITEM_TYPE)
        {
            delete input_list_widget->takeItem(i);  // 正确删除方法
        }
    }
    fixed_items_cnt = 0;
}

void InputPanel::load_history_list_from_file()
{
    QSettings edit_list_ini(EDIT_LIST_INI, QSettings::IniFormat);
    edit_list_ini.beginGroup("EditList");
    QString key = QString("Panel_%1").arg(attr.id);
    QString edit_list_str = edit_list_ini.value(key, "").toString();
    QStringList edit_list_str_list;
    if(edit_list_str.size() != 0)
    {
        edit_list_str_list = edit_list_str.split(SPLIT_SYMBOL);
    }

    for (const QString& str : edit_list_str_list)
    {
        add_edit_list_item(str, HISTORY_ITEM_TYPE);
    }
    edit_list_ini.endGroup();
}

void InputPanel::save_history_list_item_to_file(const QString &text)
{
    QSettings edit_list_ini(EDIT_LIST_INI, QSettings::IniFormat);
    edit_list_ini.beginGroup("EditList");
    QString key = QString("Panel_%1").arg(attr.id);
    QString edit_list_str = edit_list_ini.value(key, "").toString();

    QStringList edit_list_str_list;
    if(edit_list_str.size() != 0)
    {
        edit_list_str_list = edit_list_str.split(SPLIT_SYMBOL);
    }

    //有重复的删除再插入
    if(edit_list_str_list.contains(text))
    {
        edit_list_str_list.removeAll(text);
    }

    //超出量大数量删除最早的再插入
    if(edit_list_str_list.size() >= MAX_EDIT_LIST - fixed_items_cnt)
    {
        edit_list_str_list.removeFirst();
    }

    edit_list_str_list.append(text);
    edit_list_ini.setValue(key, edit_list_str_list.join(SPLIT_SYMBOL));
    edit_list_ini.endGroup();

    // 刷新文件内容到磁盘
    //settings.sync();
}

void InputPanel::clear_history_list_to_file()
{
    QSettings edit_list_ini(EDIT_LIST_INI, QSettings::IniFormat);
    edit_list_ini.beginGroup("EditList");
    QString key = QString("Panel_%1").arg(attr.id);
    edit_list_ini.setValue(key, "");
    edit_list_ini.endGroup();
}

void InputPanel::setFocus()
{
    input_line_edit_right->setFocus();
}

void InputPanel::clearFocus()
{
    prefix_line_edit->clearFocus();
    suffix_line_edit->clearFocus();
    input_line_edit_left->clearFocus();
    input_line_edit_right->clearFocus();
}

bool InputPanel::isFocus()
{
    //return input_line_edit_right->hasFocus();
    return input_line_edit_left->hasFocus() || input_line_edit_right->hasFocus();
}

void InputPanel::setText(const QString &text)
{
    if(attr.two_input_box_enable)
    {//双输入框
        if(text.count(connector_symbol) == 1)
        {
            QStringList text_split = text.split(connector_symbol);
            input_line_edit_left->setText(text_split.at(0));
            input_line_edit_right->setText(text_split.at(1));
        }
        else
        {
            input_line_edit_right->setText(text);
        }
    }
    else
    {//单输入框
        input_line_edit_right->setText(text);
    }

}

void InputPanel::setLeftBoxFocus()
{
    if(input_line_edit_left->isVisible())
    {
        input_line_edit_left->setFocus();
    }
    else
    {
        input_line_edit_right->setFocus();
    }
}

void InputPanel::setRightBoxFocus()
{
    input_line_edit_right->setFocus();
}

bool InputPanel::isLeftBoxFocus()
{
    return input_line_edit_left->hasFocus() && input_line_edit_left->isVisible();
}

bool InputPanel::isRightBoxFocus()
{
    return input_line_edit_right->hasFocus();
}

void InputPanel::setConnectorSymbol(const QString &connectorSymbol)
{
    connector_symbol = connectorSymbol;
    input_line_hyphen->setText(connector_symbol);
}

void InputPanel::setDelimiterSymbol(const QString &dilimiterSymbol)
{
    delimiter_symbol = dilimiterSymbol;
}

void InputPanel::incText()
{
    QLineEdit *focused_input_box;

    if(input_line_edit_right->hasFocus())
    {
        focused_input_box = input_line_edit_right;
    }
    else if(input_line_edit_left->hasFocus())
    {
        focused_input_box = input_line_edit_left;
    }
    else
    {
        return;
    }

    if(focused_input_box->text() == "")
        return;

    QString currentText = focused_input_box->text();
    QChar last_char = currentText.back();

    if(last_char.isDigit())
    {//是数字
        focused_input_box->setText(incTailNumber(currentText));
    }
    else if(isAlpha(last_char))
    {//是字母
        focused_input_box->setText(incTailAlpha(currentText));
    }
}

void InputPanel::decText()
{
    QLineEdit *focused_input_box;

    if(input_line_edit_right->hasFocus())
    {
        focused_input_box = input_line_edit_right;
    }
    else if(input_line_edit_left->hasFocus())
    {
        focused_input_box = input_line_edit_left;
    }
    else
    {
        return;
    }

    if(focused_input_box->text() == "")
        return;

    QString currentText = focused_input_box->text();
    QChar last_char = currentText.back();

    if(last_char.isDigit())
    {//是数字
        focused_input_box->setText(decTailNumber(currentText));
    }
    else if(isAlpha(last_char))
    {//是字母
        focused_input_box->setText(decTailAlpha(currentText));
    }
}

void InputPanel::setMergeSymbol(const QString &mergeSymbol)
{
    merge_symbol = mergeSymbol;
    QString display_merge_symbol = merge_symbol;
    display_merge_symbol.replace("&", "&&");
    single_or_couple_btn->setText(QString("[1 %1 2]").arg(display_merge_symbol));
}

void InputPanel::setSingleCoupleButtonVisible(bool isVisible)
{
    attr.single_or_couple_visible = isVisible;
    single_or_couple_btn->setVisible(attr.single_or_couple_visible);
    write_config("single_or_couple_visible", attr.single_or_couple_visible);
}

void InputPanel::setPresetTextVisible(bool isVisible)
{
    if(isVisible)
    {
        clear_preset_list();
        load_preset_list_from_file();
    }
    else
    {
        // 清空列表中的预设项
        clear_preset_list();
    }
}

void InputPanel::setup_property_menu()
{
    pre_suf_box_action = new QAction(tr("前缀和后缀输入框"), this);
    pre_suf_box_action->setCheckable(true);
    two_input_box_action = new QAction(tr("双输入框"), this);
    two_input_box_action->setCheckable(true);
    plus_btn_action = new QAction(tr("\"+1\"按钮"), this);
    plus_btn_action->setCheckable(true);
    plus_auto_btn_action = new QAction(tr("\"+1 Auto\"按钮"), this);
    plus_auto_btn_action->setCheckable(true);
    retain_action = new QAction(tr("创建后保留内容"), this);
    retain_action->setCheckable(true);

    ime_action = new QAction(tr("允许输入法"), this);
    ime_action->setCheckable(true);

    property_menu->clear();
    property_menu->addAction(pre_suf_box_action);
//    property_menu->addSeparator();
    property_menu->addAction(two_input_box_action);
    property_menu->addAction(plus_btn_action);
    property_menu->addAction(plus_auto_btn_action);
    property_menu->addAction(retain_action);
    property_menu->addAction(ime_action);

    connect(pre_suf_box_action, &QAction::triggered, this, [=]
    {//有无前后导输入框
        attr.pre_suf_visible = pre_suf_box_action->isChecked();
        right_top_container->setVisible(attr.pre_suf_visible);
        write_config("pre_suf_visible", attr.pre_suf_visible);
        if(!attr.pre_suf_visible)
        {
            prefix_line_edit->setText("");
            suffix_line_edit->setText("");
        }
    });

    connect(two_input_box_action, &QAction::triggered, this, [=]
    {//单双输入框
        attr.two_input_box_enable = two_input_box_action->isChecked();

        if(!attr.two_input_box_enable)
        {//单输入框
            input_line_edit_left->setVisible(false);
            input_line_edit_left->setText("");//隐藏后清空
            input_line_hyphen->setVisible(false);
            input_line_edit_right->setVisible(true);
            input_line_edit_right->setStyleSheet(SS_input_line_edit);
        }
        else
        {//双输入框
            input_line_edit_left->setVisible(true);
            input_line_hyphen->setVisible(true);
            input_line_edit_right->setVisible(true);
            input_line_edit_right->setStyleSheet(SS_input_line_edit_right);
        }

        write_config("two_input_box_enable", attr.two_input_box_enable);
    });

    connect(plus_btn_action, &QAction::triggered, this, [=]
    {//加一按钮类型（0:无按钮, 1:"+1", 2:"+1 Auto"）

        if(plus_btn_action->isChecked())
        {
            plus_auto_btn_action->setChecked(false);

            attr.plus_btn_type = 1;
            plus_btn->setVisible(true);
            auto_plus_btn->setVisible(false);
        }
        else if(!plus_btn_action->isChecked() && !plus_auto_btn_action->isChecked())
        {
            attr.plus_btn_type = 0;
            plus_btn->setVisible(false);
            auto_plus_btn->setVisible(false);
        }

        write_config("plus_btn_type", attr.plus_btn_type);
    });

    connect(plus_auto_btn_action, &QAction::triggered, this, [=]
    {//加一按钮类型（0:无按钮, 1:"+1", 2:"+1 Auto"）
        if(plus_auto_btn_action->isChecked())
        {
            plus_btn_action->setChecked(false);

            attr.plus_btn_type = 2;
            auto_plus_btn->setVisible(true);
            plus_btn->setVisible(false);
        }
        else if(!plus_btn_action->isChecked() && !plus_auto_btn_action->isChecked())
        {
            attr.plus_btn_type = 0;
            plus_btn->setVisible(false);
            auto_plus_btn->setVisible(false);
        }

        write_config("plus_btn_type", attr.plus_btn_type);
    });

    connect(retain_action, &QAction::triggered, this, [=]
    {//是否保持上次输入内容
        retain_btn->setChecked(retain_action->isChecked());
        attr.retain_enable = retain_action->isChecked();
        write_config("retain_enable", attr.retain_enable);
    });

    connect(ime_action, &QAction::triggered, this, [=]
    {//是否允许输入法
        ime_btn->setChecked(ime_action->isChecked());
        attr.ime_enable = ime_action->isChecked();
        write_config("ime_enable", attr.ime_enable);
    });
}


void InputPanel::retranslate_ui()
{
    property_btn->setText(tr("点击设置该录入框属性"));
    //    clear_action->setText(tr("<--------清空-------->"));
    clear_item->setText(tr("<--------清空-------->"));

    pre_suf_box_action->setText(tr("前缀和后缀输入框"));
    two_input_box_action->setText(tr("双输入框"));
    plus_btn_action->setText(tr("\"+1\"按钮"));
    plus_auto_btn_action->setText(tr("\"+1 Auto\"按钮"));
    retain_action->setText(tr("创建后保留内容"));
    ime_action->setText(tr("允许输入法"));

    prefix_btn->setText(tr("前缀"));
    suffix_btn->setText(tr("后缀"));
}

void InputPanel::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange)
    {
        qDebug() << "InputPanel LanguageChangeEvent";
        retranslate_ui();
    }
}
