#ifndef INPUT_PANEL_STYLE_H
#define INPUT_PANEL_STYLE_H

#include <QString>

/*************************************************
 *
 * InputPanel StyleSheet
 *
*************************************************/

const QString SS_InputPanel = R"(
    QWidget{
       background-color: rgb(248, 248, 248);
       border: 2px dashed #d0d0d0;
       border-radius: 2px;
    }
)";

//background-color: rgb(229, 242, 255);
const QString SS_left_part = R"(
    QWidget{
       background-color: rgb(248, 248, 248);
       border: none;
       border-radius: 2px;
    }
)";

const QString SS_right_part = R"(
    QWidget{
       background-color: rgb(248, 248, 248);
       border: none;
       border-radius: 2px;
    }
)";

const QString SS_background = R"(
    QLabel{
       background-color: #ffffff;
       border: 2px dashed #d0d0d0;
       border-radius: 2px;
    }
)";

const QString SS_background_left = R"(
    QLabel{
       background-color: rgb(248, 248, 248);
       border-radius: 2px;
       font-size: 20px;
       font-family: '微软雅黑';
       text-align: center;
       color: #2a2d32
    }
)";

const QString SS_single_or_couple_btn = R"(
    QPushButton{
       background: transparent;
       border: none;
       padding:0px;
       font-size: 14px;
       font-family: '微软雅黑';
       color: #d0d0d0
    }
    QPushButton::checked{
       background: transparent;
       border: none;
       font-size: 14px;
       font-family: '微软雅黑';
       color: #63d0f7;
    }
)";

const QString SS_prefix_line_edit_disable = R"(
    QLineEdit{
       border: 1px solid #bdbdbd;
       border-left: none;
       background-color:#f5f5f7;
       border-top-left-radius: 0px;
       border-bottom-left-radius: 0px;
       border-top-right-radius: 2px;
       border-bottom-right-radius: 2px;
       font-size: 18px;
       font-family: '微软雅黑';
       text-align: center;
    }
    QLineEdit:focus{
       border: 2px solid #63d0f7;
    }
)";

const QString SS_prefix_line_edit_enable = R"(
    QLineEdit{
       border: 1px solid #bdbdbd;
       border-left: none;
       background-color:#f8f8f8;
       border-top-left-radius: 0px;
       border-bottom-left-radius: 0px;
       border-top-right-radius: 2px;
       border-bottom-right-radius: 2px;
       font-size: 18px;
       font-family: '微软雅黑';
       text-align: center;
    }
    QLineEdit:focus{
       border: 2px solid #63d0f7;
    }
)";

const QString SS_suffix_line_edit_disable = R"(
    QLineEdit{
       border: 1px solid #bdbdbd;
       border-right: none;
       background-color:#f5f5f7;
       border-top-left-radius: 2px;
       border-bottom-left-radius: 2px;
       border-top-right-radius: 0px;
       border-bottom-right-radius: 0px;
       font-size: 18px;
       font-family: '微软雅黑';
       text-align: center;
    }
    QLineEdit:focus{
       border: 2px solid #63d0f7;
    }
)";

const QString SS_suffix_line_edit_enable = R"(
    QLineEdit{
       border: 1px solid #bdbdbd;
       border-right: none;
       background-color:#f8f8f8;
       border-top-left-radius: 2px;
       border-bottom-left-radius: 2px;
       border-top-right-radius: 0px;
       border-bottom-right-radius: 0px;
       font-size: 18px;
       font-family: '微软雅黑';
       text-align: center;
    }
    QLineEdit:focus{
       border: 2px solid #63d0f7;
    }
)";

const QString SS_prefix_btn = R"(
    QPushButton{
       border:1px solid #bdbdbd;
       background-color:#e8e8ea;
       border-top-left-radius: 2px;
       border-bottom-left-radius: 2px;
       border-top-right-radius: 0px;
       border-bottom-right-radius: 0px;
       font-size:12px;
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
       font-size:12px;
       color: #63d0f7;
    }
)";

const QString SS_suffix_btn = R"(
    QPushButton{
       border:1px solid #bdbdbd;
       background-color:#e8e8ea;
       border-top-left-radius: 0px;
       border-bottom-left-radius: 0px;
       border-top-right-radius: 2px;
       border-bottom-right-radius: 2px;
       font-size:12px;
       font-family:'微软雅黑';
       color: #585858;
    }
    QPushButton::checked{
       border:1px solid #63d0f7;
       background-color:#e5f2ff;
       border-top-left-radius: 0px;
       border-bottom-left-radius: 0px;
       border-top-right-radius: 2px;
       border-bottom-right-radius: 2px;
       font-family:'微软雅黑';
       font-size:12px;
       color: #63d0f7;
    }
)";

const QString SS_ime_btn = R"(
    QPushButton{
       background: transparent;
       border: none;
       padding:0px;
       font-size: 10px;
       font-weight: bold;
       font-family: '微软雅黑';
       color: #d0d0d0
    }
    QPushButton::checked{
       background: transparent;
       border: none;
       font-size: 10px;
       font-weight: bold;
       font-family: '微软雅黑';
       color: #63d0f7;
    }
)";

const QString SS_panel_name = R"(
    QPushButton{
       background: transparent;
       font-size: 18px;
       font-family: '微软雅黑';
       text-align: center;
       color: #2a2d32
    }
)";

const QString SS_retain_btn = R"(
    QPushButton{
       background: transparent;
       border: none;
       padding:0px;
       font-size: 22px;
       font-weight: bold;
       font-family: '微软雅黑';
       color: #d0d0d0
    }
    QPushButton::checked{
       background: transparent;
       border: none;
       font-size: 22px;
       font-weight: bold;
       font-family: '微软雅黑';
       color: #63d0f7;
    }
)";

const QString SS_input_line_edit = R"(
    QLineEdit{
       border: 1px solid #bdbdbd;
       border-top-left-radius: 2px;
       border-bottom-left-radius: 2px;
       border-top-right-radius: 0px;
       border-bottom-right-radius: 0px;
       font-size: 20px;
       font-family: '微软雅黑';
       text-align: center;
    }
    QLineEdit:focus{
       border: 2px solid #63d0f7;
    }
)";

const QString SS_input_line_edit_left = R"(
    QLineEdit{
       border: 1px solid #bdbdbd;
       border-right: none;
       border-top-left-radius: 2px;
       border-bottom-left-radius: 2px;
       border-top-right-radius: 0px;
       border-bottom-right-radius: 0px;
       font-size: 20px;
       font-family: '微软雅黑';
       text-align: center;
    }
    QLineEdit:focus{
       border: 2px solid #63d0f7;
       border-right: 1px dashed #63d0f7;
    }
)";

const QString SS_input_line_edit_right = R"(
    QLineEdit{
       border: 1px solid #bdbdbd;
       border-left: none;
       border-top-left-radius: 0px;
       border-bottom-left-radius: 0px;
       border-top-right-radius: 0px;
       border-bottom-right-radius: 0px;
       font-size: 20px;
       font-family: '微软雅黑';
       text-align: center;
    }
    QLineEdit:focus{
       border: 2px solid #63d0f7;
       border-left: 1px dashed #63d0f7;
    }
)";

const QString SS_input_line_hyphen = R"(
    QLabel{
       border-top: 1px solid #bdbdbd;
       border-bottom: 1px solid #bdbdbd;
       border-left: none;
       border-right: none;
       font-size: 20px;
       font-weight: bold;
       font-family: '微软雅黑';
       border-radius: 0px;
    }
)";

//text-align: center;

const QString SS_plus_btn = R"(
    QPushButton{
       border:1px solid #bdbdbd;
       background-color:#ffffff;
       border-radius: 0px;
       font-size:16px;
       font-family:'微软雅黑';
    }
    QPushButton:pressed{
       border:1px solid #63d0f7;
    }
    QPushButton:hover{
       color:#63d0f7;
       background-color:#e5f2ff;
    }
)";

const QString SS_auto_plus_btn = R"(
    QPushButton{
       border:1px solid #bdbdbd;
       background-color:#e8e8ea;
       border-radius: 0px;
       font-size:10px;
       font-family:'微软雅黑';
       color: #2a2d32;
    }
    QPushButton:hover{
       background-color:#ffffff;
    }
    QPushButton:checked{
       color: #ffffff;
       border:1px solid #63d0f7;
       background-color:#63d0f7;
    }
)";

const QString SS_input_list_btn = R"(
    QPushButton{
       border:1px solid #bdbdbd;
       background-color:#f8f8f8;
       border-top-left-radius: 0px;
       border-bottom-left-radius: 0px;
       border-top-right-radius: 2px;
       border-bottom-right-radius: 2px;
       font-size:16px;
       font-family:'微软雅黑';
    }
    QPushButton:pressed{
       border:2px solid #63d0f7;
    }
)";

const QString SS_input_list_menu = R"(
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
        padding: 12px 12px;
        margin: 0px;
    }
    QMenu::item:selected {
        background-color: #f0f0f0;
    }
)";

const QString SS_input_list_widget = R"(
    QListWidget {
        background-color: #ffffff;
        border: 0px solid #0061b0;
        border-radius: 2px;
        padding: 0px;
        outline: none;
        font-size: 18px;
        font-family: '微软雅黑';
        color: #4d4d4d;
    }
    QListWidget::item {
        border: none;
        border-radius: 2px;
        padding: 5px 5px;
        margin: 0px;
        background: transparent;
    }
    QListWidget::item:hover {
        background-color: #f0f0f0;
    }
    QListWidget::item:selected {
        background-color: transparent;
        color: #4d4d4d;
    }
    /* 隐藏滚动条箭头 */
    QScrollBar:vertical {
        width: 13px;
        background: transparent;
    }
    QScrollBar::handle:vertical {
        background: #c0c0c0;
        border-radius: 3px;
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
)";

const QString SS_property_btn = R"(
    QPushButton{
       border: 1px dashed #bdbdbd;
       border-radius: 2px;
       background-color: rgba(229, 242, 255, 200);
       font-size:22px;
       font-family:'微软雅黑';
       font-style:italic;
       color:#63d0f7;
    }
    QPushButton:pressed{
       border:1px solid #63d0f7;
    }
    QPushButton:hover{
       border:1px solid #bdbdbd;
    }
)";






#endif // INPUT_PANEL_STYLE_H
