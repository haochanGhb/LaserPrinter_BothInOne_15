#ifndef KEYBOARD_PANEL_H
#define KEYBOARD_PANEL_H

#include <QWidget>
#include <QPushButton>

class KeyboardPanel : public QWidget
{
    Q_OBJECT
public:
    explicit KeyboardPanel(QWidget *parent = nullptr);

private:

    void set_layout();
    void set_stylesheet();

signals:


private slots:


};

#endif // KEYBOARD_PANEL_H
