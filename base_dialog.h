#ifndef BASEDIALOG_H
#define BASEDIALOG_H

#include <QDialog>
#include <QEvent>
#include <QMouseEvent>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>
#include <QLabel>
#include <QString>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QDateTime>


class BaseDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BaseDialog(QDialog *parent = nullptr);

    void setTitle(QString Title);

    void dialog_show();
    void bottomBar_hide();
    void dialog_exit();
private:

    QWidget *title_bar;
    QLabel *title;
    QPushButton *back_btn;

    QScrollArea *main_scroll_area;

    QWidget *bottom_bar;

    void startAnimation();
    void backAnimation();

    void create_widget();
    void setup_layout();
    void set_stylesheet();

    QPoint dragPosition;
    bool isDraggable = false;

    QPushButton *datetime_btn;
    QTimer *timer;

signals:

protected slots:
    virtual void on_back_btn_clicked();
    void update_time();
protected:

    QWidget *main_area;
    QVBoxLayout *main_layout;
    QHBoxLayout *bottom_bar_layout;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;



};

#endif // BASEDIALOG_H
