#ifndef INPUT_PANEL_PROPERTIES_DIALOG_H
#define INPUT_PANEL_PROPERTIES_DIALOG_H

#include "base_dialog.h"
#include <QWidget>
#include <QRadioButton>
#include <QSettings>

#include "public_fun.h"

/********************************************************************
 *
 * 输入面板配置项
 *
 * ******************************************************************/
typedef struct {
    QString template_name;              //关联的模板名
    int     id;                         //ID
    QString name;                       //显示名称
    bool    ime_enable;                 //是否允许输入法
    bool    retain_enable;              //是否保持上次输入内容
    bool    pre_suf_visible;            //有无前后导输入框
    bool    two_input_box_enable;       //false:一个输入框、true：两个输入框
    int     plus_btn_type;              //0:无按钮, 1:"+1", 2:"+1 Auto"
    bool    auto_plus_btn_enable;       //是否自动加一
    bool    single_or_couple_visible;   //有无单双号模式(只有载玻片有)
    bool    couple_enable;              //false:单号模式、true:双号模式
}InputPanelAttr;

class InputPanelPropDialog : public BaseDialog
{
    Q_OBJECT
public:
    explicit InputPanelPropDialog(InputPanelAttr *attr, BaseDialog *parent = nullptr);
    void set_options(InputPanelAttr *attr);
private:

    InputPanelAttr *attr;

    QWidget *input_box_options;
    QWidget *pre_suf_options;
    QWidget *plus_btn_options;
    QWidget *retian_btn_options;
    QWidget *ime_btn_options;

    QRadioButton    *one_input_box;
    QLabel          *one_input_box_desc;
    QRadioButton    *two_input_boxes;
    QLabel          *two_input_boxes_desc;

    QRadioButton    *pre_suf_enable;
    QLabel          *pre_suf_enable_desc;
    QRadioButton    *pre_suf_disable;
    QLabel          *pre_suf_disable_desc;

    QRadioButton    *plus_btn;
    QLabel          *plus_btn_desc;
    QRadioButton    *no_plus_btn;
    QLabel          *no_plus_btn_desc;
    QRadioButton    *plus_auto_btn;
    QLabel          *plus_auto_btn_desc;

    QRadioButton    *retian_enable;
    QLabel          *retian_enable_desc;
    QRadioButton    *retian_disable;
    QLabel          *retian_disable_desc;

    QRadioButton    *ime_disable;
    QLabel          *ime_disable_desc;
    QRadioButton    *ime_enable;
    QLabel          *ime_enable_desc;

    QPushButton *save_btn;
    QPushButton *cancel_btn;


    void create_widget();
    void setup_layout();
    void set_stylesheet();

signals:
    void save_btn_Clicked();

private slots:
    void on_save_btn_clicked();
    void on_cancel_btn_clicked();


};

#endif // INPUT_PANEL_PROPERTIES_DIALOG_H
