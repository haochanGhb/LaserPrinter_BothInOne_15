#ifndef CODECSELECTDIALOG_H
#define CODECSELECTDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QComboBox>
#include <QLineEdit>
#include <QBoxLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QTextEdit>
#include <QDebug>
#include <QListView>

#include "public_fun.h"


const QString SS_ParserSelectDialog = R"(
    QDialog{
       border: 2px solid #63d0f7;
       background-color:#ffffff;
       border-radius: 3px;
    }
    QPushButton{
       border:1px solid #bcbcbc;
       background-color:#ffffff;
       border-radius: 2px;
       font-size:16px;
       font-family:'微软雅黑';
       color:#2a2d32;
    }
    QPushButton:hover{
       background-color:#e8e8e8;
    }
    QPushButton:pressed{
       background-color:#ffffff;
    }
    QPushButton:disabled{
       background-color:#f5f5f7;
       color:#bcbcbc;
    }
    QLabel{
       border: none;
       background: transparent;
       font-size: 16px;
       font-family: '微软雅黑';
       text-align: center;
       padding: 0px;
       color: #2a2d32;
    }
    QLineEdit{
       border: 1px solid #bdbdbd;
       border-radius: 2px;
       font-size: 16px;
       font-family: '微软雅黑';
       padding-left: 2px;
       padding-right: 2px;
       text-align: left;
    }
    QLineEdit:focus{
       border: 1px solid #0061b0;
    }
    QLineEdit:disabled{
        background-color: #f5f5f7;
    }
    QTextEdit{
       border: 1px solid #bdbdbd;
       border-radius: 2px;
       font-size: 16px;
       font-family: '微软雅黑';
       padding-left: 2px;
       padding-right: 2px;
       color: #2a2d32;
    }
    QTextEdit:focus{
       border: 1px solid #0061b0;
    }
    QComboBox {
        background-color: #ffffff;
        border: 1px solid #bdbdbd;
        border-radius: 2px;
        font-size: 16px;
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
        border: 2px solid #63d0f7;
        border-radius: 2px;
        font-size: 16px;
        font-family:'微软雅黑';
        color: #4d4d4d;
        outline: 0px;
        padding: 1px;
    }
    QComboBox QAbstractItemView::item{
        padding: 3px;
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
        border-radius: 2px;
        background: #acacac;
    }
    QRadioButton{
       font-size: 16px;
       font-family: '微软雅黑';
       text-align: left;
       color: #2a2d32;
    }
    QRadioButton:checked{
       color: #0061b0;
    }
    QRadioButton:disabled{
       color: #2a2d32;
    }
    QRadioButton::indicator{
       width: 30px;
       height: 30px;
    }
)";


const QString SS_ParserSelectDialogTitleLabel= R"(
    QLabel{
       border: none;
       background: transparent;
       font-size: 18px;
       font-family: '微软雅黑';
       font-weight: bold;
       text-align: left;
       padding-left: 5px;
       color: #2a2d32;
    }
)";

const QString SS_ParserSelectDialogCloseBtn= R"(
    QPushButton{
       border: none;
       background-image: url(:/image/close.png);
       background-position:center;
    }
)";

const QString SS_ParserSelectDialogPrimaryBtn= R"(
    QPushButton{
       border:none;
       background-color:#63d0f7;
       border-radius: 2px;
       font-size:18px;
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

const QString SS_ParserSelectDialogNormalBtn= R"(
    QPushButton{
       border:1px solid #bcbcbc;
       background-color:#ffffff;
       border-radius: 2px;
       font-size:18px;
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

const QString SS_ParserSelectCombox = R"(
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


class CodecSelectDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CodecSelectDialog(QDialog *parent = nullptr);
    void setTargetFileName(const QString name);
    QString getSelectedCodec();
private:

    QLabel *m_pLabelDialogTitle;

    QPushButton *m_pBtnOk;
    QPushButton *m_pBtnCancel;
    QPushButton *m_pBtnClose;

    QVBoxLayout *m_pVInputAreaLayout;


    QLabel *m_pLabelTargetFileName;
    QComboBox *m_pCbbCodec;

    void setupBaseUI();
    void setupStyleSheet();

    void setupMainAreaUI();

    void saveSettingsToFile();
    void readSettingsFromFile();

signals:

private slots:
    void slotOnBtnCloseClicked();
    void slotOnBtnOkClicked();
    void slotOnBtnCancelClicked();

protected:
    QPoint m_dragPosition;
    bool m_isDraggable = false;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;
};

#endif // CODECSELECTDIALOG_H
