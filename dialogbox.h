#ifndef DIALOGMSGBOX_H
#define DIALOGMSGBOX_H


#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTimer>
#include <QShowEvent>
#include <QCloseEvent>
#include <QTextEdit>
#include <QApplication>
#include <QMutex>
#include "TouchKeyboardManager.h"

typedef enum
{
    ASK_DIALOG,
    INFO_DIALOG,
    INPUT_DIALOG,
    AUTO_CLOSE_DIALOG,
    TEXT_EDIT_DIALOG
}BoxType;

class MarkHandle;

class DialogMsgBox : public QDialog
{
    Q_OBJECT
public:
    explicit DialogMsgBox(BoxType type, QDialog *parent = nullptr);
    void setBoxType(BoxType type);
    void setTitle(const QString &strText);
    void setContent(const QString &strText);
    void setInputTips(const QString &strText);
    void setInputBoxText(const QString &strText);
    void setAcceptBtnText(const QString &strText);
    void setCancelBtnText(const QString &strText);
    void setTextEditContent(const QString &strText);
    QString getTextEditContent();
    QString getInputText();
    int Exec(quint8 level,bool beepOn,MarkHandle * pMarkHandle);
    
    // 重写基类的 exec() 方法，确保所有调用方式都能避免重复显示
    int exec() override;
    
    // 静态方法检查内容是否已经显示
    static bool isContentAlreadyDisplayed(const QString &content);
    
private:
    // 静态成员变量保存当前显示的对话框内容
    static QString m_currentDisplayedContent;
    static QMutex m_contentMutex; // 线程安全互斥锁

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    
    // 重写 closeEvent 以在对话框关闭时隐藏触摸键盘
    void closeEvent(QCloseEvent *event) override {

        QDialog::closeEvent(event);
        TouchKeyboardManager::suppressTouchKeyboard(this);
        TouchKeyboardManager::hide();
    }

    // 重写 showEvent 以捕获对话框的显示事件
    void showEvent(QShowEvent *event) override {
        QDialog::showEvent(event);
        //startAutoCloseTimer();

        //把edit_panel里的input_panel编辑框的焦点转移到此处
        QWidget *currentFocusWidget = QApplication::focusWidget();
        if (currentFocusWidget)
        {
            currentFocusWidget->clearFocus();
        }

        if(input_box->isVisible())
        {
            input_box->setFocus();
        }
    }

   bool eventFilter(QObject *obj,QEvent *ev) override;

private:

    QLabel *title_label;
    QLabel *icon_label;
    QLabel *content_label;
    QPushButton *ok_btn;
    QPushButton *cancel_btn;
    QPushButton *close_btn;

    QLabel *input_tips;
    QLineEdit *input_box;

    QTextEdit *text_edit;

    QTimer *timer;
    int countdown;

    BoxType boxType;
    void create_widget();
    void set_stylesheet();

    void ask_dialog_widget_visible(bool tf);
    void input_dialog_widget_visible(bool tf);

    QPoint dragPosition;
    bool isDraggable = false;


signals:

private slots:
    void on_close_btn_clicked();
    void on_ok_btn_clicked();
    void on_cancel_btn_clicked();

    // 启动定时器的槽函数
    void startAutoCloseTimer() {
        stopTimer();
        if(boxType == BoxType::AUTO_CLOSE_DIALOG)
        {
            countdown = 10;
            ok_btn->setText(tr("确定") + QString("(%1)").arg(QString::number(countdown)));
            timer->start();
        }
    }

    void stopTimer() {
        if (timer->isActive())
        {
            timer->stop();
        }
    }

    // 更新倒计时
    void updateCountdown() {
        countdown--;
        if (countdown > 0) {
            ok_btn->setText(tr("确定") + QString("(%1)").arg(QString::number(countdown)));
        } else {
            stopTimer();
            this->close();
        }
    }

};

#endif // DIALOGMSGBOX_H
