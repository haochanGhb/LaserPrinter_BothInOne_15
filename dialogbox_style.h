#ifndef DIALOGBOX_STYLE_H
#define DIALOGBOX_STYLE_H

#include <QString>

const QString SS_DialogBox = R"(
    QDialog{
       border: 2px solid #63d0f7;
       background-color:#ffffff;
       border-radius: 5px;
    }
)";

const QString SS_close_btn= R"(
    QPushButton{
       border: none;
       background-image: url(:/image/close.png);
       background-position:center;
    }
)";

const QString SS_title_label= R"(
    QLabel{
       border: none;
       background: transparent;
       font-size: 26px;
       font-family: '微软雅黑';
       font-weight: bold;
       text-align: left;
       padding-left: 20px;
       color: #2a2d32;
    }
)";

const QString SS_icon_label= R"(
    QLabel{
       border: none;
       image: url(:/image/ask.png);
    }
)";

const QString SS_TextLabel= R"(
    QLabel{
       border: none;
       background: transparent;
       font-size: 20px;
       font-family: '微软雅黑';
       text-align: center;
       padding: 0px;
       color: #2a2d32;
    }
)";

const QString SS_input_box= R"(
    QLineEdit{
       border: 1px solid #bdbdbd;
       border-radius: 2px;
       font-size: 20px;
       font-family: '微软雅黑';
       padding-left: 10px;
       padding-right: 10px;
       text-align: left;
    }
    QLineEdit:focus{
       border: 2px solid #63d0f7;
    }
)";

const QString SS_text_edit= R"(
    QTextEdit{
       border: 1px solid #bdbdbd;
       border-radius: 2px;
       font-size: 20px;
       font-family: '微软雅黑';
       padding-left: 10px;
       padding-right: 10px;
       padding-top: 10px;
       padding-bottom: 10px;
       text-align: left;
    }
    QTextEdit:focus{
       border: 2px solid #63d0f7;
    }
)";


const QString SS_Primary_Dlg_Btn= R"(
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
)";


const QString SS_Normal_Dlg_Btn= R"(
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
)";

const QString SS_refresh_btn= R"(
    QPushButton{
       border: none;
       background-position:center;
       font-size:20px;
       font-family:'微软雅黑';
       color:#2a2d32;
    }
    QPushButton:disabled{
       color:#bcbcbc;
    }
)";



const QString SS_ListWidget= R"(
    QListWidget{
       border:1px solid #bcbcbc;
       background-color:#ffffff;
       border-radius: 2px;
       font-size:16px;
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
        border: 2px solid #0061b0;
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
)";




#endif // DIALOGBOX_STYLE_H
