#include "history_dialog.h"

void calculateDateRangeForWeek(QDateTime &sel_startTime, QDateTime &sel_endTime) {
    // 获取当前日期
    QDate currentDate = QDate::currentDate();

    // 计算周一和周日的日期
    int daysToMonday = currentDate.dayOfWeek() - 1; // dayOfWeek: Monday=1, Sunday=7
    QDate startOfWeek = currentDate.addDays(-daysToMonday);
    QDate endOfWeek = startOfWeek.addDays(6);

    // 设置开始和结束时间
    sel_startTime = QDateTime(startOfWeek.startOfDay());
    sel_endTime = QDateTime(endOfWeek.endOfDay());
}


void calculateDateRangeForMonth(QDateTime &sel_startTime, QDateTime &sel_endTime) {
    // 获取当前日期
    QDate currentDate = QDate::currentDate();

    // 计算当前月份的第一天和最后一天
    QDate startOfMonth(currentDate.year(), currentDate.month(), 1);
    QDate endOfMonth = startOfMonth.addMonths(1).addDays(-1);

    // 设置开始和结束时间
    sel_startTime = QDateTime(startOfMonth.startOfDay());
    sel_endTime = QDateTime(endOfMonth.endOfDay());
}

class CustomChartView : public QChartView
{
public:
    explicit CustomChartView(QChart *chart, QWidget *parent = nullptr)
        : QChartView(chart, parent)
    {
        setRubberBand(QChartView::NoRubberBand); // 禁用缩放橡皮筋
        setDragMode(QGraphicsView::ScrollHandDrag); // 启用拖动模式
        //setDragMode(QGraphicsView::NoDrag);     // 禁用默认拖动模式
        //chart->setAnimationOptions(QChart::NoAnimation);
    }

protected:
    void mouseMoveEvent(QMouseEvent *event) override
    {
        if (m_isPress)
         {
             QPointF delta = m_lastMousePos - event->pos();
             chart()->scroll(delta.x(), 0); // 水平拖动
             m_lastMousePos = event->pos();
         }
         QChartView::mouseMoveEvent(event);
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        // 启用拖动模式
        if (event->button() == Qt::LeftButton)
        {
            setCursor(Qt::ClosedHandCursor); // 设置手型光标
            m_lastMousePos = event->pos();
            m_isPress = true;
        }

        QChartView::mousePressEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
        // 停止拖动
        if (event->button() == Qt::LeftButton)
        {
            setCursor(Qt::ArrowCursor); // 恢复默认光标
            m_isPress = false;
            m_lastMousePos = QPoint();
        }
        QChartView::mouseReleaseEvent(event);
    }

    void wheelEvent(QWheelEvent *event) override
    {
        // 禁用滚轮缩放
        event->ignore();
    }

private:
    QPoint m_lastMousePos;
    bool m_isPress = false;
};

HistoryDialog::HistoryDialog(BaseDialog *parent) : BaseDialog(parent)
{
    setTitle(tr("历史"));
    set_layout();
    set_style_sheet();
    connect_signals_and_slots();
}

void HistoryDialog::Exec()
{
    try
    {
        //默认选择自定义,重置
        //search_operator_button->button(2)->click();
        //slot_search_operator_buttonClicked(0);
        show_history_search_panel(true);
        retranslateUi();

        load_history_data_timer = new QTimer(this);
        connect(load_history_data_timer, &QTimer::timeout, this, [=]
        {
            load_history_data_timer->stop();
            //slot_statistics_operator_buttonClicked(0);
            QAbstractButton* button = statistics_quick_time_button->button(0);
            button->click();
        });

        load_history_data_timer->start(100);
        exec();
    }
    catch (const std::exception& e)
    {
         Logger::instance().log("Error in HistoryDialog::Exec():", e.what());
    }

}

void HistoryDialog::changeEvent(QEvent *event)
{
    if (NULL != event)
    {
        switch (event->type())
        {
        // this event is send if a translator is loaded
        case QEvent::LanguageChange:
            qDebug() << "case QEvent::LanguageChange" << endl;
            retranslateUi();
            break;
        default:
            break;
        }
    }
}


/*
void HistoryDialog::load_HistoryTask_List(const QList<int> &taskTypes, const QDateTime &startTime, const QDateTime &endTime)
{
    QSqlQuery query = print_task_manager.queryPrintTasks(taskTypes, startTime, endTime);

    // 清空表格内容
    history_table->clearContents();
    history_table->setRowCount(0);

    // 设置表头
    QSqlRecord record = query.record();
    int columnCount = record.count();
    //history_table->setColumnCount(columnCount);

    int tableCol = ColNum::COL_INDEX;
    // 填充数据
    int row = 0;
    QString task_text ;
    while (query.next())
    {
        history_table->insertRow(row);
        for (int col = 0; col < columnCount; ++col) {
            QTableWidgetItem* item = new QTableWidgetItem();
            // 对特定列进行特殊格式化
            if (col == 0)
            {//隐藏 任务名
                item->setText(query.value(col).toString());
                item->setData(Qt::UserRole,query.value(col).toString());
                tableCol = ColNum::COL_NAME;
            }
            else if (col == 1)
            { //打印文本信息
                task_text = query.value(col).toString();
                QString print_text = make_task_display_text(task_text);
                item->setText(print_text);
                //tableCol = ColNum::COL_INFO;
                tableCol = ColNum::COL_CONTENT;
            }
            else if (col == 2)
            {  // 任务类型
                int taskType = query.value(col).toInt();
                QString taskInfo = make_task_type_info(task_text);
                item->setText(taskInfo);
                //tableCol = ColNum::COL_STA;
                tableCol = ColNum::COL_INFO;
            }
            else if (col == 3)
            {//全选
                item->setText(QString::number(row + 1));
                tableCol = ColNum::COL_INDEX;
            }
            else if (col == 4)
            {  // 打印时间
                QDateTime printTime = QDateTime::fromString(query.value(col).toString(), Qt::ISODate);
                item->setText(printTime.toString("yyyy-MM-dd HH:mm:ss"));
                tableCol = ColNum::COL_DATETIME;
                //tableCol = ColNum::COL_CONTENT;

            }
            item->setTextAlignment(Qt::AlignCenter);
            history_table->setItem(row, tableCol, item);
        }
        ++row;
    }

    // 调整列宽
    //history_table->resizeColumnsToContents();
}
*/

void HistoryDialog::load_HistoryTask_List(const QList<int> &taskTypes,
                                          const QDateTime &startTime,
                                          const QDateTime &endTime)
{
    history_model->clearAllTasks();

//    QProgressDialog progress_dialog(tr("正在加载历史记录，请稍候..."), QString(), 0, 0, nullptr);
//    progress_dialog.setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
//    progress_dialog.setFixedSize(300,100);
//    progress_dialog.setStyleSheet(SS_ProgressDialog);
//    progress_dialog.setWindowModality(Qt::ApplicationModal);
//    progress_dialog.show();

    on_row_state_changed();
    QSqlQuery query = print_task_manager.queryPrintTasks(taskTypes, startTime, endTime);

    if (!query.isActive())
    {
        //progress_dialog.close();
        return;
    }

    // 暂时断开信号
    disconnect(history_table->model(), &QAbstractItemModel::rowsInserted, this, &HistoryDialog::on_row_state_changed);

    QSqlRecord record = query.record();
    int columnCount = record.count();
    int row = 0;
    QString task_text;

    while (query.next()) {
        HistoryData d;
        // 默认清空
        d.guid.clear();
        d.name.clear();
        d.sta.clear();
        d.info.clear();
        d.content.clear();
        d.datetime.clear();
        d.fullText.clear();

        for (int col = 0; col < columnCount; ++col) {
            // 下面的映射严格按你原先实现的列意义来写
            if (col == 0) {
                // 原先第0字段是 GUID —— 放到 d.guid，并在模型里通过 UserRole 提供
                d.guid = query.value(col).toString();
            }
            else if (col == 1) {
                // 原先第1字段是完整 task_text -> 我们把显示内容放到 COL_CONTENT
                task_text = query.value(col).toString();
                d.fullText = task_text;
                d.content = make_task_display_text(task_text);
            }
            else if (col == 2) {
                // 原先第2字段是任务类型编号 -> 转换为 info 文本 (COL_INFO)
                int taskType = query.value(col).toInt();
                d.info = make_task_type_info(task_text);
            }
            else if (col == 3) {
                // 如果原表第3列不是需要的字段，可忽略或根据原逻辑映射
                // 你原先把序号放在 COL_INDEX，这里不需要赋值，因为序号由 model 返回
            }
            else if (col == 4) {
                // 时间字段 -> 格式化并放到 COL_DATETIME
                QDateTime t = QDateTime::fromString(query.value(col).toString(), Qt::ISODate);
                d.datetime = t.toString("yyyy-MM-dd HH:mm:ss");
            }
            // 若表有更多列，可继续 mapping
        }

        history_model->appendHistoryRow(d);
//        ++row;
//        if(row % 100 == 0)
//        {
//            QCoreApplication::processEvents();
//        }
    }
    //progress_dialog.close();

    connect(history_table->model(), &QAbstractItemModel::rowsInserted, this, &HistoryDialog::on_row_state_changed);
    //重新刷新数字状态
    on_row_state_changed();
    //滚动到最top
    QModelIndex top = history_model->index(0, 0);
    history_table->scrollTo(top, QAbstractItemView::PositionAtTop);
}

void HistoryDialog::connect_signals_and_slots()
{
    connect(search_quick_time_button, QOverload<int>::of(&QButtonGroup::buttonClicked), this,&HistoryDialog::slot_search_quick_time_buttonClicked);

    connect(search_operator_button, QOverload<int>::of(&QButtonGroup::buttonClicked), this,&HistoryDialog::slot_search_operator_buttonClicked);

    connect(datetimeEdit_Begin, &QDateTimeEdit::dateTimeChanged, this, &HistoryDialog::slot_search_datetime_Changed);
    connect(datetimeEdit_End, &QDateTimeEdit::dateTimeChanged, this, &HistoryDialog::slot_search_datetime_Changed);

    //搜索
    //connect(search_btn,&QPushButton::clicked, this,&HistoryDialog::slot_search_buttonClicked);
    connect(search_btn, &QPushButton::clicked, this, [=]() {
        if (!GlobalClickGuard::isAllowedClick(search_btn, 5000)) {
            return;
        }

        search_btn->setEnabled(false);
        QTimer::singleShot(5000, search_btn, [=]() {
            search_btn->setEnabled(true);
        });

        slot_search_buttonClicked();
        });

    //导出
    connect(export_btn, &QPushButton::clicked, this,&HistoryDialog::on_export_btn_clicked);
    //删除
    connect(delete_btn, &QPushButton::clicked, this,&HistoryDialog::on_delete_btn_clicked);

    connect(statistics_quick_time_button, QOverload<int>::of(&QButtonGroup::buttonClicked), this,&HistoryDialog::slot_statistics_operator_buttonClicked);

//    connect(statistics_quick_time_button, QOverload<int>::of(&QButtonGroup::buttonClicked), this, [=](int id) {
//        if (!GlobalClickGuard::isAllowedClick(statistics_quick_time_button, 2000)) {
//            return;
//        }

//        // 禁用按钮组中的所有按钮
//        for (QAbstractButton* button : statistics_quick_time_button->buttons()) {
//            button->setEnabled(false);
//        }

//        // 3秒后启用所有按钮
//        QTimer::singleShot(2000, this, [=]() {
//            for (QAbstractButton* button : statistics_quick_time_button->buttons()) {
//                button->setEnabled(true);
//            }
//        });

//        slot_statistics_operator_buttonClicked(id);
//        });

    connect(history_table->model(), &QAbstractItemModel::rowsInserted, this, &HistoryDialog::on_row_state_changed);
    connect(history_table->model(), &QAbstractItemModel::rowsRemoved, this, &HistoryDialog::on_row_state_changed);
    connect(history_table->selectionModel(), &QItemSelectionModel::selectionChanged, this, &HistoryDialog::on_row_state_changed);
    connect(history_table->horizontalHeader(), &QHeaderView::sectionClicked, this, &HistoryDialog::on_select_all_item_clicked);

    SettingDialog::ReadSettings(&settings);
}

void HistoryDialog::set_layout()
{
    this->bottomBar_hide();
    //历史结果列表区域
    history_table_area = new QWidget(this);

    //搜索条件设置区域=================================================================BEGIN
    search_settings_area = new QWidget(this);
    //search_settings_area->setFixedHeight(200);

    //搜索条件设置-左
    search_settings_area_left = new QWidget();
    //搜索条件设置-右
    search_settings_area_right = new QWidget();
    search_settings_area_right->setFixedWidth(150);

    QHBoxLayout *search_settings_area_layout = new QHBoxLayout(search_settings_area);
    search_settings_area_layout->setContentsMargins(5,5,5,5);
    search_settings_area_layout->setSpacing(3);
    search_settings_area_layout->addWidget(search_settings_area_left);
    search_settings_area_layout->addWidget(search_settings_area_right);

    //搜索条件设置-左-设置按钮面板1、2、3
    search_settings_left_panel1  = new QWidget();
    search_settings_left_panel2  = new QWidget();
    search_settings_left_panel3  = new QWidget();

    QVBoxLayout *search_settings_area_left_layout = new QVBoxLayout(search_settings_area_left);
    search_settings_area_left_layout->setContentsMargins(3,3,3,3);
    search_settings_area_left_layout->setSpacing(2);
    search_settings_area_left_layout->addWidget(search_settings_left_panel1);
    search_settings_area_left_layout->addWidget(search_settings_left_panel2);
    search_settings_area_left_layout->addWidget(search_settings_left_panel3);

    //搜索类型
    search_type_Label = new QLabel();
    search_type_Label->setText(tr("类型"));
    search_type_checkbox1 = new QCheckBox();
    search_type_checkbox1->setText(tr("本地任务"));

    search_type_checkbox2 = new QCheckBox();
    search_type_checkbox2->setText(tr("远程文本"));

    search_type_checkbox3 = new QCheckBox();
    search_type_checkbox3->setText(tr("远程图片"));

    QHBoxLayout *search_settings_left_panel1_layout = new QHBoxLayout(search_settings_left_panel1);
    search_settings_left_panel1_layout->setContentsMargins(5,5,5,5);
    search_settings_left_panel1_layout->setSpacing(5);
    search_settings_left_panel1_layout->addWidget(search_type_Label);
    search_settings_left_panel1_layout->addWidget(search_type_checkbox1);
    search_settings_left_panel1_layout->addWidget(search_type_checkbox2);
    search_settings_left_panel1_layout->addWidget(search_type_checkbox3);

    //快速按钮标题
    search_quick_time_Label = new QLabel();
    search_quick_time_Label->setText(tr("时间"));

    //快速按钮组
    search_quick_time_button = new QButtonGroup();
    //按钮组水平布局
    QHBoxLayout *search_settings_left_panel2_layout = new QHBoxLayout(search_settings_left_panel2);
    search_settings_left_panel2_layout->setContentsMargins(5,2,5,2);
    search_settings_left_panel2_layout->setSpacing(0);
    search_settings_left_panel2_layout->addWidget(search_quick_time_Label);

    QStringList buttonTexts = {tr("今天"), tr("昨天"), tr("本周"), tr("本月"), tr("自定义")};
    int id = 0;
    for (const QString &text : buttonTexts) {
        QPushButton *button = new QPushButton(text);
        button->setFixedHeight(40);
        button->setStyleSheet(SS_quick_datetime_btn);
        search_settings_left_panel2_layout->addWidget(button);
        search_quick_time_button->addButton(button,id);
        button->setCheckable(true);
        id++;
    }

    //时间自定义选择
    datetimeEdit_Begin = new MyDateTimeEdit;
    datetimeEdit_Begin->setFixedWidth(150);
    datetimeEdit_Begin->setDisplayFormat("yyyy-MM-dd");
    datetimeEdit_Begin->setDate(QDate::currentDate());
    datetimeEdit_Begin->setCalendarPopup(true);
    CustomCalendarWidget *calendar_begin = new CustomCalendarWidget();
    datetimeEdit_Begin->setCalendarWidget(calendar_begin);

    hyphen_Label = new QLabel;
    hyphen_Label->setText(" - ");
    hyphen_Label->setAlignment(Qt::AlignVCenter);

    datetimeEdit_End = new MyDateTimeEdit;
    datetimeEdit_End->setFixedWidth(150);
    datetimeEdit_End->setDisplayFormat("yyyy-MM-dd");
    datetimeEdit_End->setDate(QDate::currentDate());
    datetimeEdit_End->setCalendarPopup(true);
    CustomCalendarWidget *calendar_end = new CustomCalendarWidget();
    datetimeEdit_End->setCalendarWidget(calendar_end);

    QHBoxLayout *search_settings_left_panel3_layout = new QHBoxLayout(search_settings_left_panel3);
    search_settings_left_panel3_layout->setContentsMargins(5,2,5,2);
    search_settings_left_panel3_layout->setSpacing(0);
    search_settings_left_panel3_layout->addStretch();
    //search_settings_left_panel3_layout->addWidget(datetimeEdit_Begin);
    //search_settings_left_panel3_layout->addWidget(hyphen_Label);
    //search_settings_left_panel3_layout->addWidget(datetimeEdit_End);

    //右边操作按钮组
    search_operator_button = new QButtonGroup();
    //按钮组垂直布局
    QVBoxLayout *search_settings_area_right_layout = new QVBoxLayout(search_settings_area_right);
    search_settings_area_right_layout->setContentsMargins(5,0,5,0);
    search_settings_area_right_layout->setSpacing(0);
    //search_settings_area_right_layout->addWidget(search_operator_button);

    buttonTexts.clear();
    buttonTexts << tr("统计") << tr("搜索") << tr("重置");
    id = 0;
    for (const QString &text : buttonTexts) {
        QPushButton *button = new QPushButton(text);
        button->setFixedHeight(58);
        if(button->text().compare(tr("搜索"))==0)
        {
            button->setStyleSheet(SS_Primary_Btn);
        }
        else
        {
            button->setStyleSheet(SS_Normal_Btn);
        }

        if(button->text().compare(tr("统计"))==0)
        {
            button->setObjectName("unchecked");
        }

        search_settings_area_right_layout->addWidget(button);
        search_operator_button->addButton(button,id);
        id++;
    }
    //搜索条件设置区域=================================================================END

    //历史列表区域
    history_title_panel = new QWidget();
    history_title_panel->setFixedHeight(50);

    history_datetime_panel = new QWidget();
    history_datetime_panel->setFixedHeight(50);

    history_title = new QLabel();
    history_title->setFixedSize(160,50);
    history_title->setText(tr("历史内容"));

    export_btn = new QPushButton();
    export_btn->setFixedSize(130, 40);
    export_btn->setText(tr("导出"));

    delete_btn = new QPushButton();
    delete_btn->setFixedSize(130, 40);
    delete_btn->setText(tr("删除"));


    history_record_label = new QLabel();
    history_record_label->setFixedSize(300, 50);
    history_record_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    history_record_label->setText(tr("已选：0 | 全部：0"));

    QHBoxLayout *history_title_layout = new QHBoxLayout(history_title_panel);
    history_title_layout->setContentsMargins(10,5,10,5);
    history_title_layout->setSpacing(5);

    history_title_layout->addWidget(history_title);
//    history_title_layout->addWidget(export_btn);
//    history_title_layout->addWidget(delete_btn);
    history_title_layout->addStretch();
    history_title_layout->addWidget(history_record_label);


    search_btn = new QPushButton();
    search_btn->setFixedSize(130, 40);
    search_btn->setCheckable(true);
    search_btn->setText(tr("搜索"));
    //搜索时间区域布局
    QHBoxLayout *history_datetime_layout = new QHBoxLayout(history_datetime_panel);
    history_datetime_layout->setContentsMargins(10,5,10,5);
    history_datetime_layout->setSpacing(5);
    history_datetime_layout->addWidget(datetimeEdit_Begin);
    history_datetime_layout->addWidget(hyphen_Label);
    history_datetime_layout->addWidget(datetimeEdit_End);
    history_datetime_layout->addStretch();
    history_datetime_layout->addWidget(search_btn);
    history_datetime_layout->addWidget(export_btn);
    history_datetime_layout->addWidget(delete_btn);


    history_table = new QTableView(this);
    history_model = new HistoryTableModel(this);
    history_table->setModel(history_model);

    history_table->horizontalHeader()->setStretchLastSection(true);
    history_table->setSelectionBehavior(QTableWidget::SelectRows);//选整行
    history_table->setSelectionMode(QAbstractItemView::ExtendedSelection);

//    history_table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);//固定行高
    history_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);//固定列宽
    history_table->setColumnWidth(ColNum::COL_INDEX, 70);
    //history_table->setColumnWidth(ColNum::COL_NAME, 0);
    //history_table->setColumnWidth(ColNum::COL_STA, 0);
    history_table->setColumnWidth(ColNum::COL_INFO, 200);
    history_table->setColumnWidth(ColNum::COL_CONTENT, 230);
    history_table->setColumnWidth(ColNum::COL_DATETIME, 100);
    history_table->setColumnHidden(ColNum::COL_NAME, true);     //HIDE COL_NAME
    history_table->setColumnHidden(ColNum::COL_STA, true);      //HIDE COL_NAME
    history_table->verticalHeader()->setDefaultSectionSize(65); //表格高度
    history_table->horizontalHeader()->setFixedHeight(35);      //表头高度
    history_table->verticalHeader()->setVisible(false);         //隐藏垂直表头
    history_table->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    history_table->setFocusPolicy(Qt::NoFocus);
    history_table->setWordWrap(false);

    history_table_panel = new QWidget();

    QHBoxLayout *history_table_panel_layout = new QHBoxLayout(history_table_panel);
    history_table_panel_layout->setContentsMargins(10,0,10,0);
    history_table_panel_layout->setSpacing(0);
    history_table_panel_layout->addWidget(history_table);

    //统计直方图区域==============================================================================BEGIN
    history_statistics_panel = new QWidget();
    history_statistics_panel->setFixedHeight(500);
    //统计区域面板-上部分
    history_statistics_upper_panel  = new QWidget();
    history_statistics_upper_panel->setFixedHeight(100);

    //统计标题+快速按钮
    statistics_title = new QLabel();
    statistics_title->setText(tr("打印统计"));

    statistics_sum_info = new QLabel();

    //统计按钮组
    statistics_quick_time_button = new QButtonGroup();
    //按钮组水平布局
    QHBoxLayout *statistics_upper_panel_layout = new QHBoxLayout(history_statistics_upper_panel);
    statistics_upper_panel_layout->setContentsMargins(5,2,5,2);
    statistics_upper_panel_layout->setSpacing(0);
    statistics_upper_panel_layout->addWidget(statistics_title);
    statistics_upper_panel_layout->addWidget(statistics_sum_info);
    statistics_upper_panel_layout->addStretch();

    QStringList statButtonTexts = {tr("日"), tr("周"),tr("月"), tr("年")};
    int idx = 0;
    for (const QString &text : statButtonTexts) {
        QPushButton *button = new QPushButton(text);
        button->setFixedSize(120,45);
        button->setStyleSheet(SS_quick_datetime_btn);
        statistics_upper_panel_layout->addWidget(button);
        statistics_quick_time_button->addButton(button,idx);
        button->setCheckable(true);
        idx++;

        if(text.compare(tr("周"))==0)
        {
            button->hide();
        }
    }


    //统计区域面板-下部分
    history_statistics_lower_panel  = new QWidget();

    //统计区域布局
    QVBoxLayout *history_statistics_panel_layout = new QVBoxLayout(history_statistics_panel);
    history_statistics_panel_layout->setContentsMargins(10,5,10,5);
    history_statistics_panel_layout->setSpacing(2);
    history_statistics_panel_layout->addWidget(history_statistics_upper_panel);
    history_statistics_panel_layout->addWidget(seprate_line());
    history_statistics_panel_layout->addWidget(history_statistics_lower_panel);
    //统计直方图区域==============================================================================END


    //查询列表和统计区域布局
    QVBoxLayout *history_table_area_layout = new QVBoxLayout(history_table_area);
    history_table_area_layout->setContentsMargins(0,0,0,0);
    history_table_area_layout->setSpacing(5);
    //history_table_area_layout->addWidget(search_settings_area);
    search_settings_area->hide();
    history_table_area_layout->addWidget(history_title_panel);
    history_table_area_layout->addWidget(history_datetime_panel);
    history_table_area_layout->addWidget(history_table_panel);
    //history_table_area_layout->addWidget(history_statistics_panel);


    main_layout->setContentsMargins(10,10,10,10);
    main_layout->setSpacing(10);
    main_layout->addWidget(history_statistics_panel);
    main_layout->addWidget(history_table_area);

}

void HistoryDialog::set_style_sheet()
{
    search_settings_area->setStyleSheet(SS_White_Round_Corner_Panel);
    history_table_area->setStyleSheet(SS_White_Round_Corner_Panel);

    search_settings_area_left->setStyleSheet(SS_Button_Panel1);
    search_settings_area_right->setStyleSheet(SS_Button_Panel1);

    //搜索条件设置-左-设置按钮面板1、2、3
    search_settings_left_panel1->setStyleSheet(SS_White_Round_Corner_Panel);
    search_settings_left_panel2->setStyleSheet(SS_White_Round_Corner_Panel);
    search_settings_left_panel3->setStyleSheet(SS_White_Round_Corner_Panel);

    //搜索类型
    search_type_Label->setStyleSheet(SS_Normal_Label);
    search_type_checkbox1->setStyleSheet(SS_history_checkbox);
    search_type_checkbox2->setStyleSheet(SS_history_checkbox);
    search_type_checkbox3->setStyleSheet(SS_history_checkbox);

    //快速按钮标题
    search_quick_time_Label->setStyleSheet(SS_Normal_Label);

    //时间自定义选择
    datetimeEdit_Begin->setStyleSheet(SS_history_datetimeEdit);
    hyphen_Label->setStyleSheet(SS_Normal_Label);
    datetimeEdit_End->setStyleSheet(SS_history_datetimeEdit);

    //历史列表区域
    history_title->setStyleSheet(SS_Task_Title);
    history_title_panel->setStyleSheet(SS_Table_Area);
    search_btn->setStyleSheet(SS_Primary_Btn);
    export_btn->setStyleSheet(SS_Normal_Btn);
    delete_btn->setStyleSheet(SS_Normal_Btn);

    history_record_label->setStyleSheet(SS_Normal_Label);
    history_table_panel->setStyleSheet(SS_Table_Area);

    // 为历史记录表设置独立的样式表，确保滚动条在白色背景中可见
    history_table->setStyleSheet(SS_Task_Table_View);

    QHeaderView *header = history_table->horizontalHeader();
    header->setStyleSheet("background-color: #f7f7f7;font-family: '微软雅黑'; font-size: 16px; font-weight: bold; border: 1px #bcbcbc;");
    //history_table->insertRow(0);

    statistics_sum_info->setStyleSheet(SS_Statistics_Info);

    history_statistics_panel->setStyleSheet(SS_White_Round_Corner_Panel);
    //统计区域-上、下
    history_statistics_upper_panel->setStyleSheet(SS_Title_Area);
    statistics_title->setStyleSheet(SS_Task_Title);
    history_statistics_lower_panel->setStyleSheet(SS_Table_Area);
}

void HistoryDialog::slot_search_quick_time_buttonClicked(int id)
{
   datetimeEdit_Begin->hide();
   hyphen_Label->hide();
   datetimeEdit_End->hide();

   QList<int> taskTypes = {};
   if(search_type_checkbox1->isChecked())
   {
       taskTypes.append(TaskType::LOCAL_TASK);
   }
   if(search_type_checkbox2->isChecked())
   {
       taskTypes.append(TaskType::REMOTE_TEXT_TASK);
   }
   if(search_type_checkbox3->isChecked())
   {
       taskTypes.append(TaskType::REMOTE_IMAGE_TASK);
   }

   if(0==id)
   {//今天
       sel_startTime = QDateTime::currentDateTime();
       sel_endTime = QDateTime::currentDateTime();
       load_HistoryTask_List(taskTypes,sel_startTime,sel_endTime);
   }
   if(1==id)
   {//昨天
       sel_startTime = QDateTime::currentDateTime().addDays(-1);
       sel_endTime = QDateTime::currentDateTime().addDays(-1);
       load_HistoryTask_List(taskTypes,sel_startTime,sel_endTime);
   }
   if(2==id)
   {//本周
       //sel_startTime = QDateTime::currentDateTime().addDays(-7);
       //sel_endTime = QDateTime::currentDateTime();
       calculateDateRangeForWeek(sel_startTime, sel_endTime);
       load_HistoryTask_List(taskTypes,sel_startTime,sel_endTime);
   }
   if(3==id)
   {//本月
       //sel_startTime = QDateTime::currentDateTime().addDays(-30);
       //sel_endTime = QDateTime::currentDateTime();
       calculateDateRangeForMonth(sel_startTime, sel_endTime);
       load_HistoryTask_List(taskTypes,sel_startTime,sel_endTime);
   }
   if(id == 4)
   {//自定义
       datetimeEdit_Begin->show();
       hyphen_Label->show();
       datetimeEdit_End->show();
   }

   QAbstractButton* clickedButton = search_quick_time_button->button(id);
   if (clickedButton)
   {
       clickedButton->setChecked(true);
   }

   show_history_search_panel(true);
}

void HistoryDialog::slot_statistics_operator_buttonClicked(int id)
{
//    QString queryStr = GenSQLQueryString(id);

//    std::vector<int>  queryVector = LoadQueryResults(queryStr);

//    if(queryVector.size() > 0)
//    {
//       DrawBarChartProc(id, queryVector);
//    }

    loadHistoryQueryData_OnThread(id);
}

void HistoryDialog::slot_search_buttonClicked()
{
    slot_search_datetime_Changed();

    QList<int> taskTypes = {};
    if(search_type_checkbox1->isChecked())
    {
        taskTypes.append(TaskType::LOCAL_TASK);
    }
    if(search_type_checkbox2->isChecked())
    {
        taskTypes.append(TaskType::REMOTE_TEXT_TASK);
    }
    if(search_type_checkbox3->isChecked())
    {
        taskTypes.append(TaskType::REMOTE_IMAGE_TASK);
    }


    sel_startTime = datetimeEdit_Begin->dateTime();
    sel_endTime = datetimeEdit_End->dateTime();


    load_HistoryTask_List(taskTypes,sel_startTime,sel_endTime);

    show_history_search_panel(true);

}

void HistoryDialog::slot_search_operator_buttonClicked(int id)
{

    QAbstractButton* clickedButton = search_operator_button->button(id);

    qDebug() << "slot_search_operator_buttonClicked.id===" << id;

    //统计
    if(0==id)
    {
        if (clickedButton)
        {
            if(!history_statistics_panel->isVisible())
            {
                // 默认统计今天
                statistics_quick_time_button->button(0)->setChecked(true);
                slot_statistics_operator_buttonClicked(0);
                show_history_search_panel(false);
            }
            else
            {
                show_history_search_panel(true);
            }
        }
    }
    //搜索
    if(1==id)
    {
        slot_search_datetime_Changed();

        QList<int> taskTypes = {};
        if(search_type_checkbox1->isChecked())
        {
            taskTypes.append(TaskType::LOCAL_TASK);
        }
        if(search_type_checkbox2->isChecked())
        {
            taskTypes.append(TaskType::REMOTE_TEXT_TASK);
        }
        if(search_type_checkbox3->isChecked())
        {
            taskTypes.append(TaskType::REMOTE_IMAGE_TASK);
        }

        //自定义时间范围
        if(search_quick_time_button->checkedId() == 4)
        {
           sel_startTime = datetimeEdit_Begin->dateTime();
           sel_endTime = datetimeEdit_End->dateTime();
        }

        load_HistoryTask_List(taskTypes,sel_startTime,sel_endTime);

        show_history_search_panel(true);
    }
    //重置
    if(2==id)
    {
        search_type_checkbox1->setChecked(false);
        search_type_checkbox2->setChecked(false);
        search_type_checkbox3->setChecked(false);

        //默认选择自定义
        search_quick_time_button->button(4)->click();

        // 清空表格内容
        //history_table->clearContents();
        //history_table->setRowCount(0);
        history_model->clearAllTasks();

        show_history_search_panel(true);

        datetimeEdit_Begin->blockSignals(true);
        datetimeEdit_End->blockSignals(true);
        datetimeEdit_Begin->setDateTime(QDateTime::currentDateTime());
        datetimeEdit_End->setDateTime(QDateTime::currentDateTime());
        datetimeEdit_Begin->blockSignals(false);
        datetimeEdit_End->blockSignals(false);
    }
}

void HistoryDialog::slot_search_datetime_Changed()
{
    QDateTime begin = datetimeEdit_Begin->dateTime();
    QDateTime endOfDay(datetimeEdit_End->date(),QTime(23,59,59));
    QDateTime end = endOfDay;

     // 检查开始时间是否大于结束时间
     if (begin > end) {
         DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
         msg_box->setContent(tr("开始时间不能晚于结束时间！"));
         msg_box->exec();
         // 复位到有效值
         //datetimeEdit_Begin->setDateTime(end.addSecs(-1));
         return;
     }

     // 检查时间范围是否超过半年
//     QDateTime sixMonthsLater = begin.addMonths(6);
//     if (end > sixMonthsLater) {
//         DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
//         msg_box->setContent(tr("选择的时间范围不能超过半年！"));
//         msg_box->exec();
//         // 复位到半年范围
//         datetimeEdit_End->setDateTime(sixMonthsLater);
//     }
}

/*
void HistoryDialog::on_export_btn_clicked()
{
    DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);

    QList<QTableWidgetSelectionRange> selectedRanges = history_table->selectedRanges();
    if(selectedRanges.size() < 1)
    {
        DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
        msg_box->setContent(tr("请选择导出记录。"));
        msg_box->exec();
        return;
    }

    QTableWidget *testDataExportTableWidget = new QTableWidget(); // 目标表格
    // 设置列标题
    QStringList headers;
    headers << tr("信息") << tr("内容") << tr("时间") ;

    //必须要先设置setColumnCount(),后面setHorizontalHeaderLabels()的顺序
    testDataExportTableWidget->setColumnCount(headers.count());
    testDataExportTableWidget->setHorizontalHeaderLabels(headers);

    // 弹出文件保存对话框，让用户选择保存路径和文件名
    QString fileName = QFileDialog::getSaveFileName(nullptr, tr("保存CSV文件"), QDir::homePath() + "/Desktop", tr("CSV文件(*.csv)"));

    if (!fileName.isEmpty())
    {

        if(!processTableData(history_table, testDataExportTableWidget,fileName))
        {
            msg_box->setContent(tr("数据导出错误！"));
        }
        else
        {
            msg_box->setContent(tr("数据导出完成！"));
        }

        msg_box->exec();

    }
    else
    {
        //msg_box->setContent(tr("用户取消了保存操作！"));
    }

    // 调用函数处理选中数据并导出
    //processTableData(history_table, testDataExportTableWidget);


}

void HistoryDialog::on_delete_btn_clicked()
{

   QList<QTableWidgetSelectionRange> selectedRanges = history_table->selectedRanges();
   if(selectedRanges.size() == 0)
   {
       DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
       msg_box->setContent(tr("请选择需要删除的任务。"));
       msg_box->exec();
       return;
   }
   DialogMsgBox *msg_box = new DialogMsgBox(ASK_DIALOG);
   msg_box->setContent(tr("该操作无法还原，\n是否确定删除？"));
   if(msg_box->exec() != QDialog::Accepted)
   {
       return;
   }

   //parentWidget()->setEnabled(false);
   // 暂时断开 selectionChanged 信号连接
   disconnect(history_table->model(), &QAbstractItemModel::rowsRemoved,
           this, &HistoryDialog::on_row_state_changed);
   disconnect(history_table->selectionModel(), &QItemSelectionModel::selectionChanged,
           this, &HistoryDialog::on_row_state_changed);



   history_table->clearSelection();
   QProgressDialog progress_dialog(tr("正在删除，请稍候..."), QString(), 0, 0, nullptr);
   progress_dialog.setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint); // 无边框窗口
   progress_dialog.setFixedSize(300,100);
   progress_dialog.setStyleSheet(SS_ProgressDialog);
   progress_dialog.setWindowModality(Qt::WindowModal);
   progress_dialog.show();

   for (int i = selectedRanges.size() - 1; i >= 0; --i)
   {
       QTableWidgetSelectionRange range = selectedRanges[i];
       for (int row = range.bottomRow(); row >= range.topRow(); --row)
       {
           QString taskid = history_table->item(row,ColNum::COL_NAME)->data(Qt::UserRole).toString();
           print_task_manager.deletePrintTask(taskid);
           //task_list_ini.remove(task_table->item(row, 1)->text());//删除相应任务名的key
           history_table->removeRow(row);//删除列表对应行

           if(row%100 == 0)
            {
                QCoreApplication::processEvents();
                on_row_state_changed();
            }
       }
   }

   //重排序号
   int row_count = history_table->rowCount();
   for (int i=0;i<row_count;i++)
   {
       QTableWidgetItem *item;
       item = new QTableWidgetItem(QString::number(i+1));
       item->setTextAlignment(Qt::AlignCenter);
       history_table->setItem(i, 0, item);
   }

   // 删除任务完成后，恢复信号连接
   connect(history_table->model(), &QAbstractItemModel::rowsRemoved,
           this, &HistoryDialog::on_row_state_changed);
   connect(history_table->selectionModel(), &QItemSelectionModel::selectionChanged,
           this, &HistoryDialog::on_row_state_changed);

   //parentWidget()->setEnabled(true);
   //删除后要执行一次搜索，才能正常统计总数
   slot_search_operator_buttonClicked(1);

}

*/


void HistoryDialog::on_export_btn_clicked()
{
    // 获取选中行
    QModelIndexList selectedRows = history_table->selectionModel()->selectedRows();
    if (selectedRows.isEmpty())
    {
        DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
        msg_box->setContent(tr("请选择需要导出的记录。"));
        msg_box->exec();
        return;
    }

    // 获取导出路径
    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("导出 CSV 文件"),
        QDir::homePath() + "/history_export.csv",
        tr("CSV 文件 (*.csv)")
    );

    if (filePath.isEmpty())
        return;

    // 导出逻辑
    exportSelectedRowsToCsv(selectedRows, filePath);

    DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
    //msg_box->setContent(tr("导出完成：\n%1").arg(filePath));
    msg_box->setContent(QString("%1\r\n%2").arg(tr("数据导出完成！"),filePath));
    msg_box->exec();
}


void HistoryDialog::on_delete_btn_clicked()
{
    auto selectedIndexes = history_table->selectionModel()->selectedRows();
    if (selectedIndexes.isEmpty()) {
        DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
        msg_box->setContent(tr("请选择需要删除的任务。"));
        msg_box->exec();
        return;
    }

    DialogMsgBox *msg_box = new DialogMsgBox(ASK_DIALOG);
    msg_box->setContent(tr("该操作无法还原，\n是否确定删除？"));
    if (msg_box->exec() != QDialog::Accepted)
        return;

    // 暂时断开信号
    disconnect(history_table->model(), &QAbstractItemModel::rowsRemoved,
            this, &HistoryDialog::on_row_state_changed);
    disconnect(history_table->selectionModel(), &QItemSelectionModel::selectionChanged,
               this, &HistoryDialog::on_row_state_changed);

    // ===== 收集所有选中 GUID =====
    QStringList guids;
    QList<int> rowsToDelete;
    for (const QModelIndex &idx : selectedIndexes)
    {
        int row = idx.row();
        QString guid = history_model->data(history_model->index(row, ColNum::COL_INDEX), Qt::UserRole).toString();
        guids << guid;
        rowsToDelete << row;
    }
    // --- 进度提示 ---
    //QProgressDialog progress(tr("正在删除历史记录，请稍候..."), QString(), 0, guids.size(), this);
    QProgressDialog progress_dialog(tr("正在删除，请稍候..."), QString(), 0, guids.size(), nullptr);
    progress_dialog.setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    progress_dialog.setFixedSize(300, 100);
    progress_dialog.setStyleSheet(SS_ProgressDialog);
    progress_dialog.setWindowModality(Qt::ApplicationModal);
    progress_dialog.show();

    // ===== 调用批量删除接口 =====
    print_task_manager.deleteHistoryBatch(guids);

    // ===== 删除模型中的行 =====
    std::sort(rowsToDelete.begin(), rowsToDelete.end(), std::greater<int>()); // 从后往前删
    for (int i = 0; i < rowsToDelete.size(); ++i)
    {
        history_model->removeRow(rowsToDelete[i]);

        progress_dialog.setValue(i);
        if(i%100 == 0)
        {
            QCoreApplication::processEvents();
        }
    }

    history_model->removeTableRows(rowsToDelete);

    history_table->clearSelection();
    history_table->viewport()->update();

    progress_dialog.close();

    // 恢复信号连接
    connect(history_table->model(), &QAbstractItemModel::rowsRemoved,
            this, &HistoryDialog::on_row_state_changed);
    connect(history_table->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &HistoryDialog::on_row_state_changed);

    on_row_state_changed();

    // 删除后重新统计
//    int idx = statistics_quick_time_button->checkedId();
//    qDebug() << "on_delete_btn_clicked.idx====>>>>" << idx;
//    slot_statistics_operator_buttonClicked(idx);
}

void HistoryDialog::retranslateUi()
{
    if(settings.language_setting.language_cbb.compare("简体中文") == 0)
    {
        datetimeEdit_Begin->setLocale(QLocale(QLocale::Chinese));
        datetimeEdit_End->setLocale(QLocale(QLocale::Chinese));
    }
    else if(settings.language_setting.language_cbb.compare("English") == 0)
    {
        datetimeEdit_Begin->setLocale(QLocale(QLocale::English));
        datetimeEdit_End->setLocale(QLocale(QLocale::English));
    }

    statistics_title->setText(tr("打印统计"));
}

void HistoryDialog::loadHistoryQueryData_OnThread(int id)
{
    //线程加载数据
    QThread* thread = new QThread();
    connect(thread, &QThread::started, thread, [=]()mutable{
        qDebug()<<"loadHistoryQueryData_OnThread.started!";
        thread->quit();
    }, Qt::DirectConnection);
    connect(thread, &QThread::finished, this, [=]()mutable{
        thread->wait();
        thread->deleteLater();
        qDebug()<<"loadHistoryQueryData_OnThread.finished!";

//        QProgressDialog progress_dialog(tr("统计中，请稍候..."), QString(), 0, 0, nullptr);
//        progress_dialog.setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint); // 无边框窗口
//        progress_dialog.setFixedSize(300, 100);
//        progress_dialog.setStyleSheet(SS_ProgressDialog);
//        progress_dialog.setWindowModality(Qt::WindowModal);
//        progress_dialog.show();

//        QString queryStr = GenSQLQueryString(id);

//        QCoreApplication::processEvents();

        StatisticsVector  queryVector;

        queryVector = LoadQueryResults(id);

//        QThread::msleep(50);

        if(queryVector.size() > 0)
        {
           DrawBarChartProc(id, queryVector);
        }

//        progress_dialog.close();

    });
    thread->start();
}

void HistoryDialog::on_row_state_changed()
{
//    int totalRows = history_table->rowCount();
//    int selectedRows = history_table->selectionModel()->selectedRows().count();
    int totalRows = history_model->rowCount();
    int selectedRows = history_table->selectionModel()->selectedRows().count();

    QString labelText = QString("%1: <span style='color:#4d4d4d'>%3</span> | %2: <span style='color:#4d4d4d'>%4</span>").arg(tr("已选"), tr("全部"))
                           .arg(selectedRows)
                           .arg(totalRows);

    history_record_label->setText(labelText);
}

void HistoryDialog::on_select_all_item_clicked(int logicalIndex)
{
    if (logicalIndex == 0)
    {
       history_table->selectAll();
    }
}

QFrame *HistoryDialog::seprate_line()
{
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFixedHeight(1);
    line->setStyleSheet("background-color: #e0e0e0;");
    return line;
}

void HistoryDialog::show_history_search_panel(bool isShow)
{
    return;

    if(isShow)
    {
        history_title_panel->show();
        history_table_panel->show();
        history_statistics_panel->hide();
    }
    else
    {
        history_title_panel->hide();
        history_table_panel->hide();
        history_statistics_panel->show();
    }
}

QString HistoryDialog::make_task_type_info(const QString &task_text)
{
    QString task_display_text ;
    QStringList task_text_split = task_text.split(SPLIT_SYMBOL);

    if(task_text_split.size() < TaskTextPos::TASK_MAX_ITEM)
    {
        return task_display_text;
    }

    QString task_type = TaskListPanel::task_type_display_str(task_text_split[TaskTextPos::TASK_TYPE].toInt());
    QString task_info = QString("%1 %2\n%3").arg(task_text_split[TaskTextPos::TASK_TEMP],task_type,task_text_split[TaskTextPos::TASK_TIME]);
    return task_info;
}

QString HistoryDialog::make_task_display_text(const QString &task_text)
{
    QString task_display_text ;
    QStringList task_text_split = task_text.split(SPLIT_SYMBOL);

    if(task_text_split.size() < TaskTextPos::TASK_MAX_ITEM)
    {
        return task_display_text;
    }

    //QString task_sta = sta_display_str(task_text_split[TaskTextPos::TASK_STA].toInt());

    QString task_type = TaskListPanel::task_type_display_str(task_text_split[TaskTextPos::TASK_TYPE].toInt());
    QString task_info = QString("%1 %2\n%3").arg(task_text_split[TaskTextPos::TASK_TEMP],task_type,task_text_split[TaskTextPos::TASK_TIME]);

    QStringList task_text_list = task_text_split.mid(TaskTextPos::TASK_TEXT, MAX_INPUT_PANEL);
    task_text_list.removeAll("");
    QString interval_symbol = settings.list_setting.print_list_interval_symbol_le;
    QString task_text_display = task_text_list.join(interval_symbol);
    QString task_slots_display = QString(tr("槽盒：%1")).arg(TaskListPanel::slots_display_str(task_text_split[TaskTextPos::TASK_SLOTS]));
    task_display_text = QString("%1\n%2").arg(task_text_display, task_slots_display);

    return task_display_text;

}


QString HistoryDialog::GenSQLQueryString(unsigned short int iperType)
{
    QStringList caseStrList;
    QDate currentDate = QDate::currentDate();
    int queryCount = 0;
    QString genSqlQueryStr;

    switch (iperType)
    {
    case 0: // 按日统计 - 当前月份的每日数据
    {
        QString splitchar;
        //QSqlQuery query("SELECT MIN(strftime('%Y-%m-%d', timestamp)), MAX(strftime('%Y-%m-%d', timestamp)) FROM Print_task_history ");
        QSqlQuery query(QString("SELECT MIN(strftime('%Y-%m-%d', timestamp)), MAX(strftime('%Y-%m-%d', timestamp)) "
                                "FROM (SELECT timestamp FROM Print_task_history ORDER BY timestamp DESC LIMIT %1) ").arg(STATISTICS_MAX_COUNT_DAY)
                        );
        if (query.exec() && query.next())
        {
            QString startDate = query.value(0).toString();
            QString endDate = query.value(1).toString();
            m_beginDate = QDate::fromString(startDate, Qt::ISODate);
            m_endDate = QDate::fromString(endDate, Qt::ISODate);
            queryCount = m_beginDate.daysTo(m_endDate) +1 ;

            for (int i = 0; i < queryCount; ++i)
            {
                splitchar = (i == 0) ? "" : ",";
                caseStrList += QString("%1SUM(CASE strftime('%Y-%m-%d', timestamp) WHEN '%2' THEN 1 ELSE 0 END) AS d%3")
                        .arg(splitchar)
                        .arg(m_beginDate.addDays(i).toString("yyyy-MM-dd"))
                        .arg(i + 1);
            }
        }
        break;
    }

    case 1: // 按周统计 - 当前月份的每周数据
    {
        QDate startOfMonth = QDate(currentDate.year(), currentDate.month(), 1);
        QDate endOfMonth = startOfMonth.addMonths(1).addDays(-1);
        QDate startOfWeek = startOfMonth.addDays(-startOfMonth.dayOfWeek() + 1); // 找到当月首周的周一
        m_beginDate = startOfMonth;
        m_endDate = endOfMonth;
        while (startOfWeek <= endOfMonth)
        {
            QString splitchar = caseStrList.isEmpty() ? "" : ",";
            QDate endOfWeek = startOfWeek.addDays(6);
            if (endOfWeek > endOfMonth)
                endOfWeek = endOfMonth;

            caseStrList += QString("%1SUM(CASE WHEN timestamp BETWEEN '%2 00:00:00' AND '%3 23:59:59' THEN 1 ELSE 0 END) AS w%4")
                    .arg(splitchar)
                    .arg(startOfWeek.toString("yyyy-MM-dd"))
                    .arg(endOfWeek.toString("yyyy-MM-dd"))
                    .arg(caseStrList.size() + 1);

            startOfWeek = endOfWeek.addDays(1); // 下一周
        }

        break;
    }

    case 2: // 按月统计 - 当前年份的每月数据
    {
        QString splitchar;
        QSqlQuery query("SELECT MIN(strftime('%Y-%m-%d', timestamp)), MAX(strftime('%Y-%m-%d', timestamp)) FROM Print_task_history");
        if (query.exec() && query.next())
        {
            QString startDate = query.value(0).toString();
            QString endDate = query.value(1).toString();
            m_beginDate = QDate::fromString(startDate, Qt::ISODate);
            m_endDate = QDate::fromString(endDate, Qt::ISODate);
            queryCount = (m_endDate.year() - m_beginDate.year()) * 12 + (m_endDate.month() - m_beginDate.month()) +1 ;

            for (int i = 0; i < queryCount; ++i)
            {
                splitchar = (i == 0) ? "" : ",";
                caseStrList += QString("%1SUM(CASE strftime('%Y-%m', timestamp) WHEN '%2' THEN 1 ELSE 0 END) AS m%3")
                        .arg(splitchar)
                        .arg(m_beginDate.addMonths(i).toString("yyyy-MM"))
                        .arg(i + 1);
            }
        }
        break;
    }

    case 3: // 按年统计 - 数据库中的所有年份数据
    {
        QString splitchar;
        QSqlQuery query("SELECT MIN(strftime('%Y', timestamp)), MAX(strftime('%Y', timestamp)) FROM Print_task_history");
        if (query.exec() && query.next())
        {
            int startYear = query.value(0).toInt();
            int endYear = query.value(1).toInt();
            queryCount = endYear - startYear + 1;
            m_beginDate.setDate(startYear,1,1);
            m_endDate.setDate(endYear,1,1);
            for (int i = 0; i < queryCount; ++i)
            {
                splitchar = (i == 0) ? "" : ",";
                caseStrList += QString("%1SUM(CASE strftime('%Y', timestamp) WHEN '%2' THEN 1 ELSE 0 END) AS y%3")
                        .arg(splitchar)
                        .arg(startYear + i)
                        .arg(i + 1);
            }
        }
        break;
    }

    default:
        qDebug() << "Invalid type for statistics.";
        return QString();
    }

    if(0!=iperType)
    {
         genSqlQueryStr = QString("SELECT %1 FROM Print_task_history").arg(caseStrList.join(""));
         qDebug() << "[TIME] GenSQLQueryString.qryStr0:" << genSqlQueryStr << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
         return genSqlQueryStr;
    }
    else
    {
        genSqlQueryStr = QString("SELECT %1 FROM (SELECT * FROM Print_task_history ORDER BY timestamp DESC LIMIT %2)").arg(caseStrList.join("")).arg(STATISTICS_MAX_COUNT_DAY);
        qDebug() << "[TIME] GenSQLQueryString.qryStr1:" << genSqlQueryStr << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        return genSqlQueryStr;
    }
}

StatisticsVector HistoryDialog::LoadQueryResults(int perType)
{
    StatisticsVector statisticsVector;
    QList<QVariantList> result;

    qDebug() << "[TIME] statisticsPrintTask开始:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") << "perType:" << perType;
    
    bool success = false;
    QDate currentDate = QDate::currentDate();
    
    switch (perType)
    {
    case 0: // 按日统计
    {
        // 获取当前月份的日期范围
        QDate startDate = QDate(currentDate.year(), currentDate.month(), 1);
        QDate endDate = currentDate;
        success = PrintTaskDataManagement::getInstance().getDailyStatistics(startDate, endDate, result);
        break;
    }
    case 1: // 按周统计
    {
        // 周统计需要特殊处理，因为统计表只存储每日数据
        // 可以使用旧的统计方式，或者在新的统计方法中实现周统计逻辑
        // 对于周统计，我们需要使用原来的GenSQLQueryString生成SQL语句
        QString qryStr = GenSQLQueryString(perType);
        success = PrintTaskDataManagement::getInstance().statisticsPrintTask(qryStr, result);
        break;
    }
    case 2: // 按月统计
        // 获取当前年份的月度统计
        success = PrintTaskDataManagement::getInstance().getMonthlyStatistics(currentDate.year(), result);
        break;
    case 3: // 按年统计
        // 获取所有年份的统计
        success = PrintTaskDataManagement::getInstance().getYearlyStatistics(result);
        break;
    default:
        // 默认使用旧的统计方式，为了兼容可能的其他统计类型
        qDebug() << "Unknown statistics type:" << perType;
        success = false;
    }
    
    if (success)
    {
        if (perType == 1) // 周统计的结果格式不同
        {
            // 周统计的结果是一行数据，每个字段代表一周的打印次数
            for (const auto& row : result)
            {
                for (int i = 0; i < row.size(); ++i)
                {
                    StatisticsData data;
                    data.date = QString("周%1").arg(i + 1);
                    data.count = row[i].toInt();
                    statisticsVector.push_back(data);
                    qDebug() << "日期:" << data.date << " 次数:" << data.count;
                }
            }
        }
        else // 日、月、年统计的结果格式相同
        {
            // 日、月、年统计的结果是多行数据，每行包含日期和打印次数
            for (const auto& row : result)
            {
                if (row.size() >= 2)
                {
                    StatisticsData data;
                    data.date = row[0].toString();
                    data.count = row[1].toInt();
                    statisticsVector.push_back(data);
                    qDebug() << "日期:" << data.date << " 次数:" << data.count;
                }
            }
        }
    }
    qDebug() << "[TIME] statisticsPrintTask结束:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

    return statisticsVector;
}

void HistoryDialog::LoadSelectedDataToExportTable(QTableWidget & exportTableWidget)
{
    int newRow = exportTableWidget.rowCount();
    exportTableWidget.setRowCount(newRow + 1);

    //"Run ID"
    QTableWidgetItem * item0 = new QTableWidgetItem("");
    exportTableWidget.setItem(newRow, 0, item0);

    //"Assay Name"
    QTableWidgetItem * item1 = new QTableWidgetItem("");
    exportTableWidget.setItem(newRow, 1, item1);

    //"Test Name"
    QTableWidgetItem * item2 = new QTableWidgetItem("");
    exportTableWidget.setItem(newRow, 2, item2);
}

void HistoryDialog::copySelectedRowsToTable(QTableWidget *sourceTable, QTableWidget *targetTable)
{
    // 清空目标表格
    //targetTable->clear();
    //targetTable->setRowCount(0);
    //targetTable->setColumnCount(3); // 目标表格只有 3 列

    // 遍历 sourceTable 的选中行
    for (int row = 0; row < sourceTable->rowCount(); ++row) {
        if (sourceTable->isRowHidden(row)) continue; // 跳过隐藏的行
        if (sourceTable->selectionModel()->isRowSelected(row, QModelIndex())) {
            // 添加一行到 targetTable
            int newRow = targetTable->rowCount();
            targetTable->insertRow(newRow);

            // 将源表格的 3, 4, 5 列映射到目标表格的 0, 1, 2 列
            for (int col = 0; col < 3; ++col) {
                int sourceCol = col + 3; // 源表格中的列索引
                QTableWidgetItem *sourceItem = sourceTable->item(row, sourceCol);
                if (sourceItem) {
                    qDebug() << QString("sourceTable[%1][%2]==%3").arg(row).arg(sourceCol).arg(sourceItem->text());
                    targetTable->setItem(newRow, col, new QTableWidgetItem(*sourceItem));
                }
            }
        }
    }
}

// 主函数：执行数据拷贝和导出
bool HistoryDialog::processTableData(QTableWidget *history_table, QTableWidget *testDataExportTableWidget,const QString ouputFilePath)
{

//    DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);

//    QList<QTableWidgetSelectionRange> selectedRanges = history_table->selectedRanges();
//    if(selectedRanges.size() < 1)
//    {
//        DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
//        msg_box->setContent(tr("请选择导出记录。"));
//        msg_box->exec();
//        return;
//    }

    bool ret = false;

    QProgressDialog progress_dialog(tr("正在导出，请稍候..."), QString(), 0, 0, nullptr);
    progress_dialog.setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint); // 无边框窗口
    progress_dialog.setFixedSize(300,100);
    progress_dialog.setStyleSheet(SS_ProgressDialog);
    progress_dialog.setWindowModality(Qt::WindowModal);
    progress_dialog.show();

    // 复制选中数据
    copySelectedRowsToTable(history_table, testDataExportTableWidget);

    // 获取桌面路径并生成文件名
   //    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
   //    QString dateTime = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
   //    QString destinationFilePath = QString("%1/%2.csv").arg(desktopPath, dateTime);


    // 导出数据到 CSV 文件
    if(!exportTableWidgetToCsv(testDataExportTableWidget, ouputFilePath))
    {

        //msg_box->setContent(tr("数据导出错误！"));
        ret = false;
    }
    else
    {
        //msg_box->setContent(tr("数据导出完成！"));
        ret = true;
    }


    QEventLoop eventloop;
    QTimer::singleShot(1000, &eventloop, SLOT(quit()));
    eventloop.exec();
    progress_dialog.close();
    QTimer::singleShot(200, &eventloop, SLOT(quit()));
    eventloop.exec();

    //msg_box->exec();
    return ret;
}

bool HistoryDialog::exportTableWidgetToCsv(QTableWidget* tableWidget, const QString& fileName)
{
    // 转换文件名为本地编码
    QTextCodec* codec = QTextCodec::codecForLocale();
    QByteArray localFileName = codec->fromUnicode(fileName);

    QFile file(QString::fromLocal8Bit(localFileName));
    qDebug() << QString("ExportFileName=%1").arg(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file for writing:" << localFileName;
        return false;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");
    out.setGenerateByteOrderMark(true);

    qDebug() << "Exporting Table Header:";
    // 写入表头
    for (int col = 0; col < tableWidget->columnCount(); ++col)
    {
        QTableWidgetItem* headerItem = tableWidget->horizontalHeaderItem(col);
        if (headerItem)
        {
            qDebug() << "Header" << col << ":" << headerItem->text();
            out << "\"" << headerItem->text().replace("\"", "\"\"") << "\"";
        }
        else
        {
            qDebug() << "Header" << col << "is null.";
            out << "\"\"";
        }
        if (col < tableWidget->columnCount() - 1) {
            out << ",";
        }
    }
    out << "\n";

    // 写入数据
    qDebug() << "Exporting Table Data:";
    for (int row = 0; row < tableWidget->rowCount(); ++row)
    {
        QStringList rowData;
        for (int col = 0; col < tableWidget->columnCount(); ++col) {
            QTableWidgetItem* item = tableWidget->item(row, col);
            if (item) {
                out << "\"" << item->text().replace("\"", "\"\"") << "\"";
                rowData << item->text();
            } else {
                out << "\"\"";
                rowData << "<empty>";
            }
            if (col < tableWidget->columnCount() - 1) {
                out << ",";
            }
        }
        out << "\n";
        qDebug() << "Row" << row << ":" << rowData.join(", ");
    }

    file.close();
    return true;
}

void HistoryDialog::exportSelectedRowsToCsv(const QModelIndexList &selectedRows, const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
        msg_box->setContent(QString("%1%2").arg(tr("无法写入文件：")).arg(filePath));
        msg_box->exec();
        return;
    }

    QTextStream stream(&file);
    stream.setCodec("UTF-8");

    const int colCount = history_model->columnCount();
    QStringList headers;

    // 获取表头文本
    for (int col = 0; col < colCount; ++col)
    {
        if (history_table->isColumnHidden(col))
            continue;
        headers << escapeCsvField(history_model->headerData(col, Qt::Horizontal, Qt::DisplayRole).toString());
    }
    writeCsvRow(stream, headers);

    // === 遍历选中行 ===
    QProgressDialog progress(tr("正在导出数据..."), QString(), 0, selectedRows.size(), nullptr);
    progress.setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    progress.setFixedSize(300, 100);
    progress.setStyleSheet(SS_ProgressDialog);
    progress.setWindowModality(Qt::ApplicationModal);
    progress.show();

    int processed = 0;
    for (const QModelIndex &index : selectedRows)
    {
        int row = index.row();
        QStringList rowFields;

        for (int col = 0; col < colCount; ++col)
        {
            if (history_table->isColumnHidden(col))
                continue;
            QModelIndex idx = history_model->index(row, col);
            QString value = history_model->data(idx, Qt::DisplayRole).toString();
            rowFields << escapeCsvField(value);
        }

        writeCsvRow(stream, rowFields);

        processed++;
        if (processed % 100 == 0)
        {
            progress.setValue(processed);
            QCoreApplication::processEvents();
        }
    }

    file.close();
    progress.close();
}

QString HistoryDialog::escapeCsvField(const QString &field)
{
    QString f = field;
    f.replace("\"", "\"\"");  // 转义内部引号

    // 如果包含逗号、换行、回车、引号，则整体用双引号包裹
    if (f.contains(',') || f.contains('\n') || f.contains('\r') || f.contains('"')) {
        f = "\"" + f + "\"";
    }
    return f;
}

void HistoryDialog::writeCsvRow(QTextStream &stream, const QStringList &fields)
{
    stream << fields.join(",") << "\n";
}

void HistoryDialog::DrawChartProc(int nPerType, const std::vector<int> &queryVector)
{
    QString PerStr;
    QString TitleStr = tr("工作量");
    switch (nPerType)
    {
    case 0:
        PerStr = "MM/dd";
        TitleStr = tr("最近90天每日工作量");
        break;
    case 1:
        PerStr = "周";
        TitleStr = tr("本月每周工作量");
        break;
    case 2:
        PerStr = "yy/MM";
        TitleStr = tr("本年每月工作量");
        break;
    case 3:
        PerStr = "yyyy";
        TitleStr = tr("年份工作量");
        break;
    default:
        PerStr = "MMdd";
        break;
    }

    // 配置图表
    //scene->clear();
    auto *barChart = new QChart();
    //auto barChart = std::make_unique<QChart>();
    //barChart->setTitle(TitleStr);
    barChart->legend()->setVisible(false);//去掉图例
    barChart->setAnimationOptions(QChart::SeriesAnimations);

    // 配置柱状数据
    auto *bars = new QBarSeries();
    auto *set = new QBarSet(tr(""));
    //auto bars = std::make_unique<QBarSeries>();
    //auto set = std::make_unique<QBarSet>(tr(""));
    set->setBrush(QBrush(qRgba(0, 97, 176, 255)));
    for (const int &value : queryVector)
    {
        *set << value;
    }

    bars->append(set);
    barChart->addSeries(bars);

    // 使柱状图上的标签可见
    bars->setLabelsPosition(QAbstractBarSeries::LabelsPosition::LabelsOutsideEnd);
    bars->setLabelsVisible(true);
    bars->setLabelsFormat("<font color='black'><b>@value</b></font>");

    // 配置X轴标签
    QStringList categories;
    for (unsigned short int i = 0; i < queryVector.size(); ++i)
    {
        switch (nPerType)
        {
        case 0:
            categories << m_beginDate.addDays(i).toString(PerStr);
            break;
        case 1:
            categories << QString("%1%2").arg(PerStr).arg(i + 1);
            break;
        case 2:
            categories << m_beginDate.addMonths(i).toString(PerStr);
            break;
        case 3:
            categories << m_beginDate.addYears(i).toString(PerStr);
            break;
        }
    }

    auto *axisX = new QBarCategoryAxis();

    axisX->append(categories);

    //这里按年统计第一次不出结果?
    if(categories.count() < 1)
    {
        return;
    }

    // 限制X轴初始显示范围
    //axisX->setRange(categories.first(), categories[std::min(14, (int)categories.size() - 1)]);
    if(categories.size() > 8)
    {
        axisX->setRange(categories[categories.size()-8], categories.last());
    }
    else
    {
        axisX->setRange(categories.first(), categories.last());
    }

    // 允许拖动和缩放
//    auto *axisXScrollable = new QValueAxis();
//    axisXScrollable->setRange(0, std::min(10, (int)queryVector.size()));
//    axisXScrollable->setTickCount(std::min(10, (int)queryVector.size()));
//    barChart->setAxisX(axisXScrollable, bars);

    // 动态调整字体大小
    //int fontSize = queryVector.size() > 10 ? 8 : 8; // 超过10个柱子时，字体缩小6
    int fontSize = 11;
    QFont axisFont = axisX->labelsFont();
    axisFont.setPointSize(fontSize);
    axisX->setLabelsFont(axisFont);

    //axisX->setLabelsAngle(45); // 设置斜体显示
    barChart->setAxisX(axisX, bars);


    // 配置 Y 轴
    auto *axisY = new QValueAxis();
    //auto axisY = std::make_unique<QValueAxis>();
    axisY->setLabelFormat("%d");
    barChart->setAxisY(axisY, bars);

    // 设置Y轴范围
    axisY->setRange(0, *std::max_element(queryVector.begin(), queryVector.end()) * 1.1);

    // 创建ChartView
    CustomChartView *chartView = new CustomChartView(barChart,history_statistics_lower_panel);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setFixedHeight(560); // 固定高度

    connect(axisX, &QBarCategoryAxis::rangeChanged, this, [=](){
        qDebug() << "axisX rangeChanged";
//        bars->setLabelsPosition(QAbstractBarSeries::LabelsPosition::LabelsOutsideEnd);
//        bars->setLabelsFormat("<font color='black'><b>@value</b></font>");
//        bars->setLabelsVisible(false);
//        bars->setLabelsVisible(true);
//        chartView->chart()->update();
    });

    // 创建数据表格 (两行)
    QTableWidget *dataTable = new QTableWidget(2, queryVector.size(), this);
    dataTable->verticalHeader()->hide(); // 隐藏垂直序号
    dataTable->horizontalHeader()->hide(); // 隐藏水平序号
    dataTable->setFixedHeight(130);                                   // 固定高度
    dataTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽平均分配
    //dataTable->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);          // 防止扩展

    dataTable->setVerticalHeaderLabels({tr("X轴标签"), tr("Y轴数据")});

    for (int i = 0; i < queryVector.size(); ++i)
    {
        QTableWidgetItem *xItem = new QTableWidgetItem(categories.at(i));
        QTableWidgetItem *yItem = new QTableWidgetItem(QString::number(queryVector.at(i)));

        xItem->setTextAlignment(Qt::AlignCenter); // 文本居中
        yItem->setTextAlignment(Qt::AlignCenter); // 文本居中

        dataTable->setItem(0, i, xItem); // 第一行是X坐标
        dataTable->setItem(1, i, yItem); // 第二行是Y数据
    }

    dataTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
    dataTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);


    // 获取 history_statistics_lower_panel 的布局
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(history_statistics_lower_panel->layout());
    if (!layout)
    {
        // 如果没有布局，为该面板创建一个 QVBoxLayout
        layout = new QVBoxLayout(history_statistics_lower_panel);
        history_statistics_lower_panel->setLayout(layout);
    }

    // 清除原有的子控件（如果需要）
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr)
    {
        if (child->widget())
        {
            child->widget()->deleteLater();
        }
        delete child;
    }

    QLabel *sum_label = new QLabel();
    QList<int> taskTypes = {};
    QDateTime sel_startTime = QDateTime::fromString("1970-01-01",Qt::ISODate);
    QDateTime sel_endTime = QDateTime::currentDateTime();

    int total_print_cnt = print_task_manager.countPrintTasks(taskTypes, sel_startTime, sel_endTime);

    sum_label->setText(QString("(%1%2)").arg(tr("总打印数："), QString::number(total_print_cnt)));
    sum_label->setStyleSheet(SS_Statistics_Info);

    // 添加图表和数据表格到面板
    layout->addWidget(chartView);
    //隐藏图表
    layout->addStretch();
    layout->addWidget(sum_label);
    //layout->addWidget(dataTable);
    dataTable->hide();

}

void HistoryDialog::DrawBarChartProc(int nPerType, const StatisticsVector &statisticsVector)
{
    // 创建数据集
    QBarSet* set = new QBarSet("PrintTimes");
    set->setBrush(QBrush(qRgba(99, 208, 247, 255)));
    QStringList categories;

    for (const auto& data : statisticsVector)
    {
        *set << data.count;
        
        // 将日期格式中的"-"替换为"/"
        QString formattedDate = data.date;
        formattedDate.replace("-", "/");

        // 根据统计类型调整日期格式
        if (formattedDate.count("/") >= 2)
        {
            if (nPerType == 0) { // 按日统计，只保留月/日部分（如12/25）
                int firstSlashPos = formattedDate.indexOf("/");
                formattedDate = formattedDate.mid(firstSlashPos + 1);

            } else if (nPerType == 2) { // 按月统计，只保留年月部分（如2025/12）
                int secondSlashPos = formattedDate.lastIndexOf("/");
                formattedDate = formattedDate.left(secondSlashPos);
            }
        }
        
        categories << formattedDate;
    }

    // 创建系列
    QBarSeries* series = new QBarSeries();
    series->append(set);
    series->setLabelsPosition(QAbstractBarSeries::LabelsPosition::LabelsOutsideEnd);
    series->setLabelsVisible(true);
    series->setLabelsFormat("<font face='微软雅黑' color='black'><b>@value</b></font>");

    // 创建图表
    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->legend()->setVisible(false);//去掉图例
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // X轴配置
    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setRange(categories.first(), categories.last());
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QFont axisFont("微软雅黑", 10, QFont::Bold);

    axisX->setLabelsFont(axisFont);

    // Y轴配置
    QValueAxis* axisY = new QValueAxis();
    
    // 计算Y轴的最大范围，使用lambda表达式比较StatisticsData对象的count成员
    if (!statisticsVector.empty()) {
        auto maxIter = std::max_element(statisticsVector.begin(), statisticsVector.end(),
            [](const StatisticsData& a, const StatisticsData& b) {
                return a.count < b.count;
            });
        axisY->setRange(0, maxIter->count * 1.3);
    } else {
        axisY->setRange(0, 100); // 默认范围
    }
    
    axisY->setTickCount(6);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    axisY->hide();

    // 创建ChartView
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    chartView->setFixedHeight(400); // 固定高度 650

    if(nPerType == 0)
    {//按日显示，柱形窄一点
        if(categories.size() < 10)
        {
            chartView->setFixedWidth(720);
        }
        else
        {
            chartView->setFixedWidth(categories.size() * 80);
        }
    }
    else
    {
        if(categories.size() <= 7)
        {
            chartView->setFixedWidth(720);
        }
        else
        {
            chartView->setFixedWidth(categories.size() * 100);
        }
    }

    // 获取 history_statistics_lower_panel 的布局
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(history_statistics_lower_panel->layout());
    if (!layout)
    {
        // 如果没有布局，为该面板创建一个 QVBoxLayout
        layout = new QVBoxLayout(history_statistics_lower_panel);
        history_statistics_lower_panel->setLayout(layout);
    }

    // 清除原有的子控件（如果需要）
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr)
    {
        if (child->widget())
        {
            child->widget()->deleteLater();
        }
        delete child;
    }

    //QLabel *sum_label = new QLabel();
    qDebug() << "[TIME] getTotalPrintCount开始:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    int total_print_cnt = print_task_manager.getTotalPrintCount();
    qDebug() << "[TIME] getTotalPrintCount结束:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

    //sum_label->setText(QString("(%1%2)").arg(tr("总打印数："), QString::number(total_print_cnt)));
    //sum_label->setStyleSheet(SS_Task_Title);
    statistics_sum_info ->setText(QString(" (%1%2)").arg(tr("总打印数："), QString::number(total_print_cnt)));


    QScrollArea *scrArea = new QScrollArea(history_statistics_lower_panel);
    scrArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrArea->setWidget(chartView);
    scrArea->setFixedHeight(650);
    scrArea->horizontalScrollBar()->setStyleSheet("QScrollBar { height:0px; }");
    scrArea->horizontalScrollBar()->setValue(scrArea->horizontalScrollBar()->maximum());

    QScroller *scroller;
    QScrollerProperties scrPrt;
    scroller =  QScroller::scroller(scrArea->viewport());
    scroller->grabGesture(scrArea->viewport(),QScroller::LeftMouseButtonGesture);
    scrPrt = scroller->scrollerProperties();
    scrPrt.setScrollMetric(QScrollerProperties::OvershootDragResistanceFactor,0.08);
    scrPrt.setScrollMetric(QScrollerProperties::OvershootScrollDistanceFactor,0.05);
    scrPrt.setScrollMetric(QScrollerProperties::DecelerationFactor,1);
    scroller->setScrollerProperties(scrPrt);

    // 添加图表和统计数据到面板
    layout->setContentsMargins(20,10,20,10);
    layout->addStretch();
    layout->addWidget(scrArea);
    //layout->addWidget(sum_label);
    layout->addStretch();
}

void HistoryDialog::DrawLineChartProc(int nPerType, const std::vector<int> &queryVector)
{
    QString PerStr;
    switch (nPerType)
    {
    case 0:
        PerStr = "MM/dd";
        break;
    case 1:
        PerStr = "周";
        break;
    case 2:
        PerStr = "yy/MM";
        break;
    case 3:
        PerStr = "yyyy";
        break;
    default:
        PerStr = "MMdd";
        break;
    }

    // 创建折线序列
    QStringList dateLabels;
    QLineSeries* series = new QLineSeries();
    series->setPen(QPen(QBrush(qRgba(0, 97, 176, 255)), 3, Qt::DashLine));


    for (unsigned short int i = 0; i < queryVector.size(); ++i)
    {
        switch (nPerType)
        {
        case 0:
            dateLabels << m_beginDate.addDays(i).toString(PerStr);
            break;
        case 1:
            dateLabels << QString("%1%2").arg(PerStr).arg(i + 1);
            break;
        case 2:
            dateLabels << m_beginDate.addMonths(i).toString(PerStr);
            break;
        case 3:
            dateLabels << m_beginDate.addYears(i).toString(PerStr);
            break;
        }

        series->append(i, queryVector[i]); // 使用索引作为X坐标‌:ml-citation{ref="1" data="citationList"}
    }

    // 配置数据点样式
    QFont labelFont("微软雅黑", 10, QFont::Bold);
    series->setPointsVisible(true);
    series->setPointLabelsVisible(true);
    series->setPointLabelsFormat("@yPoint");
    series->setPointLabelsFont(labelFont);

    // 创建图表
    QChart* chart = new QChart();
    chart->addSeries(series);
    //chart->setTitle("日期-数量折线图");
    chart->legend()->setVisible(false);//去掉图例
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // X轴配置（日期标签）
    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(dateLabels);
    //axisX->setTitleText("日期");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    QFont axisFont("微软雅黑", 10, QFont::Bold);
    axisX->setLabelsFont(axisFont);

    if(dateLabels.size() > 7)
    {
        axisX->setRange(dateLabels[dateLabels.size()-7], dateLabels.last());
    }
    else
    {
        axisX->setRange(dateLabels.first(), dateLabels.last());
    }

    // Y轴配置
    QValueAxis* axisY = new QValueAxis();
    //axisY->setTitleText("数量");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    quint64 max_cnt = *std::max_element(queryVector.begin(), queryVector.end()) * 1.3;
    axisY->setRange(0, max_cnt);
    //axisY->setTickCount(max_cnt/20 + 1);
    axisY->setLabelFormat("%d");


    // 创建ChartView
    CustomChartView *chartView = new CustomChartView(chart,history_statistics_lower_panel);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    chartView->setFixedHeight(650); // 固定高度

    // 获取 history_statistics_lower_panel 的布局
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(history_statistics_lower_panel->layout());
    if (!layout)
    {
        // 如果没有布局，为该面板创建一个 QVBoxLayout
        layout = new QVBoxLayout(history_statistics_lower_panel);
        history_statistics_lower_panel->setLayout(layout);
    }

    // 清除原有的子控件（如果需要）
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr)
    {
        if (child->widget())
        {
            child->widget()->deleteLater();
        }
        delete child;
    }

    QLabel *sum_label = new QLabel();
//    QList<int> taskTypes = {};
//    QDateTime sel_startTime = QDateTime::fromString("1970-01-01",Qt::ISODate);
//    QDateTime sel_endTime = QDateTime::currentDateTime();
//    int total_print_cnt = print_task_manager.countPrintTasks(taskTypes, sel_startTime, sel_endTime);
//    sum_label->setText(QString("%1%2").arg(tr("总打印数："), QString::number(total_print_cnt)));
//    sum_label->setStyleSheet(SS_Task_Title);

    // 添加图表和统计数据到面板
    layout->addStretch();
    layout->addWidget(chartView);
    layout->addWidget(sum_label);
    layout->addStretch();

    m_total_label = sum_label;
    refreshTotalNumber();
}


void HistoryDialog::refreshTotalNumber()
{
    if(m_total_label)
    {
        QList<int> taskTypes = {};
        QDateTime sel_startTime = QDateTime::fromString("1970-01-01",Qt::ISODate);
        QDateTime sel_endTime = QDateTime::currentDateTime();
        int total_print_cnt = print_task_manager.countPrintTasks(taskTypes, sel_startTime, sel_endTime);
        m_total_label->setText(QString("(%1%2)").arg(tr("总打印数："), QString::number(total_print_cnt)));
        m_total_label->setStyleSheet(SS_Task_Title);
    }
}


