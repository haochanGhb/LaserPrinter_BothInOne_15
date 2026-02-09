#include "about_dialog.h"
#include "menu_dialog_style.h"
#include "main_window.h"

AboutDialog::AboutDialog(QWidget *parent) :
    BaseDialog(dynamic_cast<BaseDialog*>(parent))
  , mainWindow(nullptr)
{
    // 尝试将parent转换为MainWindow指针
    mainWindow = qobject_cast<MainWindow*>(parent);

    setTitle(tr("关于"));
    create_widget();
    setup_layout();
    set_stylesheet();
    set_about_info();
    connect_signals_and_slots();
}

void AboutDialog::showEvent(QShowEvent *event)
{
    firmware_version->setText(QString(tr("底层软件版本:")+ "  V%1").arg("V0.0.0"));
    QTimer::singleShot(100, this, [this]()
    {
        QString mcuVersion;
        if(mainWindow)
        {
            mcuVersion = mainWindow->getMcuVersion();
            if(mcuVersion.isEmpty())
            {
                mcuVersion = "0.0.0";
            }
            firmware_version->setText(QString(tr("底层软件版本:")+ "  V%1").arg(mcuVersion));
        }
    });
}

void AboutDialog::create_widget()
{
    about_panel = new QWidget(this);
    about_panel->setFixedSize(780, 1150); //(720, 1060)

    logo = new QLabel(this);
    barcode = new QLabel(this);
    website = new QLabel(this);
    device_name = new QLabel(this);
    application_version = new QLabel(this);
    firmware_version = new QLabel(this);

    license_notice = new QLabel(this);

//    //QPixmap logo_pixmap(":/image/logo.png");
//    QPixmap logo_pixmap("./images_about/logo.png");
//    QPixmap logo_scaled = logo_pixmap.scaled(QSize(260, 260), Qt::KeepAspectRatio, Qt::SmoothTransformation);
//    logo->setPixmap(logo_scaled);
//    logo->setFixedSize(logo_scaled.size());

//    //QPixmap barcode_pixmap(":/image/barcode.png");
//    QPixmap barcode_pixmap("./images_about/barcode.png");
//    QPixmap barcode_scaled = barcode_pixmap.scaled(QSize(220, 220), Qt::KeepAspectRatio, Qt::SmoothTransformation);
//    barcode->setPixmap(barcode_scaled);
//    barcode->setFixedSize(barcode_scaled.size());

//    website->setText(QString("https://www.4es-usa.com"));

#ifdef USE_CASSETTE
device_name->setText(QString(tr("组织盒激光书写仪")));
#elif defined(USE_SLIDE) || defined(USE_SLIDE_DOUBLE)
device_name->setText(tr("载玻片激光书写仪"));
#endif

    application_version->setText(QString("%1 %2").arg(tr("应用软件版本："),"V1.0.3"));

    QString licensePath = QCoreApplication::applicationDirPath() + "/LICENSE.LGPLv3.txt";

    license_notice->setText(tr("本软件使用 Qt 框架，采用 GNU Lesser General Public License v3.0 授权") +
                            QString(" ( <a href=\"https://www.gnu.org/licenses/lgpl-3.0.html\" style=\"color:#bdbdbd;\">LGPL v3.0</a> | <a href=\"file:///%1\" style=\"color:#bdbdbd;\">LICENSE.LGPLv3</a> )").arg(licensePath));


    license_notice->setTextFormat(Qt::RichText);
    license_notice->setOpenExternalLinks(true);
}

void AboutDialog::setup_layout()
{
    this->bottomBar_hide();
    QVBoxLayout *about_panel_layout = new QVBoxLayout(about_panel);
    about_panel_layout->setSpacing(10);
    about_panel_layout->addStretch(0);
    about_panel_layout->addWidget(logo, 0, Qt::AlignHCenter);
    about_panel_layout->addSpacing(20);
    about_panel_layout->addWidget(barcode, 0, Qt::AlignHCenter);
    about_panel_layout->addWidget(website, 0, Qt::AlignHCenter);
    about_panel_layout->addSpacing(20);
    about_panel_layout->addWidget(device_name, 0, Qt::AlignHCenter);
    about_panel_layout->addSpacing(20);
    about_panel_layout->addWidget(application_version, 0, Qt::AlignHCenter);
    about_panel_layout->addWidget(firmware_version, 0, Qt::AlignHCenter);
    about_panel_layout->addStretch(0);
    about_panel_layout->addWidget(license_notice, 0, Qt::AlignHCenter);

    main_layout->addWidget(about_panel, 0, Qt::AlignHCenter);
}

void AboutDialog::set_stylesheet()
{
    about_panel->setStyleSheet(SS_Panels);
    device_name->setStyleSheet(SS_Title_Label);
    website->setStyleSheet(SS_Normal_Label);
    application_version->setStyleSheet(SS_Normal_Label);
    firmware_version->setStyleSheet(SS_Normal_Label);
    license_notice->setStyleSheet(SS_LicenseNotice_Label);
}

void AboutDialog::set_about_info()
{
    QSettings about_info_ini(ABOUT_INFO_INI, QSettings::IniFormat);
    about_info_ini.beginGroup(QString("logo_filename_png"));
    QString logo_filename_png = about_info_ini.value("name","").toString();
    int width_logo = about_info_ini.value("width",0).toInt();
    int height_logo = about_info_ini.value("height",0).toInt();
    bool visible_logo = about_info_ini.value("visible",0).toBool();
    about_info_ini.endGroup();

    about_info_ini.beginGroup(QString("barcode_filename_png"));
    QString barcode_filename_png = about_info_ini.value("name","").toString();
    int width_barcode = about_info_ini.value("width",0).toInt();
    int height_barcode = about_info_ini.value("height",0).toInt();
    bool visible_barcode = about_info_ini.value("visible",0).toBool();
    about_info_ini.endGroup();

    about_info_ini.beginGroup(QString("website"));
    QString websiteStr = about_info_ini.value("name","").toString();
    bool visible_website = about_info_ini.value("visible",0).toBool();
    about_info_ini.endGroup();

    QPixmap logo_pixmap(QString("./images_about/%1").arg(logo_filename_png));
    QPixmap logo_scaled = logo_pixmap.scaled(QSize(width_logo, height_logo), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    logo->setPixmap(logo_scaled);
    logo->setFixedSize(logo_scaled.size());
    logo->hide();

    QPixmap barcode_pixmap(QString("./images_about/%1").arg(barcode_filename_png));
    QPixmap barcode_scaled = barcode_pixmap.scaled(QSize(width_barcode, height_barcode), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    barcode->setPixmap(barcode_scaled);
    barcode->setFixedSize(barcode_scaled.size());
    barcode->hide();

    website->setText(QString("%1").arg(websiteStr));
    website->hide();

    if(visible_logo)
    {
        logo->show();
    }

    if(visible_barcode)
    {
        barcode->show();
    }

    if(visible_website)
    {
        website->show();
    }
}

void AboutDialog::connect_signals_and_slots()
{

}
