#ifndef SLOTGROUPS_DIALOG_H
#define SLOTGROUPS_DIALOG_H

#include <QObject>
#include <QComboBox>
#include <QListView>
#include <QMenu>
#include <QButtonGroup>
#include <ColorPushButton.h>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include "base_dialog.h"
#include "maintenance_dialog.h"

#include "public_fun.h"
#include "dialogbox.h"

struct SlotGroupsEditState {
    QJsonObject hopperColors;  // 存储hopper颜色索引 {"hopper_1": 0, "hopper_2": 1, ...}
    QJsonObject slotSelections; // 存储槽位选择状态 {"group_1": [1,0,1,0,0,1], "group_2": [0,1,0,1,1,0], ...}
    QJsonObject mappings;       // 存储映射符 {"group_1": "ABC", "group_2": "DEF", ...}

    // 转换为JSON字符串
    QString toJsonString() const {
        QJsonObject json;
        json["hopper_colors"] = hopperColors;
        json["slot_selections"] = slotSelections;
        json["mappings"] = mappings;
        return QJsonDocument(json).toJson(QJsonDocument::Compact);
    }

    // 从JSON字符串解析
    static SlotGroupsEditState fromJsonString(const QString &jsonStr) {
        SlotGroupsEditState state;
        QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
        if (doc.isObject())
        {
            QJsonObject json = doc.object();
            state.hopperColors = json["hopper_colors"].toObject();
            state.slotSelections = json["slot_selections"].toObject();
            state.mappings = json["mappings"].toObject();
        }
        return state;
    }
};

Q_DECLARE_METATYPE(SlotGroupsEditState)

class SlotGroups_Dialog : public BaseDialog
{
    Q_OBJECT
public:
    explicit SlotGroups_Dialog(BaseDialog *parent = nullptr);
    int Exec();
    //获取和设置编辑状态
    SlotGroupsEditState getEditState() const;
    void setEditState(const SlotGroupsEditState &state);
signals:
    void editStateChanged(const SlotGroupsEditState &state);
private slots:
    void on_edit_btn_clicked();
    void on_save_btn_clicked();
    void on_cancel_btn_clicked();
    void on_back_btn_clicked() override;
    void on_mapping_edit_finished(); // 新增：映射符编辑完成信号槽
private:
    bool is_editing = false;
    SlotGroupsEditState currentEditState; // 当前编辑状态
    SlotGroupsEditState originalEditState; // 原始状态（用于取消时恢复）

    QWidget *slot_groups_area_panel;

    QPushButton *edit_btn;
    QPushButton *save_btn;
    QPushButton *cancel_btn;

    //QComboBox* mark_param_sel_cbb;
    QButtonGroup *slot_groups_btn_group[MAX_SLOTS];
    QPushButton *slot_buttons[MAX_SLOTS][BUTTONS_PER_GROUP];
    QLabel *group_titles[MAX_SLOTS];
    QLabel *mapping_labels[MAX_SLOTS];
    QLineEdit *mapping_edits[MAX_SLOTS];

    //选槽颜色
    QLabel *slot_attr_title;
    QButtonGroup *slot_attr_btn_group;
    ColorPushButton *slot_attr_buttons[BUTTONS_PER_GROUP];
    QMenu  *slot_attr_menu[BUTTONS_PER_GROUP];

    void create_widget();
    void setup_layout();
    void set_stylesheet();
    void connect_signals_and_slots();

    // 数据操作函数
    void loadEditStateFromUI();
    void applyEditStateToUI();

    QWidget *create_item(const QString &item_name, QWidget *widget_a=nullptr, QWidget *widget_b=nullptr, QWidget *widget_c=nullptr);
    QWidget *create_item(QWidget *widget_a, QWidget *widget_b=nullptr, QWidget *widget_c=nullptr, QWidget *widget_d=nullptr);

    QFrame *seprate_line();
    void setup_slots_menu_actions(ColorPushButton *menu_btn, QMenu *menu);
    void set_last_selected_slot_attr_btn_color();

    bool validateMappingUniqueness(); // 校验映射符唯一性
    void highlightDuplicateMappings(); // 高亮重复的映射符
    void clearMappingHighlights(); // 清除高亮显示
};

#endif // SLOTGROUPS_DIALOG_H
