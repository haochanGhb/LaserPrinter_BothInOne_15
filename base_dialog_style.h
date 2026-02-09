#ifndef BASE_DIALOG_STYLE_H
#define BASE_DIALOG_STYLE_H

#include <QString>

const QString SS_BaseDialog = R"(
    QWidget{
       border: none;
       background: #3e3a39;
       padding: 0px;
    }
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
    QScrollBar:horizontal {
        height: 0px;
        background: transparent;
    }
    QListWidget QScrollBar:vertical {
        background-color: #ffffff;
        width: 14px;
        margin: 0px 0px 0px 0px;
        border-radius: 0px;
    }
)";

const QString SS_title_bar = R"(
    QWidget{
       border: none;
       background-color: #3e3a39;
       padding: 0px;
    }
)";


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


const QString SS_back_btn = R"(
                              QPushButton{
                                 border:none;
                                 background-color: #3e3a39;
                                 font-size:24px;
                                 font-family:'微软雅黑';
                                 color: #ffffff;
                                 padding-right: 10px;
                              }
                              QPushButton:hover{
                                 background-color:#4e4a49;
                              }
                          )";


const QString SS_datetime_btn= R"(
    QPushButton{
       border:none;
       background: transparent;
       font-size:16px;
       font-family:'微软雅黑';
       color: #f0f0f0;
    }
)";


const QString SS_main_area = R"(
    QWidget{
       border: none;
       background-color: #e8e8e8;
       padding: 0px;
    }
)";

const QString SS_bottom_bar = R"(
    QWidget{
       border: none;
       background-color: #ffffff;
       padding: 0px;
    }
)";

#endif // BASE_DIALOG_STYLE_H
