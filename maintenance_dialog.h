#ifndef MAINTENANCEDIALOG_H
#define MAINTENANCEDIALOG_H

#include "base_dialog.h"
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QListView>
#include <QColorDialog>
#include <QSettings>
#include <QDir>


#include "public_fun.h"
#include "dialogbox.h"
#include "print_task_data_management.h"

typedef struct{
    float x_offset_le;
    float y_offset_le;
    float x_width_le;
    float y_height_le;
}MarkAreaArgs;

typedef struct{
    QString color_name;
    QString color_code;
    int mark_level;
}ColorMarkLevelArgs;

class MaintenanceDialog : public BaseDialog
{
    Q_OBJECT
public:
    MaintenanceDialog();

    static bool MaintenanceDialog::ReadMarkAreaArgs(int index, MarkAreaArgs *args);
    static bool ReadColorMarkLevelArgs(const int colorNum, ColorMarkLevelArgs *args);

signals:
    void signale_print_task_exit_request();
private:
    QWidget *mark_area_panel;
    QWidget *color_level_panel;

    QLineEdit *x_offset_le;
    QLineEdit *y_offset_le;
    QLineEdit *x_width_le;
    QLineEdit *y_height_le;
    QLineEdit *tilt_angle_le;

    QComboBox* mark_param_sel_cbb;
    QList<QLineEdit*> color_name_les;
    QList<QPushButton*> color_btns;
    QList<QLabel*> color_code_lbs;
    QList<QComboBox*> color_print_level_cbbs;

    QPushButton *reset_btn;
    QPushButton *edit_btn;
    QPushButton *save_btn;
    QPushButton *cancel_btn;

    MarkAreaArgs edit_args[2];
    bool is_editing = false;

    void create_widget();
    void setup_layout();
    void set_stylesheet();
    void connect_signals_and_slots();

    void write_mark_area_args_to_file();
    void load_mark_area_args_from_file();

    void write_color_level_args_to_file();
    void load_color_level_args_from_file();
    void color_name_les_enable(bool isEnable);
    void update_ui_from_args(int index) ;

    QWidget *create_item(const QString &item_name, QWidget *widget_a=nullptr, QWidget *widget_b=nullptr, QWidget *widget_c=nullptr);
    QWidget *create_item(QWidget *widget_a, QWidget *widget_b=nullptr, QWidget *widget_c=nullptr, QWidget *widget_d=nullptr);

    QFrame *seprate_line();

signals:

protected:
    bool eventFilter(QObject *obj,QEvent *ev) override;


private slots:
    void on_color_btns_clicked();

    void on_edit_btn_clicked();
    void on_save_btn_clicked();
    void on_cancel_btn_clicked();

    void on_back_btn_clicked() override;

    void on_mark_param_changed();

    void on_reset_btn_clicked();
};

#endif // MAINTENANCEDIALOG_H
