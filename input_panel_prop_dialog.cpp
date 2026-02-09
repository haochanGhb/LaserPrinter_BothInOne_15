#include "input_panel_prop_dialog.h"
#include "menu_dialog_style.h"

InputPanelPropDialog::InputPanelPropDialog(InputPanelAttr *attr, BaseDialog *parent) : BaseDialog(parent)
{
    this->attr = attr;

    create_widget();
    setup_layout();
    set_stylesheet();

    set_options(attr);

    connect(save_btn, &QPushButton::clicked, this, &InputPanelPropDialog::on_save_btn_clicked);
    connect(cancel_btn, &QPushButton::clicked, this, &InputPanelPropDialog::on_cancel_btn_clicked);

}

void InputPanelPropDialog::create_widget()
{
    input_box_options = new QWidget();
    input_box_options->setFixedWidth(720);

    pre_suf_options = new QWidget();
    pre_suf_options->setFixedWidth(720);

    plus_btn_options = new QWidget();
    plus_btn_options->setFixedWidth(720);

    retian_btn_options = new QWidget();
    retian_btn_options->setFixedWidth(720);

    ime_btn_options = new QWidget();
    ime_btn_options->setFixedWidth(720);

    one_input_box = new QRadioButton();
    one_input_box->setText(tr("提供1个录入框"));
    one_input_box_desc = new QLabel();
    one_input_box_desc->setText(tr("提供1个录入框，“单号任务”和“多号任务”共用一个录入框。"));
    two_input_boxes = new QRadioButton();
    two_input_boxes->setText(tr("提供2个录入框"));
    two_input_boxes_desc = new QLabel();
    two_input_boxes_desc->setText(tr("提供2个录入框，用户无需输入“-”。"));

    pre_suf_enable = new QRadioButton();
    pre_suf_enable->setText(tr("提供“前导录入框”和“后导录入框”"));
    pre_suf_enable_desc = new QLabel();
    pre_suf_enable_desc->setText(tr("提供“前导录入框”和“后导录入框”来辅助“主录入框”。"));
    pre_suf_disable = new QRadioButton();
    pre_suf_disable->setText(tr("只提供“主录入框”"));
    pre_suf_disable_desc = new QLabel();
    pre_suf_disable_desc->setText(tr("只提供“主录入框”（以节省空间）。"));

    no_plus_btn = new QRadioButton();
    no_plus_btn->setText(tr("不提供“+1”按钮"));
    no_plus_btn_desc = new QLabel();
    no_plus_btn_desc->setText(tr("不提供“+1”按钮"));
    plus_btn = new QRadioButton();
    plus_btn->setText(tr("提供“+1”按钮"));
    plus_btn_desc = new QLabel();
    plus_btn_desc->setText(tr("每次点击“+1”按钮，会把左侧录入框的数字/字母增加1。"));
    plus_auto_btn = new QRadioButton();
    plus_auto_btn->setText(tr("提供“+1 Auto”按钮"));
    plus_auto_btn_desc = new QLabel();
    plus_auto_btn_desc->setText(tr("每次创建新任务后，会自动把左侧录入框的数字/字母增加1。"));

    retian_enable = new QRadioButton();
    retian_enable->setText(tr("保留录入框内的信息"));
    retian_enable_desc = new QLabel();
    retian_enable_desc->setText(tr("创建任务后，保留录入框内的信息。"));
    retian_disable = new QRadioButton();
    retian_disable->setText(tr("不保留录入框内的信息"));
    retian_disable_desc = new QLabel();
    retian_disable_desc->setText(tr("创建任务后，自动清空录入框内的信息。"));

    ime_disable = new QRadioButton();
    ime_disable->setText(tr("屏蔽输入法"));
    ime_disable_desc = new QLabel();
    ime_disable_desc->setText(tr("若该录入框只需录入数字/字母/符号，无需拼写文字，请选择屏蔽输入法。"));
    ime_enable = new QRadioButton();
    ime_enable->setText(tr("不屏蔽输入法"));
    ime_enable_desc = new QLabel();
    ime_enable_desc->setText(tr("若该录入框需要拼写文字，请选择不屏蔽输入法。"));

    save_btn = new QPushButton();
    save_btn->setFixedSize(120,40);
    save_btn->setText(tr("保存"));
    cancel_btn = new QPushButton();
    cancel_btn->setFixedSize(120,40);
    cancel_btn->setText(tr("取消"));
}

void InputPanelPropDialog::setup_layout()
{
    main_layout->addWidget(input_box_options, 0, Qt::AlignHCenter);
    main_layout->addWidget(pre_suf_options, 0, Qt::AlignHCenter);
    main_layout->addWidget(plus_btn_options, 0, Qt::AlignHCenter);
    main_layout->addWidget(retian_btn_options, 0, Qt::AlignHCenter);
    main_layout->addWidget(ime_btn_options, 0, Qt::AlignHCenter);

    bottom_bar_layout->addStretch();
    bottom_bar_layout->addWidget(save_btn, 0, Qt::AlignVCenter);
    bottom_bar_layout->addWidget(cancel_btn, 0, Qt::AlignVCenter);

    QVBoxLayout *input_box_options_layout = new QVBoxLayout(input_box_options);
    input_box_options_layout->setContentsMargins(10,10,10,10);
    input_box_options_layout->setSpacing(8);
    input_box_options_layout->addWidget(one_input_box);
    input_box_options_layout->addWidget(one_input_box_desc);
    input_box_options_layout->addWidget(two_input_boxes);
    input_box_options_layout->addWidget(two_input_boxes_desc);

    QVBoxLayout *pre_suf_options_layout = new QVBoxLayout(pre_suf_options);
    pre_suf_options_layout->setContentsMargins(10,10,10,10);
    pre_suf_options_layout->setSpacing(8);
    pre_suf_options_layout->addWidget(pre_suf_enable);
    pre_suf_options_layout->addWidget(pre_suf_enable_desc);
    pre_suf_options_layout->addWidget(pre_suf_disable);
    pre_suf_options_layout->addWidget(pre_suf_disable_desc);

    QVBoxLayout *plus_btn_options_layout = new QVBoxLayout(plus_btn_options);
    plus_btn_options_layout->setContentsMargins(10,10,10,10);
    plus_btn_options_layout->setSpacing(8);
    plus_btn_options_layout->addWidget(no_plus_btn);
    plus_btn_options_layout->addWidget(no_plus_btn_desc);
    plus_btn_options_layout->addWidget(plus_btn);
    plus_btn_options_layout->addWidget(plus_btn_desc);
    plus_btn_options_layout->addWidget(plus_auto_btn);
    plus_btn_options_layout->addWidget(plus_auto_btn_desc);

    QVBoxLayout *retian_btn_options_layout = new QVBoxLayout(retian_btn_options);
    retian_btn_options_layout->setContentsMargins(10,10,10,10);
    retian_btn_options_layout->setSpacing(8);
    retian_btn_options_layout->addWidget(retian_enable);
    retian_btn_options_layout->addWidget(retian_enable_desc);
    retian_btn_options_layout->addWidget(retian_disable);
    retian_btn_options_layout->addWidget(retian_disable_desc);

    QVBoxLayout *ime_btn_options_layout = new QVBoxLayout(ime_btn_options);
    ime_btn_options_layout->setContentsMargins(10,10,10,10);
    ime_btn_options_layout->setSpacing(8);
    ime_btn_options_layout->addWidget(ime_disable);
    ime_btn_options_layout->addWidget(ime_disable_desc);
    ime_btn_options_layout->addWidget(ime_enable);
    ime_btn_options_layout->addWidget(ime_enable_desc);
}

void InputPanelPropDialog::set_stylesheet()
{
    input_box_options->setStyleSheet(SS_ItemPanels);
    pre_suf_options->setStyleSheet(SS_ItemPanels);
    plus_btn_options->setStyleSheet(SS_ItemPanels);
    retian_btn_options->setStyleSheet(SS_ItemPanels);
    ime_btn_options->setStyleSheet(SS_ItemPanels);

    one_input_box_desc->setStyleSheet(SS_SubLabel);
    two_input_boxes_desc->setStyleSheet(SS_SubLabel);

    pre_suf_enable_desc->setStyleSheet(SS_SubLabel);
    pre_suf_disable_desc->setStyleSheet(SS_SubLabel);

    no_plus_btn_desc->setStyleSheet(SS_SubLabel);
    plus_btn_desc->setStyleSheet(SS_SubLabel);
    plus_auto_btn_desc->setStyleSheet(SS_SubLabel);

    retian_enable_desc->setStyleSheet(SS_SubLabel);
    retian_disable_desc->setStyleSheet(SS_SubLabel);

    ime_disable_desc->setStyleSheet(SS_SubLabel);
    ime_enable_desc->setStyleSheet(SS_SubLabel);

    save_btn->setStyleSheet(SS_Primary_Menu_Btn);
    cancel_btn->setStyleSheet(SS_Normal_Menu_Btn);
}

void InputPanelPropDialog::set_options(InputPanelAttr *attr)
{

    setTitle(attr->name);

    if(attr->two_input_box_enable)
    {
        two_input_boxes->setChecked(true);
    }
    else
    {
        one_input_box->setChecked(true);
    }

    if(attr->pre_suf_visible)
    {
        pre_suf_enable->setChecked(true);
    }
    else
    {
        pre_suf_disable->setChecked(true);
    }

    if(attr->plus_btn_type == 1)
    {
        plus_btn->setChecked(true);
    }
    else if(attr->plus_btn_type == 2)
    {
        plus_auto_btn->setChecked(true);
    }
    else
    {
        no_plus_btn->setChecked(true);
    }

    if(attr->retain_enable)
    {
        retian_enable->setChecked(true);
    }
    else
    {
        retian_disable->setChecked(true);
    }

    if(attr->ime_enable)
    {
        ime_enable->setChecked(true);
    }
    else
    {
        ime_disable->setChecked(true);
    }
}

void InputPanelPropDialog::on_save_btn_clicked()
{
    attr->two_input_box_enable = two_input_boxes->isChecked();
    attr->pre_suf_visible = pre_suf_enable->isChecked();

    if(plus_btn->isChecked())
    {
        attr->plus_btn_type = 1;
    }
    else if(plus_auto_btn->isChecked())
    {
        attr->plus_btn_type = 2;
    }
    else
    {
        attr->plus_btn_type = 0;
    }

    attr->retain_enable = retian_enable->isChecked();
    attr->ime_enable = ime_enable->isChecked();

    this->accept();
}



void InputPanelPropDialog::on_cancel_btn_clicked()
{
    this->reject();
}
