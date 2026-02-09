#include "main_window.h"

#include <QApplication>
#include <QSplashScreen>
#include <QSharedMemory>
#include <QMessageBox>
#include <QClipboard>
#include "MachineCode/MachineCode.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSharedMemory shared_memory("LaserPrinterApplication");
    if(!shared_memory.create(1))
    {
//        DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
//        msg_box->setContent(QObject::tr("程序已在运行中！"));
//        msg_box->exec();
        return 0;
    }


    #ifdef USE_CASSETTE
        QString machineCode = MachineCode::generateMachineCode("ZTCP-6");
    #elif defined(USE_SLIDE)
        QString machineCode = MachineCode::generateMachineCode("ZMSP-1");
    #elif defined(USE_SLIDE_DOUBLE)
        QString machineCode = MachineCode::generateMachineCode("ZMSP-2");
    #endif

        qDebug() << "machineCode:" << machineCode;

        if(machineCode.isEmpty())
        {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setWindowTitle("Hint");
            msgBox.setText("Failed to retrieve device information. Please try again!");
            msgBox.exec();
            return -1;
        }
        QByteArray activationCodeFromServer;
        QFile file("./license.lic");
        if (file.open(QIODevice::ReadOnly))
        {
            QTextStream in(&file);
            in.setCodec("UTF-8");
            activationCodeFromServer = in.readAll().toUtf8();
            file.close();
        }

        bool isValid = MachineCode::verifyActivationCode(machineCode, activationCodeFromServer, "://pem/public.pem");
        if (isValid)
        {
            qDebug() << "verify success!";
        }
        else
        {
            qDebug() << "verify failed!";
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setWindowTitle("Verify failed");
            msgBox.setText("System unauthorized, please contact the supplier.");
            msgBox.setInformativeText("Machine code:" + machineCode);
            msgBox.setTextInteractionFlags(Qt::TextSelectableByMouse);
            msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            msgBox.setButtonText(QMessageBox::Ok, "Copy");
            if (msgBox.exec() == QMessageBox::Ok)
            {
                QClipboard *clipboard = QApplication::clipboard();
                clipboard->setText(machineCode);
                QMessageBox::information(nullptr,
                        "Information",
                        "The machine code has been copied to the clipboard.",
                        QMessageBox::Ok
                    );
            }
            return -1;
        }

    MainSettings settings;
    SettingDialog::ReadSettings(&settings);
    //加载翻译器
    SettingDialog::ChangeTranslator(settings.language_setting.language_cbb);

    QString sfile = ":/image/boot.png";
    if(settings.language_setting.language_cbb.compare("简体中文") == 0)
    {
        sfile = ":/image/boot.png";
    }
    else if(settings.language_setting.language_cbb.compare("English") == 0)
    {
        sfile = ":/image/boot.png";
    }


    QPixmap pixmap(sfile);
    QSplashScreen splash(pixmap);

    // 设置字体
    QFont font = splash.font();
    font.setFamily("Microsoft YaHei");
    font.setPointSize(22);
    font.setBold(true);
    splash.setFont(font);

    splash.showMessage(QString(QObject::tr("软件启动中，请稍候...")),Qt::AlignHCenter | Qt::AlignVCenter, QColor(89,87,88));
    //a.processEvents();
    splash.show();
    a.processEvents();

    MainWindow w;
    splash.finish(&w);
    QThread::msleep(20);
    w.show();
    return a.exec();
}
