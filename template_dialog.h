#ifndef TEMPLATEDIALOG_H
#define TEMPLATEDIALOG_H

#include "base_dialog.h"
#include <QWidget>
#include <QListWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QTabWidget>
#include <QSpinBox>
#include <QLineEdit>
#include <QSettings>
#include <QButtonGroup>
#include <QFontDatabase>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QDateTime>
#include <QScroller>
#include <QThread>
#include <TemplatePreview.h>
#include <QFileDialog>

#include "dialogbox.h"
#include "public_fun.h"
#include "setting_dialog.h"
#include "MarkHandle.h"
#include "touch_keyboard_controller.h"

/********************************************************************
 *
 * 模板对话框配置项
 *
 * ******************************************************************/

typedef struct{
    QString type = "-1";           //条码类型
    int     width ;          //条码宽
    int     height ;         //条码高
    int     width_factor = 16;   //条码宽系数
    int     height_factor = 16;   //条码高系数
    bool    fixed_size;     //固定大小
    int     fixed_width = 21;    //固定宽
    int     fixed_height = 21;   //固定高
    int     pos_x = 0;      //x坐标
    int     pos_y = 0;      //y坐标
    QString separator;      //间隔字符
    bool    separator_ischecked;     //间隔字符是否checked
    QString text_index;     //生成二维码的文本索引
    int     direct_val = 1;     //翻转方向
}CodeAttr;

typedef struct{
    QString name;
    QString input_text;
    int     min_width = 50;
    QString font = "Microsoft YaHei";
    int     font_size;
    int     pos_x = 21;          //x坐标
    int     pos_y = 21;          //y坐标
    QString sta;
}PanelAttr;

typedef struct{
    int     width ;              //徽标宽
    int     height ;             //徽标高
    int     width_factor = 1;    //徽标宽系数
    int     height_factor = 1;   //徽标高系数
    bool    fixed_size;          //固定大小
    int     fixed_width = 21;    //固定宽
    int     fixed_height = 21;   //固定高
    int     pos_x = 0;           //x坐标
    int     pos_y = 0;           //y坐标
    int     direct_val = 1;      //翻转方向
    QString file_name;          //文件名
    bool    is_display = false;
}LogoAttr;

typedef struct{
    QString     name;
    CodeAttr    code_attr;
    PanelAttr   panel_attr[MAX_INPUT_PANEL];
    LogoAttr    logo_attr;
}TemplateAttr;



enum Direct
{
    Up = 0,
    Down,
    Left,
    Right,
};

constexpr double LOGO_ASPECT_RATIO = double(PIXEL_VIEW_WIDTH) / double(PIXEL_VIEW_HEIGHT);

#define TIME_ALIAS_INDEX    15  //7

class TemplateDialog : public BaseDialog
{
    Q_OBJECT
public:
    explicit TemplateDialog(BaseDialog *parent = nullptr);
    ~TemplateDialog();
    static QStringList ReadTemplateNames(const QString &Dir);
    static bool ReadTemplateParam(const QString &Dir, const QString &tempName, TemplateAttr *tempAttr);
    void Exec();
    void setMarkHandle(MarkHandle *handle);
protected:
    void changeEvent(QEvent *event) override;
    bool eventFilter(QObject *obj,QEvent *ev) override;
private:
    bool m_enableTouchKeyboard; // 控制是否启用触摸键盘功能

    //主区域容器面板
    QWidget * upper_edit_area;
    QWidget * upper_btn_panel;
    QWidget * upper_preview_panel;
    QWidget * upper_preview_left_panel;
    QWidget * upper_preview_right_panel;

    QWidget * lower_content_area;
    QScrollArea *scrollArea;

    //模板参数内容区域标题
    QWidget * content_panel_title;
    //模板参数表格列标题
    QWidget * content_table_column_title;
    QLabel * column_title[7];

    //各区域面板
    //QWidget *temp_panel;
    //QWidget *code_type_panel;
    QWidget *code_param_panel;
    QWidget *preview_panel;
    //QWidget *content_panel;

    //temp_panel
    QLabel *temp_title;
    QComboBox   *template_selector;
    QPushButton *temp_create_btn;
    //QListWidget *temp_list;

    //code_type_panel
    QLabel *code_type_title;
    //QListWidget *code_type_list;
    QComboBox *code_type_selector;

    //code_param_panel
    QLabel *code_param_title;
    QLabel *code_param_pic;
    QCheckBox *code_fixed_size_checkbox;
    QLabel *code_width_label;
    QLabel *code_height_label;
    QSlider *code_width_slider;
    QLabel *code_width_slider_val;
    QSlider *code_height_slider;
    QLabel *code_height_slider_val;
    QSpinBox *code_width_spinbox;
    QSpinBox *code_height_spinbox;
    QLineEdit *code_seperator_lineedit;
    QCheckBox * code_seperator_checkbox;

    QLabel *code_size_title;
    QLabel *code_seperator_title;

    //logo_param
    QLabel *logo_setting_title;
    QLabel *logo_setting_display;
    QCheckBox * logo_setting_show_checkbox;

    QLabel *logo_width_label;
    QLabel *logo_height_label;
    QSlider *logo_width_slider;
    QLabel *logo_width_slider_val;
    QSlider *logo_height_slider;
    QLabel *logo_height_slider_val;

    //preview_panel

    //content_panel
//    QPushButton *text_tab_btn;
//    QPushButton *font_tab_btn;

    QPushButton *name_btn[MAX_INPUT_PANEL];
    QLineEdit *input_box[MAX_INPUT_PANEL];      //前7行为文本输入框
    QComboBox *time_format_selector;            //第8行为时间格式选择框
    QSpinBox *min_width[MAX_INPUT_PANEL];
    QComboBox *sta_selector[MAX_INPUT_PANEL];
    QComboBox *font_selector[MAX_INPUT_PANEL];
    QSpinBox *font_size[MAX_INPUT_PANEL];
    QCheckBox * barcode_checkbox[MAX_INPUT_PANEL];

    //底部按钮
    QPushButton *delete_btn;
    QPushButton *copy_btn;
    QPushButton *edit_btn;
    QPushButton *save_btn;
    QPushButton *cancel_btn;

    //模版预览
    TemplatePreview * m_pTemplatePreview;

    //TemplateAttr  selection_temp_attr;

    TemplateAttr  edit_temp_attr;
    TemplateAttr  * p_selection_temp_attr;

    bool m_bEditEnable = false;

    MarkHandle *m_pMarkHandle = nullptr;
    QString m_StrLastSelectTemp = "";
    QString m_StrNewlogofile = "";
    bool m_isNewTemplate;
    QString m_StrNewTempName = "";

    void create_widget();
    void setup_layout();
    void setup_layout_v2();
    void set_stylesheet();
    void connect_signals_and_slots();

    void write_template_param(const QString &Dir, const QString &fileName);

    void load_template_names(const QString &Dir);
    void load_template_param(const QString &Dir, const QString &tempName);

    void set_template_edit_enable(bool isEnable);
    void set_barcode_checkbox_enable(int barcodeType);
    void set_code_seperator_enable(int barcodeType);

    void set_template_preview_barcode(const TemplateAttr  & temp_attr);
    void set_template_preview_text(int inputId,const TemplateAttr  & temp_attr);
    void set_template_preview_datetime(const TemplateAttr  & temp_attr);
    void set_template_preview_logo(const TemplateAttr  & temp_attr);
    void set_slider_range();
    void set_template_preview_position(const TemplateAttr  & temp_attr);
    void get_template_preview_position(TemplateAttr  & temp_attr);
    void copy_editTemplate(const TemplateAttr* source, TemplateAttr& destination);

    QString get_barCode_textIndex();
    void retranslateUi();
    void translateDefaultTemp();
    void refreshTemplateSelector();
    void installWheelBlocker(QWidget *root);
signals:
    void signal_code_selector_changed();
    void signal_time_format_selector_changed();

private slots:
    void on_temp_create_btn_clicked();
    void on_temp_list_selection_changed();
    void on_template_selector_changed();
    void on_code_type_list_selection_changed();
    void on_code_selector_changed();

    void on_code_param_fixed_size_changed();

    void on_text_tab_btn_clicked();
    void on_font_tab_btn_clicked();
    void on_name_btn_clicked();
    void on_input_text_changed(QString text);
    void on_time_format_selector_changed();

    void on_input_sta_selector_changed();
    void on_time_sta_selector_changed();

    void on_font_selector_changed();
    void on_font_size_value_changed(int value);

    void on_delete_btn_clicked();
    void on_copy_btn_clicked();
    void on_edit_btn_clicked();
    void on_save_btn_clicked();
    void on_cancel_btn_clicked();

    void on_back_btn_clicked() override;
    void on_select_logofile_pressed();

    void slot_HighLightTextIndex(int index);
    void slot_HighLightTime();
    void slot_HighLightBarCode();
    void slot_code_width_slider_valueChanged(int value);
    void slot_code_height_slider_valueChanged(int value);
    void slot_code_edit_close();
    void slot_code_separator_changed();
    void slot_barcode_checked();

    void slot_min_width_spinbox_valueChanged(int value);

    void slot_logo_width_slider_valueChanged(int value);
    void slot_logo_height_slider_valueChanged(int value);
    void slot_logo_show_changed();

private:
    void code_selector_OnThread();
    void time_format_selector_OnThread();
    void template_selector_OnThread();

    void reload_logo_file(const QString logofile);
    bool saveLogoToTemplate(QString &logoFileName);
    void clearNameHighLight();
    void handleFocusHighlight(QObject *obj);
};

#endif // TEMPLATEDIALOG_H
