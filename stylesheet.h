#ifndef STYLESHEET_H
#define STYLESHEET_H

#include <QString>

/*************************************************
 *
 * MainWidget StyleSheet
 *
*************************************************/

const QString SS_title_bar = R"(
    QWidget{
       background-color: #3e3a39;
       padding: 0px;
    }
)";

const QString SS_MainWidget = R"(
    QWidget{
       background-color: #e8e8e8;
       padding: 0px;
    }
)";

const QString SS_title_area2= R"(
    QWidget{
       background-color: #2a2d32;
       padding: 0px;
    }
)";

const QString SS_input_area= R"(
    QWidget{
       background-color: #ffffff;
       border: none;
       border-radius: 3px;
    }
)";

const QString SS_preview_area= R"(
    QWidget{
       background-color: #c4ddf1;
       border-top-left-radius: 3px;
       border-bottom-left-radius: 3px;
       border-top-right-radius: 0px;
       border-bottom-right-radius: 0px;
    }
)";

const QString SS_keyboard_area= R"(
    QWidget{
       background-color: #e5f2ff;
       border: none;
       border-radius: 3px;
    }
)";


/*************************************************
 *
 * TitleBar
 *
*************************************************/

const QString SS_title = R"(
    QLabel{
       background: transparent;
       font-size: 24px;
       font-family: '微软雅黑';
       text-align: left;
       padding: 10px;
       color: #ffffff;
    }
)";

const QString SS_machine_status= R"(
    QWidget{
       background-color: #3e3a39;
       border: none;
    }
    QLabel{
       border:none;
       background: transparent;
       font-size:16px;
       font-family:'微软雅黑';
       color: #f0f0f0;
    }
)";

const QString SS_menu_btn = R"(
    QPushButton{
       border:none;
       background-color: #3e3a39;
       font-size:24px;
       font-family:'微软雅黑';
       color: #ffffff;
       padding-right: 10px;
    }
    QPushButton::menu-indicator {
       image: none;
        subcontrol-origin: padding;
        subcontrol-position: center right;
    }
    QPushButton:hover{
       background-color:#4e4a49;
    }
)";

const QString SS_main_menu = R"(
    QMenu {
        background-color: #ffffff;
        border: 2px solid #63d0f7;
        border-top-left-radius: 0px;
        border-top-right-radius: 0px;
        border-bottom-left-radius: 5px;
        border-bottom-right-radius: 5px;
        padding: 20px 10px;
    }
    QMenu::item {
        border: 1px solid #bcbcbc;
        border-radius: 2px;
        text-align: center;
        font-size: 24px;
        font-family: '微软雅黑';
        color: #4d4d4d;
        padding: 20px 20px;
        margin: 10px;
    }
    QMenu::item:selected {
        background-color: #d9f0f8;
    }
)";


const QString SS_quit_btn= R"(
    QPushButton{
       border:none;
       background-color: #3e3a39;
       font-size:24px;
       font-family:'微软雅黑';
       color: #ffffff;
    }
    QPushButton:hover{
       background-color:#4e4a49;
    }
)";




/*************************************************
 *
 * TaskListPanel
 *
*************************************************/
const QString SS_ProgressDialog = R"(
    QProgressDialog{
        border: 2px solid #63d0f7;
        background: #ffffff;
        border-radius: 3px;
    }
    QProgressBar{
        border: transparent;
        background: #ffffff;
        text-align: center;
    }
    QProgressBar::chunk{
        background: qlineargradient(
        spread: pad,
        x1: 0, y1: 0, x2: 1, y2: 0,
        stop: 0 #ffffff, stop: 0.5 #63d0f7, stop: 1 #ffffff
        );
    }
    QLabel{
       background: transparent;
       font-size: 16px;
       font-family: '微软雅黑';
       text-align: left;
       color: #2a2d32;
    }
)";

const QString SS_Task_Title = R"(
    QLabel{
       background: transparent;
       font-size: 23px;
       font-family: '微软雅黑';
       text-align: left;
       font-weight: bold;
       color: #2a2d32;
    }
)";

const QString SS_Statistics_Info = R"(
    QLabel{
       background: transparent;
       font-size: 20px;
       font-family: '微软雅黑';
       text-align: left;
       font-weight: normal;
       color: #2a2d32;
    }
)";

const QString SS_Title_Area = R"(
    QWidget{
       background-color: #ffffff;
       border-top-left-radius: 3px;
       border-top-right-radius: 3px;
       border-bottom-left-radius: 0px;
       border-bottom-right-radius: 0px;
       padding: 0px;
    }
)";

const QString SS_Table_Area= R"(
    QWidget{
       background-color: #ffffff;
       border-top-left-radius: 0px;
       border-top-right-radius: 0px;
       border-bottom-left-radius: 3px;
       border-bottom-right-radius: 3px;
       padding: 0px;
    }
)";

const QString SS_Button_Panel= R"(
    QWidget{
       background-color: #a2a2a2;
       border-top-left-radius: 0px;
       border-top-right-radius: 0px;
       border-bottom-left-radius: 3px;
       border-bottom-right-radius: 3px;
       padding: 0px;
    }
)";

const QString SS_Primary_Btn= R"(
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



const QString SS_Normal_Btn= R"(
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
                color:#bcbcbc;
    }
)";


const QString SS_Normal_Label= R"(
    QLabel{
       border:none;
       background-color:#ffffff;
       font-size:18px;
       font-family:'微软雅黑';
       color:#4d4d4d;
    }
)";



const QString SS_Max_Btn= R"(
    QPushButton{
       border:none;
       background-color:#ffffff;
       border-radius: 2px;
       font-size:20px;
       font-weight: bold;
       font-family:'微软雅黑';
       color:#0061b0;
    }
    QPushButton:hover{
       border:1px solid #bcbcbc;
       background-color:#e5f2ff;
    }
    QPushButton:pressed{
       border:1px solid #0061b0;
    }
)";

const QString SS_Task_Table= R"(
    QHeaderView::section {
       background-color: #898989;
       color: #ffffff;
       font-weight: bold;
       font-size:18px;
       font-family:'微软雅黑';
       border: 1px solid #bcbcbc;
    }
    QTableWidget {
       border: none;
       font-size:17px;
       font-family:'微软雅黑';
       color: #2a2d32;
       text-align: center;
       vertical-align: middle;
    }
    QTableWidget::item:selected {
       background-color: #d9f0f8;
       border: 0px ;
       border-radius: 3px;
       color: #2a2d32;
    }
    QScrollBar:vertical {
        background-color: #ffffff;
        width: 20px;
        margin: 2px 0px 0px 0px;
        border-radius: 3px;
    }
    QScrollBar::handle:vertical {
        background: #acacac;
        min-height: 20px;
        border-radius: 3px;
    }
    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
        height: 0px;
    }
    QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
        background: none;
    }
)";

/*
const QString SS_Task_Table_View = R"(
    QHeaderView::section {
       background-color: #898989;
       color: #ffffff;
       font-weight: bold;
       font-size: 18px;
       font-family: '微软雅黑';
       border: 1px solid #bcbcbc;
    }

    QTableView {
       border: none;
       font-size: 17px;
       font-family: '微软雅黑';
       color: #2a2d32;
       selection-background-color: #d9f0f8;
       selection-color: #2a2d32;
       gridline-color: #d0d0d0;
    }

    QTableView::item {
       padding: 0px;
    }

    QTableView::item:selected {
       background-color: #d9f0f8;
       color: #2a2d32;
    }

    QScrollBar:vertical {
        background-color: #ffffff;
        width: 20px;
        margin: 2px 0px 0px 0px;
        border-radius: 3px;
    }

    QScrollBar::handle:vertical {
        background: #acacac;
        min-height: 20px;
        border-radius: 3px;
    }

    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
        height: 0px;
    }

    QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
        background: none;
    }
)";*/

const QString SS_Task_Table_View = R"(
    QHeaderView::section {
        background-color: #898989;
        color: #ffffff;
        font-weight: bold;
        font-size: 18px;
        font-family: '微软雅黑';
        border: 1px solid #bcbcbc;
        padding: 4px;
    }

    QTableView {
        border: none;
        font-size: 17px;
        font-family: '微软雅黑';
        color: #2a2d32;
        selection-background-color: #d9f0f8;
        selection-color: #2a2d32;
        gridline-color: #d0d0d0;
        outline: 0; /* 去掉虚线框 */
    }

    QTableView::item {
        padding: 2px;
    }

    QTableView::item:selected {
        background-color: #d9f0f8;
        color: #2a2d32;
    }

    /* === 垂直滚动条 === */
    QScrollBar:vertical {
        width: 25px;
        background: transparent;
        margin: 0px;
    }
    QScrollBar::handle:vertical {
        background: #c0c0c0;
        border-radius: 3px;
        min-height: 35px;
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
    QScrollBar::add-page:vertical,
    QScrollBar::sub-page:vertical {
        background: none;
    }

    /* === 水平滚动条（隐藏或同风格）=== */
    QScrollBar:horizontal {
        height: 0px;
        background: transparent;
    }
)";



const QString SS_context_menu = R"(
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
        font-size: 18px;
        font-family: '微软雅黑';
        color: #4d4d4d;
        padding: 10px 30px 10px 10px;
        margin: 0px;
    }
    QMenu::item:selected {
        background-color: #f0f0f0;
    }
)";

/*************************************************
 *
 * StatusArea
 *
*************************************************/

/* background-color: #d8d8d8 */
/* color: #4e4e4e */
const QString SS_status_area= R"(
    QWidget{
       background-color: #3e3a39;
       border: none;
    }
    QLabel{
       border:none;
       background: transparent;
       font-size:16px;
       font-family:'微软雅黑';
       color: #f0f0f0;
    }
)";


/* #4e4e4e */
const QString SS_datetime_btn= R"(
    QPushButton{
       border:none;
       background: transparent;
       font-size:16px;
       font-family:'微软雅黑';
       color: #f0f0f0;
    }
)";

/*************************************************
 *
 * History
 *
*************************************************/
const QString SS_history_checkbox= R"(
QCheckBox {
    border: 1px solid gray;          /* 灰色边框 */
    border-radius: 2px;              /* 圆角 */
    padding: 5px;                    /* 内边距 */
    height: 25px;
    font-size:20px;
    font-family:'微软雅黑';
}

QCheckBox::indicator {
    width: 20px;                     /* 指示器宽度 */
    height: 20px;                    /* 指示器高度 */
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

const QString SS_quick_datetime_btn = R"(
    QPushButton{
        border:0px solid #bcbcbc;
        background-color:#e0f6fd;
        border-radius: 0px;
        font-size:20px;
        font-family:'微软雅黑';
        color:#4d4d4d;
    }
    QPushButton:hover{
        border:1px solid #50c8f4;

    }
    QPushButton:checked{
        background-color:#63d0f7;
        color:#ffffff;
    }
    QPushButton:disabled{
            background-color:#cccccc;
            color:#888888;
    }
)";


const QString SS_tabwidget_area = R"(
    QTabWidget::pane {
        border: 2px dashed #a0a0a0; /* 浅灰黑色虚线边框 */
        border-radius: 2px;
        padding: 5px;
    }
    QTabBar::tab {
        background: white;
        border: 2px solid #a0a0a0;
        border-bottom: none;
        border-top-left-radius: 2px;
        border-top-right-radius: 2px;
        padding: 8px 28px;
        font-size: 20px;
        font-family: '微软雅黑';
        color:#898989;
        min-height: 25px;
        min-width: 80px;
    }
    QTabBar::tab:selected {
        background: white;
        font-weight: bold;
        color:#000000;
    }
    QTabBar::tab:hover {
        background: #f0f0f0;
    }
)";

const QString SS_history_datetimeEdit = R"(

                                        QDateTimeEdit {
                                        border: 1px solid gray;          /* 默认灰色边框 */
                                        border-radius: 2px;              /* 圆角 */
                                        padding: 2px;                    /* 内边距 */
                                        height: 30px;
                                        color: #4d4d4d;
                                        font-size: 18px;
                                        font-family: '微软雅黑';
                                        }

                                        /* 获得焦点或进入编辑状态时 */
                                        QDateTimeEdit:focus, QDateTimeEdit:hover {
                                        border: 1px solid #79d6f9;       /* 焦点状态的蓝色边框 */
                                        }

                                        /* 下拉箭头按钮样式 */
                                        QDateTimeEdit::drop-down {
                                        border: none;                    /* 去除下拉按钮边框 */
                                        background: transparent;         /* 下拉按钮背景透明 */
                                        }

                                        /* 下拉箭头样式 */
                                        QDateTimeEdit::down-arrow {
                                        width: 10px;
                                        height: 10px;
                                        image: url(":/image/down_arrow_.png");  /* 替换为自己的箭头图片 */
                                        }

                                        /* 日历控件整体样式 */
                                        QCalendarWidget {
                                        border: 1px solid gray;          /* 日历控件的边框 */
                                        border-radius: 2px;              /* 日历控件的圆角 */
                                        background-color: white;         /* 日历背景色 */
                                        color: black;                    /* 默认文字颜色 */
                                        font-family: '微软雅黑';
                                        }

                                        /* 标题栏样式（包括背景色和文字色） */
                                        QCalendarWidget QToolButton {
                                        background-color: #f8f8f8;       /* 标题栏背景色 */
                                        color: #4d4d4d;                    /* 标题栏文字颜色 */
                                        font-size: 16px;
                                        border: none;
                                        height: 30px;
                                        margin: 0px;
                                        padding: 0px 5px;
                                        font-family: '微软雅黑';
                                        }

                                        /* 标题栏子区域样式 */
                                        QCalendarWidget QToolButton#qt_calendar_prevmonth,
                                        QCalendarWidget QToolButton#qt_calendar_nextmonth {
                                        background-color: #f8f8f8;       /* 左右翻页按钮背景色 */
                                        border-radius: 15px;             /* 翻页按钮圆角 */
                                        width: 24px;
                                        height: 24px;
                                        }

                                        /* 中间区域（年份和月份选择） */
                                        QCalendarWidget QWidget#qt_calendar_navigationbar {
                                        background-color: #f8f8f8;       /* 完全覆盖标题栏背景 */
                                        border: none;
                                        }

                                        /* 悬停效果 */
                                        QCalendarWidget QToolButton:hover {
                                        background-color: #f0f0f0;       /* 鼠标悬停时按钮背景色 */
                                        }

                                        /* 日历视图部分 */
                                        QCalendarWidget QAbstractItemView {
                                        background-color: white;         /* 日历背景 */
                                        selection-background-color: #63d0f7; /* 选中日期背景色 */
                                        selection-color: white;          /* 选中日期文字颜色 */
                                        }

                                        /* 取消焦点虚线框 */
                                        QCalendarWidget QWidget {
                                        outline: 0px;
                                        }

                                        QMenu::item:selected {
                                            background-color: #63d0f7;  /* 选中背景 */
                                            color: white;               /* 选中文字颜色 */
                                        }
)";

const QString SS_Button_Panel1= R"(
    QWidget{
       background-color: #f2f2f2;
       border-top-left-radius: 3px;
       border-top-right-radius: 3px;
       border-bottom-left-radius: 3px;
       border-bottom-right-radius: 3px;
       padding: 0px;
    }
)";

const QString SS_White_Round_Corner_Panel= R"(
    QWidget{
       background-color: #ffffff;
       border-top-left-radius: 3px;
       border-top-right-radius: 3px;
       border-bottom-left-radius: 3px;
       border-bottom-right-radius: 3px;
       padding: 0px;
    }
)";

const QString SS_LicenseNotice_Label= R"(
    QLabel{
       border: none;
       background: transparent;
       font-size: 14px;
       font-family: '微软雅黑';
       text-align: center;
       padding: 10px;
       color: #bdbdbd;
    }
)";

#endif // STYLESHEET_H
