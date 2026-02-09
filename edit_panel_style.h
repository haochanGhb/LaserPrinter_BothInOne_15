#ifndef EDIT_PANEL_STYLE_H
#define EDIT_PANEL_STYLE_H

#include <QString>

const QString SS_EditPanel= R"(
    QWidget{
       background-color: #ffffff;
       border: none;
       border-radius: 3px;
    }
    QLabel{
       background: transparent;
       font-size: 18px;
       font-family: '微软雅黑';
       text-align: center;
       color: #bdbdbd
    }
)";

const QString SS_edit_area= R"(
    QWidget{
       background-color: #ffffff;
       border: none;
       border-radius: 3px;
    }
)";

const QString SS_template_seletor_main = R"(
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

const QString SS_create_btn= R"(
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
        background-color:#50c8f4;
    }

    QPushButton:disabled{
         background-color:#cccccc;
         color:#888888;
    }
)";

const QString SS_Order_Btn = R"(
    QPushButton{
       border:1px solid #bdbdbd;
       background-color:#e8e8ea;
       border-top-left-radius: 2px;
       border-bottom-left-radius: 2px;
       border-top-right-radius: 0px;
       border-bottom-right-radius: 0px;
       font-size:20px;
       font-family:'微软雅黑';
       color: #585858;
    }
    QPushButton::checked{
       border:1px solid #63d0f7;
       background-color:#e5f2ff;
       border-top-left-radius: 2px;
       border-bottom-left-radius: 2px;
       border-top-right-radius: 0px;
       border-bottom-right-radius: 0px;
       font-family:'微软雅黑';
       font-size:20px;
       color: #63d0f7;
    }
)";


const QString SS_create_menu_btn = R"(
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
        background-color:#50c8f4;
    }
    QPushButton::menu-indicator {
        subcontrol-origin: padding;
        subcontrol-position: center center;
        width: 25px;
        height: 25px;
    }
)";

const QString SS_create_menu = R"(
    QMenu {
        background-color: #ffffff;
        border: 2px solid #63d0f7;
        border-radius: 2px;
        padding: 2px 2px;
    }
    QMenu::item {
        border: none;
        border-radius: 2px;
        text-align: center;
        font-size: 20px;
        font-family: '微软雅黑';
        color: #4d4d4d;
        padding: 10px 30px 10px 10px;
        margin: 0px;
    }
    QMenu::item:selected {
        background-color: #f0f0f0;
    }
)";

const QString SS_preview_part= R"(
    QWidget{
       background-color: #ffffff;
       border-top-left-radius: 3px;
       border-bottom-left-radius: 3px;
       border-top-right-radius: 0px;
       border-bottom-right-radius: 0px;
    }
)";

//#c4ddf1



const QString SS_print_preview_panel= R"(
    QWidget{
       background-color: #f2f9ff;
       border: 2px solid #63d0f7;
       border-radius: 0px;
    }
)";

const QString SS_separation_line= R"(
    QFrame{
       background-color: #ffffff;
    }
)";

const QString SS_keyboard_btn= R"(
    QPushButton{
        border:none;
        background-color:#c6dcf0;
        border-radius: 2px;
        font-size:16px;
        font-family:'微软雅黑';
        color:#63d0f7;
        image: url(:/image/keyboard.png);
        opacity: 0.5;
    }
    QPushButton:hover{
        border:1px solid #bcbcbc;
        background-color:#e5f2ff;
    }
    QPushButton:checked{
        border: 1px solid #63d0f7;
        background-color:#e5f2ff;
        border-radius: 2px;
        font-size:16px;
        font-family:'微软雅黑';
        color:#63d0f7;
    }
)";

const QString SS_param_btn= R"(
    QPushButton{
        border:none;
        background-color:#c6dcf0;
        border-radius: 2px;
        font-size:16px;
        font-family:'微软雅黑';
        color:#63d0f7;
        image: url(:/image/param.png);
        opacity: 0.5;
    }
    QPushButton:hover{
        border:1px solid #bcbcbc;
        background-color:#e5f2ff;
    }
    QPushButton:checked{
        border: 1px solid #63d0f7;
        background-color:#e5f2ff;
        border-radius: 2px;
        font-size:16px;
        font-family:'微软雅黑';
        color:#63d0f7;
    }
)";

const QString SS_input_part= R"(
    QWidget{
       background-color: #ffffff;
       border: none;
       border-radius: 3px;
    }
)";

const QString SS_slot_title = R"(
    QLabel{
       background: transparent;
       font-size: 18px;
       font-family: '微软雅黑';
       text-align: center;
       color: #2a2d32
    }
)";

const QString SS_slot_btn = R"(
    QPushButton{
        border:1px solid #bcbcbc;
        background-color:#ffffff;
        border-radius: 2px;
        font-size:20px;
        font-family:'微软雅黑';
        color:#4d4d4d;
    }
    QPushButton:hover{
        border:1px solid #79d6f9;
    }
    QPushButton:checked{
        background-color:#63d0f7;
        color:#ffffff;
    }
)";

const QString SS_slot_attr_btn = R"(
    QPushButton{
        border:1px solid #bcbcbc;
        background-color:#ffffff;
        border-radius: 2px;
    }
    QPushButton:pressed{
        border:2px solid #63d0f7;
    }
    QPushButton::menu-indicator {
        image:none;
    }
)";

const QString SS_slot_attr_menu = R"(
    QMenu {
        background-color: #ffffff;
        border: 2px solid #63d0f7;
        border-radius: 2px;
        padding: 0px 0px;
    }
    QMenu::item {
        border: none;
        border-radius: 2px;
        text-align: center;
        font-size: 20px;
        font-family: '微软雅黑';
        color: #4d4d4d;
        padding: 10px 50px 10px 20px;
        margin: 0px;
    }
    QMenu::icon {
        padding: 10px;
    }
    QMenu::item:selected {
        border-radius: 2px;
        background-color: #f0f0f0;
    }
)";

const QString SS_input_scroll_area = R"(
    QScrollArea {
        background-color: #ffffff;
        border: 0px solid #0061b0;
        border-radius: 2px;
    }
    QWidget#input_mid_container {
        background-color: #ffffff;
    }
    /* 垂直滚动条 */
    QScrollBar:vertical {
        width: 13px;
        background: transparent;
        margin: 0px;
    }
    QScrollBar::handle:vertical {
        background: #c0c0c0;
        border-radius: 3px;
        min-height: 20px;
    }
    QScrollBar::handle:vertical:hover {
        background: #a0a0a0;
    }
    QScrollBar::add-line:vertical,
    QScrollBar::sub-line:vertical {
        height: 0px;
        background: none;
    }
    QScrollBar::up-arrow:vertical,
    QScrollBar::down-arrow:vertical {
        width: 0px;
        height: 0px;
    }

    /* 水平滚动条（隐藏） */
    QScrollBar:horizontal {
        height: 0px;
    }
)";



#endif // EDIT_PANEL_STYLE_H
