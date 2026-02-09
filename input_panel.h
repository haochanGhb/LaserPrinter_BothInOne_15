#ifndef INPUT_PANEL_H
#define INPUT_PANEL_H

#include <QWidget>
#include <windows.h>
#include <QProcess>
#include <QFile>
#include <QThread>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QWidget>
#include <QEvent>
#include <QMenu>
#include <QList>
#include <QListWidget>
#include <QListWidgetItem>
#include <QWidgetAction>


#include "input_panel_style.h"
#include "public_fun.h"
#include "base_dialog.h"
#include "input_panel_prop_dialog.h"
#include "touch_keyboard_controller.h"
#include "TouchKeyboardManager.h"

#define PANEL_SIZE_W 435
#define PANEL_SIZE_H1 70
#define PANEL_SIZE_H2 45

typedef enum
{
    FIXED_ITEM_TYPE,
    HISTORY_ITEM_TYPE,
    CLEAR_ITEM_TYPE
}ITEM_TYPE;




class InputPanel : public QWidget
{
    Q_OBJECT

public:
    // 静态标志，标识是否由扫描枪触发的输入
    static inline bool s_isScannerInput = false;
    static void setIsScannerInput(bool isScanner);

private:
public:
    explicit InputPanel(int panelId, QWidget *parent = nullptr);
    ~InputPanel(); // 析构函数
    void refreshPropertyText();
    void setPanelId(int panelId);
    void setPanelName(QString panelName);
    void setPropBtnVisible(bool isVisible);
    void clearInputText();

    int getPanelId();
    qulonglong getTextCount();
    QStringList getText();

    QString getEditText();

    void setFocus();
    void clearFocus();
    bool isFocus();
    void setText(const QString &text);

    void setLeftBoxFocus();
    void setRightBoxFocus();
    bool isLeftBoxFocus();
    bool isRightBoxFocus();

    void setConnectorSymbol(const QString &connectorSymbol);     //输入连续符
    void setDelimiterSymbol(const QString &dilimiterSymbol);   //输入分隔符

    void incText();
    void decText();

    void setMergeSymbol(const QString &mergeSymbol);
    void setSingleCoupleButtonVisible(bool isVisible);
    void setPresetTextVisible(bool isVisible);

    void suppressTouchKeyboard();
    void allowTouchKeyboard();
    void hideTouchKeyboard();
signals:
    void signal_edit_focusIn();
    void property_btn_saved();
private:

    InputPanelAttr attr;

    QWidget *main_panel;
    QWidget *left_part;
    QWidget *left_top_container;
    QWidget *left_bottom_container;
    QWidget *right_part;
    QWidget *right_top_container;
    QWidget *right_bottom_container;

    QPushButton *panel_name;

    QAction *two_input_box_action;
    QAction *pre_suf_box_action;
    QAction *plus_btn_action;
    QAction *plus_auto_btn_action;
    QAction *ime_action;
    QAction *retain_action;

    QMenu   *property_menu;
    //void setup_property_menu();


    QPushButton *plus_btn;      //加一按钮
    QPushButton *auto_plus_btn; //自动加一按钮
    QPushButton *ime_btn;       //屏蔽输入法按钮
    QPushButton *retain_btn;    //保留输入框内容按钮
    QPushButton *prefix_btn;    //前导按钮
    QPushButton *suffix_btn;    //后导按钮

    QPushButton *single_or_couple_btn;//单双小号切换（只有载玻片的小号有这按钮）

    QPushButton *property_btn;     //属性按钮

    QLineEdit *prefix_line_edit;    //前导输入框
    QLineEdit *suffix_line_edit;    //后导输入框

    QLineEdit   *input_line_edit_left;      //双输入框时，增加左输入框
    QLabel      *input_line_hyphen;         //双输入框时，增加连接符
    QLineEdit   *input_line_edit_right;     //单输入框时，只有这个
    QPushButton *input_list_btn;    //历史输入下拉列表按钮
    QMenu       *input_list_menu;   //历史输入下拉列表
    QListWidget *input_list_widget;

//    QAction *clear_action;
    QListWidgetItem *clear_item;

    int fixed_items_cnt=0;
    QPixmap *fixed_icon;
    void edit_list_menu_init();
    void add_edit_list_item(const QString &text, ITEM_TYPE type);

    void load_preset_list_from_file();
    void clear_preset_list();

    void create_widget();
    void setup_layout();
    void set_stylesheet();
    void connect_singals_and_slots();

    void read_all_config();
    void write_all_config();

    void write_config(const QString &key, const QVariant &value);

    void setPanelAttribute();
    void load_history_list_from_file();
    void save_history_list_item_to_file(const QString &text);
    void clear_history_list_to_file();
    void setup_property_menu();
    void retranslate_ui();
signals:
    void input_text_Changed(QString content);
    void property_btn_Clicked();
    void signalOpenInputListMenu();
    void signalCloseInputListMenu();

private slots:

    void on_single_or_couple_btn_clicked();

    void on_ime_btn_clicked();
    void on_ime_btn_toggled(bool checked);
    void on_retain_btn_clicked();

    void on_prefix_btn_clicked();
    void on_suffix_btn_clicked();

    void on_input_text_changed();

    void on_plus_btn_clicked();
    void on_auto_plus_btn_clicked();

    void on_property_btn_clicked();

    void on_edit_list_btn_clicked();
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void changeEvent(QEvent *event) override;
private:
    QString connector_symbol;
    QString delimiter_symbol;
    QString merge_symbol;
    QTimer *m_menuTimer;

};

#endif // INPUT_PANEL_H
