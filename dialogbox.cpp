#include "dialogbox.h"
#include "dialogbox_style.h"
#include "MarkHandle.h"
#include <QMutex>

// 初始化静态成员变量
QString DialogMsgBox::m_currentDisplayedContent = "";
QMutex DialogMsgBox::m_contentMutex;

DialogMsgBox::DialogMsgBox(BoxType type, QDialog *parent) : QDialog(parent)
{

    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);

    create_widget();
    set_stylesheet();

    connect(close_btn, &QPushButton::clicked, this, &DialogMsgBox::on_close_btn_clicked);
    connect(ok_btn, &QPushButton::clicked, this, &DialogMsgBox::on_ok_btn_clicked);
    connect(cancel_btn, &QPushButton::clicked, this, &DialogMsgBox::on_cancel_btn_clicked);

    setBoxType(type);
    boxType = type;
    //当对话框显示时启动定时器
    connect(timer, &QTimer::timeout, this, &DialogMsgBox::updateCountdown);

    // 连接对话框关闭信号，清除当前显示的内容
    connect(this, &QDialog::finished, this, [=](int result) {
        QMutexLocker locker(&m_contentMutex);
        m_currentDisplayedContent.clear();
    });
}

void DialogMsgBox::setBoxType(BoxType type)
{

    switch(type)
    {
    case ASK_DIALOG:
        ask_dialog_widget_visible(true);
        input_dialog_widget_visible(false);
        break;
    case INFO_DIALOG:
        ask_dialog_widget_visible(true);
        input_dialog_widget_visible(false);
        icon_label->setStyleSheet("QLabel{border: none;image: url(:/image/info.png);}");
        ok_btn->setVisible(true);
        ok_btn->setGeometry(120,390,120,40);
        cancel_btn->setVisible(false);
        break;
    case INPUT_DIALOG:
        ask_dialog_widget_visible(false);
        input_dialog_widget_visible(true);
        break;
    case AUTO_CLOSE_DIALOG:
        ask_dialog_widget_visible(true);
        input_dialog_widget_visible(false);
        icon_label->setStyleSheet("QLabel{border: none;image: url(:/image/info.png);}");
        ok_btn->setVisible(true);
        ok_btn->setGeometry(120,390,120,40);
        cancel_btn->setVisible(false);
        break;
    case TEXT_EDIT_DIALOG:
        ask_dialog_widget_visible(false);
        input_dialog_widget_visible(false);
        text_edit->setVisible(true);
        ok_btn->setVisible(true);
        cancel_btn->setVisible(true);
        break;

    default:
        break;
    }
}

void DialogMsgBox::setTitle(const QString &strText)
{
    title_label->setText(strText);
}

void DialogMsgBox::setContent(const QString &strText)
{
    content_label->setText(strText);
}

void DialogMsgBox::setInputTips(const QString &strText)
{
    input_tips->setText(strText);
}

void DialogMsgBox::setInputBoxText(const QString &strText)
{
    input_box->setText(strText);
}

void DialogMsgBox::setAcceptBtnText(const QString &strText)
{
    ok_btn->setText(strText);
}

void DialogMsgBox::setCancelBtnText(const QString &strText)
{
    cancel_btn->setText(strText);
}

void DialogMsgBox::setTextEditContent(const QString &strText)
{
    text_edit->setPlainText(strText);
}
QString DialogMsgBox::getTextEditContent()
{
    return text_edit->toPlainText();
}

QString DialogMsgBox::getInputText()
{
    return input_box->text();
}

bool DialogMsgBox::isContentAlreadyDisplayed(const QString &content)
{
    QMutexLocker locker(&m_contentMutex);
    // 当内容为空时，总是允许弹出对话框
    if (content.isEmpty()) {
        return false;
    }
    return m_currentDisplayedContent == content;
}

int DialogMsgBox::exec()
{
    // 根据对话框类型获取关键内容进行重复检查
    QString keyContent;
    switch(boxType)
    {
    case INPUT_DIALOG:
        // 对于输入对话框，使用输入提示作为关键内容
        keyContent = input_tips->text();
        break;
    case TEXT_EDIT_DIALOG:
        // 对于文本编辑对话框，使用文本编辑控件的内容作为关键内容
        keyContent = text_edit->toPlainText();
        break;
    default:
        // 对于其他类型的对话框，使用内容标签作为关键内容
        keyContent = content_label->text();
        break;
    }

    // 检查内容是否已经显示
    if (isContentAlreadyDisplayed(keyContent))
    {
        return QDialog::Rejected; // 返回拒绝状态，表示未显示新对话框
    }

    // 设置当前显示的内容
    {
        QMutexLocker locker(&m_contentMutex);
        m_currentDisplayedContent = keyContent;
    }

    return QDialog::exec();
}

int DialogMsgBox::Exec(quint8 level,bool beepOn,MarkHandle * pMarkHandle)
{
    // 根据对话框类型获取关键内容进行重复检查
    QString keyContent;
    switch(boxType)
    {
    case INPUT_DIALOG:
        // 对于输入对话框，使用输入提示作为关键内容
        keyContent = input_tips->text();
        break;
    case TEXT_EDIT_DIALOG:
        // 对于文本编辑对话框，使用文本编辑控件的内容作为关键内容
        keyContent = text_edit->toPlainText();
        break;
    default:
        // 对于其他类型的对话框，使用内容标签作为关键内容
        keyContent = content_label->text();
        break;
    }

    // 检查内容是否已经显示
    if (isContentAlreadyDisplayed(keyContent)) {
        return QDialog::Rejected; // 返回拒绝状态，表示未显示新对话框
    }

    // 设置当前显示的内容
    {
        QMutexLocker locker(&m_contentMutex);
        m_currentDisplayedContent = keyContent;
    }

    if (beepOn && pMarkHandle != nullptr)
    {
        pMarkHandle->beepPlay(level);
    }
    int ret = QDialog::exec(); // 直接调用基类的 exec() 方法，避免重复检查
    if (pMarkHandle != nullptr)
    {
        pMarkHandle->beepPlay(0);
        if((ret == QDialog::Accepted) && (level < 2))
        {
            pMarkHandle->restart();
        }
    }
    return ret;
}

void DialogMsgBox::create_widget()
{
    this->setFixedSize(360,470);
    close_btn = new QPushButton(this);
    close_btn->setGeometry(310,10,40,40);
    close_btn->setFocusPolicy(Qt::NoFocus);

    title_label = new QLabel(this);
    title_label->setGeometry(0,0,300,60);
    icon_label = new QLabel(this);
    icon_label->setGeometry(130,80,100,100);
    content_label = new QLabel(this);
    content_label->setGeometry(50,200,260,160);
    content_label->setAlignment(Qt::AlignCenter);
    content_label->setWordWrap(true);//自动换行
    ok_btn = new QPushButton(this);
    ok_btn->setGeometry(40,390,120,40);
    ok_btn->setText(tr("确定"));
    ok_btn->setShortcut(QKeySequence(Qt::Key_Enter));

    cancel_btn = new QPushButton(this);
    cancel_btn->setGeometry(200,390,120,40);
    cancel_btn->setText(tr("取消"));
    cancel_btn->setShortcut(QKeySequence(Qt::Key_Escape));

    input_tips = new QLabel(this);
    input_tips->setGeometry(30,180,300,60);
    input_tips->setAlignment(Qt::AlignLeft);
    input_box = new QLineEdit(this);
    input_box->setGeometry(30,220,300,40);
    input_box->installEventFilter(this);

    text_edit = new QTextEdit(this);
    text_edit->setGeometry(30,80,300,280);
    text_edit->hide();
    text_edit->installEventFilter(this);

    // 定时器
    timer = new QTimer(this);
    timer->setInterval(1000);

}

void DialogMsgBox::set_stylesheet()
{
    this->setStyleSheet(SS_DialogBox);
    close_btn->setStyleSheet(SS_close_btn);

    title_label->setStyleSheet(SS_title_label);
    icon_label->setStyleSheet(SS_icon_label);
    content_label->setStyleSheet(SS_TextLabel);
    ok_btn->setStyleSheet(SS_Primary_Dlg_Btn);
    cancel_btn->setStyleSheet(SS_Normal_Dlg_Btn);

    input_tips->setStyleSheet(SS_TextLabel);
    input_box->setStyleSheet(SS_input_box);
    text_edit->setStyleSheet(SS_text_edit);
}

void DialogMsgBox::ask_dialog_widget_visible(bool tf)
{
    icon_label->setVisible(tf);
    content_label->setVisible(tf);
}

void DialogMsgBox::input_dialog_widget_visible(bool tf)
{
    input_tips->setVisible(tf);
    input_box->setVisible(tf);
}

void DialogMsgBox::on_close_btn_clicked()
{
    stopTimer();
    this->close();
    this->deleteLater();
}

void DialogMsgBox::on_ok_btn_clicked()
{
    stopTimer();
    this->accept();
    this->deleteLater();
}

void DialogMsgBox::on_cancel_btn_clicked()
{
    stopTimer();
    this->reject();
    this->deleteLater();
}

void DialogMsgBox::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        isDraggable = true;
        event->accept();
    }
    else
    {
        isDraggable = false;
    }
}

void DialogMsgBox::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::LeftButton && isDraggable)
    {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}

void DialogMsgBox::mouseReleaseEvent(QMouseEvent *event)
{
    isDraggable = false;
}

bool DialogMsgBox::eventFilter(QObject *obj, QEvent *ev)
{
    // 处理焦点进入事件，支持触摸键盘
    if(ev->type() == QEvent::FocusIn)
    {
        // 对于QLineEdit，调用TouchKeyboardManager处理焦点进入
        if(QLineEdit *lineEdit = qobject_cast<QLineEdit *>(obj))
        {

            TouchKeyboardManager::instance().handleFocusIn(lineEdit);
        }
        // text_edit
        else if(QTextEdit *qTextEdit = qobject_cast<QTextEdit *>(obj))
        {
            TouchKeyboardManager::instance().handleFocusIn(qTextEdit);
        }

        // 无论是什么控件，获得焦点时都确保不被阻塞
        obj->setProperty("blocked", false);
        if(QWidget *widget = qobject_cast<QWidget *>(obj))
        {
            // 确保窗口属性正确设置
//            if(TouchKeyboardManager::isEnabled())
//            {
//                TouchKeyboardManager::allowAutoInvoke(widget);
//            }
            widget->update();
            widget->setEnabled(true); // 确保控件启用
        }
    }

    return QDialog::eventFilter(obj, ev);
}
