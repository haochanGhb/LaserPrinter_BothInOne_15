#include "codec_select_dialog.h"
#include <QTextCodec>
#include <QSettings>

//#include "ParserDialogStyle.h"

CodecSelectDialog::CodecSelectDialog(QDialog *parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);

    setupBaseUI();
    setupStyleSheet();

    connect(m_pBtnClose, &QPushButton::clicked, this, &CodecSelectDialog::slotOnBtnCloseClicked);
    connect(m_pBtnOk, &QPushButton::clicked, this, &CodecSelectDialog::slotOnBtnOkClicked);
    connect(m_pBtnCancel, &QPushButton::clicked, this, &CodecSelectDialog::slotOnBtnCancelClicked);

    setupMainAreaUI();
    readSettingsFromFile();

}

void CodecSelectDialog::setTargetFileName(const QString name)
{
    m_pLabelTargetFileName->setText(name);
}

QString CodecSelectDialog::getSelectedCodec()
{
    return m_pCbbCodec->currentText();
}

void CodecSelectDialog::setupBaseUI()
{
    this->setFixedSize(320,260);
    m_pBtnClose = new QPushButton(this);
    m_pBtnClose->setFixedSize(30,30);
    m_pBtnClose->setFocusPolicy(Qt::NoFocus);

    m_pLabelDialogTitle = new QLabel(this);
    m_pLabelDialogTitle->setFixedHeight(30);

    m_pBtnOk = new QPushButton(this);
    m_pBtnOk->setFixedSize(100,35);
    m_pBtnOk->setText(tr("确定"));
//    ok_btn->setShortcut(QKeySequence(Qt::Key_Enter));

    m_pBtnCancel = new QPushButton(this);
    m_pBtnCancel->setFixedSize(100,35);
    m_pBtnCancel->setText(tr("取消"));

    QHBoxLayout *pHTitleBarLayout = new QHBoxLayout();
    pHTitleBarLayout->addWidget(m_pLabelDialogTitle);
    pHTitleBarLayout->addStretch();
    pHTitleBarLayout->addWidget(m_pBtnClose);

    m_pVInputAreaLayout = new QVBoxLayout();
    m_pVInputAreaLayout->setContentsMargins(15,0,15,0);
    m_pVInputAreaLayout->setSpacing(5);

    QHBoxLayout *pHBtnLayout = new QHBoxLayout();
    pHBtnLayout->addStretch();
    pHBtnLayout->addWidget(m_pBtnOk);
    pHBtnLayout->addWidget(m_pBtnCancel);

    QVBoxLayout *pVMainLayout = new QVBoxLayout();
    pVMainLayout->addLayout(pHTitleBarLayout);
    pVMainLayout->addStretch();
    pVMainLayout->addLayout(m_pVInputAreaLayout);
    pVMainLayout->addStretch();
    pVMainLayout->addLayout(pHBtnLayout);

    this->setLayout(pVMainLayout);

}

void CodecSelectDialog::setupStyleSheet()
{
//    this->setStyleSheet(SS_DialogBox);
//    m_pLabelDialogTitle->setStyleSheet(SS_title_label);
//    m_pBtnClose->setStyleSheet(SS_close_btn);
//    m_pBtnOk->setStyleSheet(SS_Primary_Dlg_Btn);
//    m_pBtnCancel->setStyleSheet(SS_Normal_Dlg_Btn);

    this->setStyleSheet(SS_ParserSelectDialog);
    m_pLabelDialogTitle->setStyleSheet(SS_ParserSelectDialogTitleLabel);
    m_pBtnClose->setStyleSheet(SS_ParserSelectDialogCloseBtn);
    m_pBtnOk->setStyleSheet(SS_ParserSelectDialogPrimaryBtn);
    m_pBtnCancel->setStyleSheet(SS_ParserSelectDialogNormalBtn);

}

void CodecSelectDialog::setupMainAreaUI()
{
    m_pLabelDialogTitle->setText(tr("编码类型"));

    m_pLabelTargetFileName = new QLabel(this);
    QHBoxLayout *pHLayoutTargetFileName = new QHBoxLayout();
    pHLayoutTargetFileName->setContentsMargins(5, 0, 10, 0);
    pHLayoutTargetFileName->setSpacing(2);
    pHLayoutTargetFileName->addWidget(new QLabel(tr("文件名")));
    pHLayoutTargetFileName->addStretch();
    pHLayoutTargetFileName->addWidget(m_pLabelTargetFileName);
    pHLayoutTargetFileName->addStretch();

    m_pCbbCodec = new QComboBox(this);
    m_pCbbCodec->setFixedSize(156, 30);
    m_pCbbCodec->setView(new QListView());

    QList<QByteArray> codecs = QTextCodec::availableCodecs();
    for (const QByteArray &codec_name : codecs)
    {
        m_pCbbCodec->addItem(codec_name);
    }

    QHBoxLayout *pHLayoutCodec = new QHBoxLayout();
    pHLayoutCodec->setContentsMargins(5, 0, 10, 0);
    pHLayoutCodec->setSpacing(2);
    pHLayoutCodec->addWidget(new QLabel(tr("编码类型")));
    pHLayoutCodec->addStretch();
    pHLayoutCodec->addWidget(m_pCbbCodec);

    m_pVInputAreaLayout->addLayout(pHLayoutTargetFileName);
    m_pVInputAreaLayout->addLayout(pHLayoutCodec);
    m_pVInputAreaLayout->addStretch();

}

void CodecSelectDialog::saveSettingsToFile()
{
//    QSettings settings_ini(PARSER_SELECT_INI, QSettings::IniFormat);

//    settings_ini.beginGroup("last_codec_select");
//    settings_ini.setValue("codec", m_pCbbCodec->currentText());
//    settings_ini.endGroup();
}

void CodecSelectDialog::readSettingsFromFile()
{
//    QString strCodec;
//    QSettings settings_ini(PARSER_SELECT_INI, QSettings::IniFormat);

//    RemoteTextSetting parserSetting;
//    settings_ini.beginGroup("last_codec_select");
//    strCodec = settings_ini.value("codec", "").toString();
//    settings_ini.endGroup();
//    m_pCbbCodec->setCurrentText(strCodec);
}

void CodecSelectDialog::slotOnBtnCloseClicked()
{
    this->close();
    this->deleteLater();
}

void CodecSelectDialog::slotOnBtnOkClicked()
{
    saveSettingsToFile();
    this->accept();
    this->deleteLater();
}

void CodecSelectDialog::slotOnBtnCancelClicked()
{
    this->reject();
    this->deleteLater();
}

void CodecSelectDialog::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        m_isDraggable = true;
        event->accept();
    }
    else
    {
        m_isDraggable = false;
    }
}

void CodecSelectDialog::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::LeftButton && m_isDraggable)
    {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

void CodecSelectDialog::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_isDraggable = false;
}

void CodecSelectDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter)
    {
        //event->ignore();
    }
    else
    {
        QDialog::keyPressEvent(event);
    }
}
