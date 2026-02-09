QT       += core gui network sql charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TRANSLATIONS += \
Translate_EN.ts \
Translate_CN.ts

CONFIG += c++11
CONFIG += c++17

DEFINES += NOMINMAX

#编译选项  组织盒 OR 载玻片
#组织盒:toggle_Cassette
#载玻片:toggle_Slide
#CONFIG += toggle_Cassette
CONFIG += toggle_Slide

msvc {
QMAKE_CFLAGS += /utf-8
QMAKE_CXXFLAGS += /utf-8
}


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ColorPushButton.cpp \
    DeviceDiscovery.cpp \
    DeviceDiscoveryDialog.cpp \
    ExcelReader.cpp \
    MachineCode/MachineCode.cpp \
    ScannerCapture.cpp \
    TouchKeyboardManager.cpp \
    about_dialog.cpp \
    base_dialog.cpp \
    codec_select_dialog.cpp \
    dialogbox.cpp \
    edit_panel.cpp \
    hardwareidprovider.cpp \
    history_dialog.cpp \
    history_table_model.cpp \
    input_panel.cpp \
    input_panel_prop_dialog.cpp \
    keyboard_panel.cpp \
    main.cpp \
    main_window.cpp \
    maintenance_dialog.cpp \
    mark_controller.cpp \
    print_task_data_management.cpp \
    print_state_machine.cpp \
    public_fun.cpp \
    setting_dialog.cpp \
    slotgroups_dialog.cpp \
    status_bar.cpp \
    status_item_delegate.cpp \
    task_scheduler.cpp \
    task_table_model.cpp \
    tasklist_panel.cpp \
    template_dialog.cpp \
    title_bar.cpp \
    waitframemanager.cpp

HEADERS += \
    ColorPushButton.h \
    DeviceDiscovery.h \
    DeviceDiscoveryDialog.h \
    ExcelReader.h \
    MachineCode/MachineCode.h \
    MachineCode/MachineCode.h \
    ScannerCapture.h \
    TouchKeyboardManager.h \
    about_dialog.h \
    base_dialog.h \
    base_dialog_style.h \
    codec_select_dialog.h \
    dialogbox.h \
    dialogbox_style.h \
    edit_panel.h \
    edit_panel_style.h \
    hardwareidprovider.h \
    history_dialog.h \
    history_table_model.h \
    input_panel.h \
    input_panel_prop_dialog.h \
    input_panel_style.h \
    keyboard_panel.h \
    main_window.h \
    maintenance_dialog.h \
    mark_controller.h \
    menu_dialog_style.h \
    print_task_data_management.h \
    print_state_machine.h \
    public_fun.h \
    setting_dialog.h \
    shared_enums.h \
    slotgroups_dialog.h \
    status_bar.h \
    status_item_delegate.h \
    stylesheet.h \
    task_scheduler.h \
    task_table_model.h \
    tasklist_panel.h \
    template_dialog.h \
    title_bar.h \
    touch_keyboard_controller.h \
    waitframemanager.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

include($$PWD/SimpleKeyboard/SimpleKeyboard.pri)
include($$PWD/QBarcode/QBarcode.pri)
include($$PWD/RemoteProl/RemoteProl.pri)
include($$PWD/RemoteRecv/RemoteRecv.pri)
include($$PWD/RemoteSend/RemoteSend.pri)
include($$PWD/QSimplePicCrop/QSimplePicCrop.pri)

# QXlsx code for Application Qt project
QXLSX_PARENTPATH=$$PWD/QXlsx/         # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=$$PWD/QXlsx/header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=$$PWD/QXlsx/source/  # current QXlsx source path is ./source/
include($$PWD/QXlsx/QXlsx.pri)

contains(CONFIG, toggle_Cassette) {
    message("Using Interface_Cassette for Cassette App")
    include($$PWD/MarkPreview_Cassette/MarkPreview.pri)
    include($$PWD/TemplatePreview_Cassette/TemplatePreview.pri)
    include($$PWD/MarkProcess_Cassette/MarkProcess.pri)

    DISTFILES += \
    Cassette_logo.rc
    RC_FILE += Cassette_logo.rc

}

contains(CONFIG, toggle_Slide) {
    message("Using Interface_Slide for Slide App")
    include($$PWD/MarkPreview_Slide/MarkPreview.pri)
    include($$PWD/TemplatePreview_Slide/TemplatePreview.pri)
    include($$PWD/MarkProcess_Slide/MarkProcess.pri)

    DISTFILES += \
    Slide_logo.rc
    RC_FILE += Slide_logo.rc
}


RESOURCES += \
    image.qrc

contains(CONFIG, toggle_Cassette):DEFINES += USE_CASSETTE

contains(CONFIG, toggle_Slide):DEFINES += USE_SLIDE_DOUBLE #单槽载玻片:USE_SLIDE, 双槽载玻片:USE_SLIDE_DOUBLE

INCLUDEPATH += "D:/OpenSSL-Win64/include"

LIBS += -L"D:/OpenSSL-Win64/lib" -llibssl -llibcrypto

LIBS += -lole32

FORMS +=



