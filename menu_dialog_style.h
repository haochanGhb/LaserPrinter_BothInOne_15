#ifndef TEMPLATE_DIALOG_STYLE_H
#define TEMPLATE_DIALOG_STYLE_H

#include<QString>

const QString SS_Panels= R"(
                         QWidget{
                         background-color: #ffffff;
                         border: none;
                         border-radius: 3px;
                         }

                         QLineEdit{
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
)";

const QString SS_ItemPanels= R"(
    QWidget{
       background-color: #ffffff;
       border: none;
       border-radius: 3px;
    }
    QLabel{
       border: none;
       background: transparent;
       font-size: 18px;
       font-family: '微软雅黑';
       text-align: left;
       padding: 0px;
       color: #2a2d32;
    }
    QPushButton {
        image: url(:/image/switch_button_off.png);
    }
    QPushButton:checked{
        image: url(:/image/switch_button_on.png);
    }
    QLineEdit{
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
    QRadioButton{
       font-size: 18px;
       font-family: '微软雅黑';
       text-align: left;
       color: #2a2d32;
    }
    QRadioButton:checked{
       color: #63d0f7;

    }
    QRadioButton:disabled{
       color: #2a2d32;
    }
    QRadioButton::indicator{
       width: 30px;
       height: 30px;
    }
    QRadioButton::indicator:checked {
         background-color: #63d0f7;
         border: 1px #2a2d32;
         border-radius: 14px;
    }
)";

const QString SS_MaintenanceItemPanels= R"(
    QWidget{
       background-color: #ffffff;
       border: none;
       border-radius: 3px;
    }
    QLabel{
       border: none;
       background: transparent;
       font-size: 18px;
       font-family: '微软雅黑';
       text-align: left;
       padding: 0px;
       color: #2a2d32;
    }
    QLineEdit{
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
    QRadioButton{
       font-size: 18px;
       font-family: '微软雅黑';
       text-align: left;
       color: #2a2d32;
    }
    QRadioButton:checked{
       color: #63d0f7;
    }
    QRadioButton:disabled{
       color: #2a2d32;
    }
    QRadioButton::indicator{
       width: 30px;
       height: 30px;
    }
)";

const QString SS_SeparateLine= R"(
    QFrame{
       background-color: #e0e0e0;
    }
)";

const QString SS_Title_Label= R"(
    QLabel{
       border: none;
       background: transparent;
       font-size: 20px;
       font-family: '微软雅黑';
       font-weight: bold;
       text-align: left;
       padding: 0px;
       color: #2a2d32;
    }
)";

const QString SS_Logo_Label= R"(
    QLabel{
       border: 1px solid #bdbdbd;
       background: transparent;
       font-size: 20px;
       font-family: '微软雅黑';
       font-weight: bold;
       text-align: left;
       padding: 0px;
       color: #2a2d32;
    }
)";


const QString SS_template_seletor = R"(
    QComboBox {
        background-color: #ffffff;
        border: 1px solid #bdbdbd;
        border-radius: 2px;
        font-size: 20px;
        font-family:'微软雅黑';
        color: #4d4d4d;
        padding: 5px;
    }
    QComboBox::drop-down {
        width: 18px;
        border:none;
    }
    QComboBox::down-arrow {
        image: url(:/image/down_arrow_.png);
        width:12px;
        height:6px;
    }
    QComboBox QAbstractItemView {
        border: 2px solid #63d0f7;
        font-size: 20px;
        font-family:'微软雅黑';
        color: #4d4d4d;
        border-radius: 2px;
        outline: 0px;
    }
    QComboBox QAbstractItemView::item{
        padding: 5px;
    }
    QComboBox QAbstractItemView::item:hover{
        border-radius: 2px;
        background-color:#f0f0f0;
        color:#4d4d4d;
    }
    QComboBox QAbstractItemView::item:selected{
        border-radius: 2px;
        background-color:#f0f0f0;
        color:#4d4d4d;
    }
    QComboBox QAbstractScrollArea QScrollBar:vertical {
        width: 12px;
        background-color: #ffffff;
    }
    QComboBox QAbstractScrollArea QScrollBar::handle:vertical {
        border-radius: 3px;
        background: #acacac;
    }
)";


const QString SS_SubLabel= R"(
    QLabel{
       font-size: 16px;
       font-family: '微软雅黑';
       text-align: left;
       padding-left: 20px;
       color: #2a2d32;
    }
)";


const QString SS_Primary_Menu_Btn= R"(
    QPushButton{
       border:none;
       background-color:#63d0f7;
       border-radius: 2px;
       font-size:20px;
       font-family:'微软雅黑';
       color:#ffffff;
    }
    QPushButton:hover{
       background-color:#79d6f9;
    }
    QPushButton:pressed{
       background-color:#63d0f7;
    }
    QPushButton:disabled{
       background-color:#a6a6a6;
    }
)";

const QString SS_Normal_Menu_Btn= R"(
    QPushButton{
       border:1px solid #bcbcbc;
       background-color:#ffffff;
       border-radius: 2px;
       font-size:20px;
       font-family:'微软雅黑';
       color:#4d4d4d;
    }
    QPushButton:hover{
       background-color:#e5f2ff;
    }
    QPushButton:pressed{
       background-color:#ffffff;
    }
    QPushButton:disabled{
       background-color:#f5f5f7;
       color:#bcbcbc;
    }
)";

const QString SS_Normal_Menu_Label= R"(
    QLabel{
       border: none;
       background: transparent;
       font-size: 18px;
       font-family: '微软雅黑';
       text-align: center;
       padding: 10px;
       color: #2a2d32;
    }
)";

const QString SS_CheckBox= R"(
    QCheckBox{
       border: none;
       background: transparent;
       font-size: 18px;
       font-family: '微软雅黑';
       padding: 10px;
       color: #2a2d32;
    }
    QCheckBox::indicator{
        width: 20px;
        height: 20px;
    }
    QCheckBox::indicator:unchecked{
       border: 1px solid #bcbcbc;
       border-radius: 3px;
       background-color:#ffffff;
    }
    QCheckBox::indicator:checked{
       border: 1px solid #ffffff;
       border-radius: 3px;
       background-color:#63d0f7;
    }
)";

const QString SS_code_width_slider = R"(
    QSlider {
        background-color: #ffffff;
    }
    QSlider::groove:horizontal {
        background-color: #575757;
        border: 0px solid #0061b0;
        height: 6px;
        border-radius: 3px;
    }
    QSlider::handle:horizontal {
        background-color: #63d0f7;
        border: 1px solid #9e9e9e;
        width: 35px;
        height: 12px;
        border-radius: 2px;
        margin: -4px 0px;
    }
    QSlider::sub-page {
        background-color: #575757;
        border: 0px solid #0061b0;
        height: 6px;
        border-radius: 3px;
    }
    QSlider::add-page {
        background-color: #575757;
        border: 0px solid #0061b0;
        height: 6px;
        border-radius: 3px;
    }
)";

const QString SS_SpinBox = R"(
    QSpinBox {
        background-color: #ffffff;
        border: 1px solid #bdbdbd;
        border-radius: 2px;
        font-size: 20px;
        font-family:'微软雅黑';
        qproperty-alignment: 'AlignCenter';
        color: #4d4d4d;
        padding: 5px;
    }
    QSpinBox:disabled{
        background-color: #f5f5f7;
    }
    QSpinBox:focus {
        border: 2px solid #63d0f7;
    }
    QSpinBox::up-button, QSpinBox::down-button{
        background: transparent;
        border: none;
        width: 30px;
    }
    QSpinBox::up-arrow{
        image: url(:/image/up_arrow.png);
        width: 12px;
        height: 6px;
    }
    QSpinBox::down-arrow {
        image: url(:/image/down_arrow_.png);
        width: 12px;
        height: 6px;
    }
)";

const QString SS_Tab_Btn= R"(
    QPushButton{
       border: 2px solid #d5d5d5;
       border-radius: 2px;
       background-color: #d5d5d5;
       font-size: 20px;
       font-family: '微软雅黑';
       text-align: center;
       color: #2a2d32;
    }
    QPushButton:checked{
        border: 2px solid #63d0f7;
        background-color: #e5f2ff;
        font-weight: bold;
        color: #63d0f7;
    }
)";

const QString SS_name_btn = R"(
    QPushButton{
       border: 1px solid #bdbdbd;
       border-radius: 2px;
       font-size: 20px;
       font-family: '微软雅黑';
       text-align: center;
       color: #2a2d32;
    }
    QPushButton:checked{
       color: #63d0f7;
    }
)";

const QString SS_Input_Box = R"(
    QLineEdit{
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
)";

/*
const QString SS_ComboBox = R"(
    QComboBox {
        background-color: #ffffff;
        border: 1px solid #bdbdbd;
        border-radius: 2px;
        font-size: 20px;
        font-family:'微软雅黑';
        color: #4d4d4d;
        padding-left: 10px;
    }
    QComboBox:disabled{
        background-color: #f5f5f7;
    }
    QComboBox::drop-down {
        width: 18px;
        border:none;
    }
    QComboBox::down-arrow {
        image: url(:/image/down_arrow_.png);
        width:12px;
        height:6px;
    }
    QComboBox QAbstractItemView {
        border: 2px solid #0061b0;
        border-radius: 2px;
        font-size: 20px;
        font-family:'微软雅黑';
        color: #4d4d4d;
        outline: 0px;
        padding: 1px;
    }
    QComboBox QAbstractItemView::item{
        padding: 5px;
    }
    QComboBox QAbstractItemView::item:hover{
        border-radius: 2px;
        background-color:#f0f0f0;
        color: #4d4d4d;
    }
    QComboBox QAbstractItemView::item:selected{
        border-radius: 2px;
        background-color:#f0f0f0;
        color: #4d4d4d;
    }
    QComboBox QAbstractScrollArea QScrollBar:vertical {
        width: 12px;
        background-color: #ffffff;
    }
    QComboBox QAbstractScrollArea QScrollBar::handle:vertical {
        border-radius: 3px;
        background: #acacac;
    }
)";
*/


const QString SS_ComboBox = R"(
    QComboBox {
        background-color: #ffffff;
        border: 1px solid #bdbdbd;
        border-radius: 2px;
        font-size: 20px;
        font-family:'微软雅黑';
        color: #4d4d4d;
        padding: 5px;
    }
    QComboBox::drop-down {
        width: 18px;
        border:none;
    }
    QComboBox::down-arrow {
        image: url(:/image/down_arrow_.png);
        width:12px;
        height:6px;
    }
    QComboBox QAbstractItemView {
        border: 2px solid #63d0f7;
        font-size: 20px;
        font-family:'微软雅黑';
        color: #4d4d4d;
        border-radius: 2px;
        outline: 0px;
    }
    QComboBox QAbstractItemView::item{
        padding: 5px;
    }
    QComboBox QAbstractItemView::item:hover{
        border-radius: 2px;
        background-color:#f0f0f0;
        color:#4d4d4d;
    }
    QComboBox QAbstractItemView::item:selected{
        border-radius: 2px;
        background-color:#f0f0f0;
        color:#4d4d4d;
    }
    QComboBox QAbstractScrollArea QScrollBar:vertical {
        width: 12px;
        background-color: #ffffff;
    }
    QComboBox QAbstractScrollArea QScrollBar::handle:vertical {
        border-radius: 3px;
        background: #acacac;
    }
)";

const QString SS_SpinBoxWithLittleArrow = R"(
    QSpinBox {
        background-color: #ffffff;
        border: 1px solid #bdbdbd;
        border-radius: 2px;
        font-size: 20px;
        font-family:'微软雅黑';
        qproperty-alignment: 'AlignCenter';
        color: #4d4d4d;
        padding: 5px;
    }
    QSpinBox:disabled{
        background-color: #f5f5f7;
    }
    QSpinBox:focus {
        border: 2px solid #63d0f7;
    }
    QSpinBox::up-button, QSpinBox::down-button{
        background: transparent;
        border: none;
        width: 16px;
    }
    QSpinBox::up-arrow{
        image: url(:/image/up_arrow.png);
        width: 12px;
        height: 6px;
    }
    QSpinBox::down-arrow {
        image: url(:/image/down_arrow_.png);
        width: 12px;
        height: 6px;
    }
)";

const QString SS_Color_Btn= R"(
    QPushButton{
        background-color:#ffffff;
        border:1px solid #bcbcbc;
        border-radius: 0px;
        font-size:18px;
        font-family:'微软雅黑';
    }
)";

const QString SS_SwitchButton= R"(
    QPushButton {
        image: url(:/image/switch_button_off.png);
    }
    QPushButton:checked{
        image: url(:/image/switch_button_on.png);
    }
)";

const QString SS_ListViewButton= R"(
    QPushButton {
       image: url(:/image/no_view_list_button.png);
    }
)";


const QString SS_RadioButton= R"(
    QRadioButton{
       font-size: 20px;
       font-family: '微软雅黑';
       text-align: left;
       color: #2a2d32;
    }
    QRadioButton:checked{
       color: #63d0f7;
    }
    QRadioButton::indicator{
       width: 30px;
       height: 30px;
    }
)";

const QString SS_List= R"(
    QListWidget{
       border:1px solid #bcbcbc;
       background-color:#ffffff;
       border-radius: 2px;
       font-size:20px;
       font-family:'微软雅黑';
       color:#2a2d32;
       outline: 0px;
    }
    QListWidget::Item {
        border-bottom: 1px dashed #bcbcbc;
        padding:5px;
        margin:0px;
    }
    QListWidget::Item:selected {
        background-color:#e5f2ff;
        border-radius: 2px;
        border: 2px solid #63d0f7;
        color:#2a2d32;
    }
    QListWidget::Item:selected:disabled {
        background-color:#a6a6a6;
        border-radius: 2px;
        border: 2px solid #a6a6a6;
        color:#ffffff;
    }
    QListWidget:disabled {
        background-color:#f5f5f7;
    }
    QListWidget QAbstractScrollArea QScrollBar:vertical {
        width: 12px;
        background-color: #ffffff;
    }
    QListWidget QAbstractScrollArea QScrollBar::handle:vertical {
        border-radius: 3px;
        background: #acacac;
    }
)";


const QString SS_barcode_checkbox= R"(
QCheckBox {
    border: none;          /* 灰色边框 */
    border-radius: 2px;              /* 圆角 */
    padding: 5px;                    /* 内边距 */
    height: 25px;
    font-size:20px;
    font-family:'微软雅黑';
}

QCheckBox::indicator {
    width: 30px;                     /* 指示器宽度 */
    height: 30px;                    /* 指示器高度 */
}

QCheckBox::indicator:unchecked {
    background: url(":/image/unchecked.png");  /* 未选中时的背景图片 */
}

QCheckBox::indicator:checked {
    background: url(":/image/checked.png");    /* 选中时的背景图片 */
}

QCheckBox:unchecked {
    color: #4d4d4d;                    /* 未选中状态的字体颜色 */
}

QCheckBox:checked {
    color: #0061b0;                  /* 选中状态的字体颜色 */
}
)";

#endif // TEMPLATE_DIALOG_STYLE_H
