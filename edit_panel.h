#ifndef EDIT_PANEL_H
#define EDIT_PANEL_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QMenu>
#include <QVBoxLayout>
#include <QDebug>
#include <QDateTime>
#include <QSettings>
#include <QString>
#include <QWidgetAction>
#include <QPainter>
#include <QShortcut>
#include <ColorPushButton.h>
#include <QPainter>
#include <QScrollBar>

#include "edit_panel_style.h"
#include "input_panel.h"
#include "template_dialog.h"
#include "tasklist_panel.h"
#include "SimpleKeyboard.h"
#include "public_fun.h"
#include "MarkPreview.h"
#include "BslModule.h"
#include "maintenance_dialog.h"

#define TIME_ALIAS_INDEX    15  //7

class EditPanel : public QWidget
{
    Q_OBJECT

public:

public:
    explicit EditPanel(QWidget *parent = nullptr);

    void setInputPanelName(const int panelId, const QString &panelName);
    void setInputPanelVisible(const int panelId, bool isVisible);
    void setTemplateSelector(const QStringList &tempNames);
    void setRepeatSelector(const int maxRepeatTimes);
    void setLaserInfoText(const QString &title, const QString &sta);
    void setCurrentTempAttr(TemplateAttr &temp_attr);
    void updateSlotColorMenuArgs();
    void updateMarkPreviewTextAlign();

    void setFirstInputPanalFocus();
    void setNextInputPanalFocus(bool isLoop);
    void setFirstInputBoxFocus();
    void setNextInputBoxFocus(bool isLoop = true);

    void setInputSeparateMode(int separateMode);                //扫描输入分割使能,1:分割符,2:预设长度，其它:不可用
    void setInputSeparateSymbol(const QString &seprateSymbol);  //扫描输入分割符
    void setInputPresetLengthStr(const QString &lengthStr);     //扫描输入预设长度

    void setTypeinConnectorSymbol(const QString &connectorSymbol);  //输入连续符
    void setTypeinDelimiterSymbol(const QString &dilimiterSymbol);  //输入分隔符

    void setPresetTextVisible(bool isVisible);                //使能下拉列表预设内容

    QString     GetCurrentTemplateName();
    int         GetRepeatTimes();
    QString     GetSelectSlots();
    int         GetSlotColorMarkLevel(const int slotNumber);

    qulonglong  GetTextListCount();
    QStringList GetTextList();
    bool  GetAlternatingPrint();

    void clearInputPanelsFocus();

    void load_text_to_markPreview(const QStringList & text_list);
    void load_image_to_markPreview(const QImage & image);
    QImage createMarkPicture(QList<QLabel*> list,const TemplateAttr  & temp_attr,const QStringList &bcList);
    void set_logo_to_markPreview(const TemplateAttr  & temp_attr);

//#ifdef USE_SLIDE
    BarcodeInfo makeMarkBarCodeInfo(const TemplateAttr  & temp_attr,const QStringList &bcList);

    QString read_last_selected_template_name();
//#endif

    bool isLastInputPanalFocus();

    void incFocusInputBox();
    void decFocusInputBox();

    void setMergeSymbol(const QString &mergeSymbol);

    void setInputPanelText(const int panelId, const QString &text);
    void setExpand_box_btn_enble(bool isEnable);
    void setRetract_box_btn_enble(bool isEnable);
    void scheduleRelayout();
    void updateInputPanelLayout();
protected:
    void changeEvent(QEvent *event) override;
private:

    QWidget *edit_area;
    QWidget *preview_part;
    QWidget *input_part;
    QWidget *keyboard_area;

    //preview_part content
    QWidget *preview_top_panel;
    QWidget *preview_bottom_panel;
    QFrame  *separation_line;
    QLabel      *laser_info;
    QPushButton *keyboard_btn;
    QPushButton *property_btn;

    //MarkPreview
    MarkPreview *m_pMarkPreview;

    //input_part content
    QWidget *input_top_panel;
    QWidget *input_mid_panel;
    QWidget *input_bottom_panel;

    QPushButton *toggle_expand_btn = nullptr;   // 折叠/展开按钮
    QTimer *m_relayoutTimer = nullptr;

    QScrollArea *input_scroll_area = nullptr;  // 新增
    QWidget *input_mid_container = nullptr;    // scrollArea 的内容容器
    QVBoxLayout *input_mid_panel_layout = nullptr;

    //void updateInputPanelLayout();    // 更新显示逻辑
    //void scheduleRelayout();

    //收料盒弹出、收起
    QPushButton *expand_box_btn;
    QPushButton *retract_box_btn;

    QWidget *template_part_panel;
    //模板
    QLabel      *template_title;
    QComboBox   *template_selector;
    QPushButton   *template_edit_btn;

    //重复次数
    QLabel      *repeat_title;
    QComboBox   *repeat_selector;
    QPushButton *create_btn;
    QPushButton *create_menu_btn;
    QMenu   *create_menu;
    QAction *create_task;
    QAction *create_task_and_print;
    QAction *create_task_and_print_instant;
    QAction *clear_all_input_box;
    QAction     *create_task_from_file;

    InputPanel      *input_panel[MAX_INPUT_PANEL];
    InputPanelAttr  input_panel_attr[MAX_INPUT_PANEL];
    TemplateAttr current_select_temp_attr;

    //Slots part content
    QLabel      *slot_title;
    QWidget     *slot_btn_container[MAX_SLOTS];
    QPushButton *slot_btn[MAX_SLOTS];
    ColorPushButton *slot_attr_btn[MAX_SLOTS];
    QMenu       *slot_attr_menu[MAX_SLOTS];
    QWidget     *print_order_button_panel;
    QButtonGroup *print_order_button_group;

    int         slot_attr_menu_selected_index;
    QString     m_textAlignment = "left";

    //keyboard
    SimpleKeyboard *m_pKeyboard;

    int     input_separate_mode;
    QString input_separate_symbol;
    int     input_preset_length[MAX_INPUT_PANEL] = {0};

    void create_widget();
    void create_widget_v2();
    void setup_layout();
    void setup_layout_v2();
    void set_stylesheet();
    void connect_signals_and_slots();

    void setup_create_menu();
    void setup_slots_menu_actions(ColorPushButton *menu_btn, QMenu *menu);
    void set_last_selected_slots();
    void set_last_selected_slot_attr_btn_color();

    //QString read_last_selected_template_name();
    QString read_last_selected_repeat_times();

    void save_last_selected_template();
    void save_last_selected_repeat();
    void save_last_selected_slots();

    void clear_all_input_panel_text();
    void load_template_attr_To_markPreview();
    void set_mark_preview_text(int inputId,const TemplateAttr  & temp_attr);

    void retranslateUi();

signals:
    void template_selector_Changed(QString currentText);
    void create_btn_Clicked();
    void template_edit_btn_Clicked();
    void create_task_action_Triggered();
    void create_task_and_print_action_Triggered();
    void create_task_and_print_instant_action_Triggered();
    void create_task_from_file_action_Triggered();
    void expand_box_btn_Clicked();
    void retract_box_btn_Clicked();
private slots:
    void on_create_btn_clicked();
    void on_create_menu_btn_clicked();
    //void on_keyboard_btn_clicked();
    //void on_attr_btn_clicked();
    void on_panel_prop_btn_clicked();
    void on_panel_prop_btn_saved();
    void on_input_text_changed(QString text);
    void on_template_selector_changed();
    void on_repeat_selector_changed();
    void on_slots_btn_clicked();
    void on_input_edit_focusIn();

    void slotOnSimpleKeyboard(Qt::Key val, QString text);
private:
    void on_template_selector_OnThread();
};

#endif // EDIT_PANEL_H
