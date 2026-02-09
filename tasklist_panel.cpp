#include "tasklist_panel.h"
#include "public_fun.h"
#include "main_window.h"

TaskListPanel::TaskListPanel(QWidget *parent) :
    QWidget(parent),
    mainWindow(nullptr),
    m_printStateMachine(nullptr)
{
    // 尝试将parent转换为MainWindow指针
    mainWindow = qobject_cast<MainWindow*>(parent);
    qRegisterMetaType<PrintSta>("PrintSta");

    set_layout();
    set_style_sheet();

//    connect(action_btn, &QPushButton::clicked, this, &TaskListPanel::on_action_btn_clicked);
//    connect(delete_btn, &QPushButton::clicked, this, &TaskListPanel::on_delete_btn_clicked);
    connect(max_btn, &QPushButton::clicked, this, &TaskListPanel::on_max_btn_clicked);

    connect(task_table->model(), &QAbstractItemModel::rowsInserted, this, &TaskListPanel::on_row_state_changed);
    connect(task_table->model(), &QAbstractItemModel::rowsRemoved, this, &TaskListPanel::on_row_state_changed);
    connect(task_table->selectionModel(), &QItemSelectionModel::selectionChanged, this, &TaskListPanel::on_row_state_changed);

    connect(task_table->horizontalHeader(), &QHeaderView::sectionClicked, this, &TaskListPanel::on_select_all_item_clicked);

    //connect(task_table,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(on_table_cellclick(QTableWidgetItem*)));
    connect(task_table, &QTableView::clicked,this, &TaskListPanel::on_table_cellclick);

    connect(this, &TaskListPanel::set_print_sta, this, &TaskListPanel::set_print_sta_slot);

    // 设置上下文菜单策略
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTableWidget::customContextMenuRequested, this, &TaskListPanel::show_context_menu);

}


void TaskListPanel::on_select_all_item_clicked(int logicalIndex)
{
    if (logicalIndex != COL_INDEX)
        return;

    if (!task_table || !task_table->selectionModel() || !task_model)
        return;

    QItemSelectionModel *selModel = task_table->selectionModel();
    int totalRows = task_model->rowCount();

    // 检查是否已全部选中
    bool alreadyAllSelected = (selModel->selectedRows().count() == totalRows);

    selModel->clearSelection();

    if (!alreadyAllSelected && totalRows > 0)
    {
        QModelIndex topLeft = task_model->index(0, 0);
        QModelIndex bottomRight = task_model->index(totalRows - 1, task_model->columnCount() - 1);
        QItemSelection selection(topLeft, bottomRight);

        selModel->select(selection, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }

    task_table->viewport()->update();  // 可选：强制刷新界面
}


void TaskListPanel::setPanelMax()
{
    task_table->setVisible(true);
    max_btn->setText("-");
}

void TaskListPanel::setPanelMin()
{
    task_table->setVisible(false);
    max_btn->setText("+");
}

void TaskListPanel::setTitle(QString title)
{
    this->title->setText(title);
}

void TaskListPanel::setIniFilePath(const QString &iniFilePath)
{
    ini_file = iniFilePath;
}

QString TaskListPanel::getTaskdataDir()
{
    return  QCoreApplication::applicationDirPath() + "/task_data/";
}



void TaskListPanel::setHeaderText(QString Col_1, QString Col_2, QString Col_3)
{
    QStringList headers;
    headers << Col_1 << Col_2 << Col_3;

    // 模型前 3 列由外部设置，后续列固定（或也可支持动态扩展）
    headers << tr("信息") << tr("内容");

    if (task_model)
        task_model->setHeaderText(headers);
}

//void TaskListPanel::setButtonText(QString mainBtnText, QString mainBtnCheckedText, QString delBtnText)
//{
//    action_btn_text = mainBtnText;
//    action_btn_checked_text = mainBtnCheckedText;

//    action_btn->setText(mainBtnText);
//    delete_btn->setText(delBtnText);
//}

//void TaskListPanel::setActionButtonReady()
//{
//     QString statStr = (action_btn->isChecked() == true)? "action_btn->isChecked(true)":"action_btn->isChecked(false)";
//     qDebug() << "打印任务按钮Action状态" << statStr;
//     if(action_btn->isChecked())
//     {
//         action_btn->setChecked(false);
//         action_btn->setText(action_btn_text);
//     }
////     if(!ischecked)
////     {
////         action_btn->setChecked(false);
////         action_btn->setText(action_btn_text);
////     }
//}

//void TaskListPanel::execActionButtonClicked()
//{
//     action_btn->click();
//}

void TaskListPanel::setPrintStateMachine(PrintStateMachine *stateMachine)
{
    m_printStateMachine = stateMachine;
}

void TaskListPanel::execDeleteButtonClicked(bool isButtonChecked)
{
    deleteSelectedTasks();
}

void TaskListPanel::deleteSelectedTasks()
{
    // 保存当前焦点窗口
    QWidget *oldFocusWidget = QApplication::focusWidget();

    // 获取被选中行
    if (!task_table || !task_table->selectionModel())
        return;

    // 检查状态机状态，如果不是READY状态，不能删除
    if (m_printStateMachine && ini_file == PRINT_LIST_INI)
    {
        PrintProcessSta currentState = m_printStateMachine->getState();

        qDebug() << "获取到的状态机状态.currentState=====>>>>" << currentState;

        if (currentState != PrintProcessSta::READY)
        {
            DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
            msg_box->setContent(tr("打印任务正在运行，请勿执行删除操作！"));
            msg_box->exec();
            return;
        }
    }

    QModelIndexList selectedIndexes = task_table->selectionModel()->selectedRows();

    if (selectedIndexes.isEmpty())
    {
        DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
        msg_box->setContent(tr("请选择需要删除的任务。"));
        msg_box->exec();
        return;
    }

    if (!task_model)
        return;

    //把edit_panel里的input_panel编辑框的焦点转移到此处
    this->setFocus();

    QList<int> rowsToDelete;
    QStringList taskNamesToDelete;

    for (const QModelIndex &index : selectedIndexes)
    {
        int row = index.row();
        const TaskData &task = task_model->getTask(row);
        
        // 只有PRINT_LIST_INI（打印列表）需要检查任务状态
        // TASK_LIST_INI（任务列表）没有等待状态，可以直接删除
        if (ini_file == PRINT_LIST_INI)
        {
            // 如果任务不是等待状态，跳过不删除
            if (task.status != PRINT_STA_WAIT)
            {
                qDebug() << "跳过非等待状态的任务:" << task.name << "状态:" << task.status;
                continue;
            }
        }
        
        rowsToDelete << row;
        taskNamesToDelete << task.name;
    }

    // 如果没有可删除的任务（仅对PRINT_LIST_INI有效）
    if (rowsToDelete.isEmpty() && ini_file == PRINT_LIST_INI)
    {
        DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
        msg_box->setContent(tr("所选任务中没有等待状态的任务，无法删除。"));
        msg_box->exec();
        
        // 恢复之前的焦点窗口
        if (oldFocusWidget)
        {
            oldFocusWidget->setFocus();
        }
        return;
    }

    // 降序排序，防止删除时行号错位
    std::sort(rowsToDelete.begin(), rowsToDelete.end(), std::greater<int>());

    // 可选：确认框
    DialogMsgBox *msg_box = new DialogMsgBox(ASK_DIALOG);
    msg_box->setContent(tr("该操作无法还原，\n是否确定删除？"));
    if (msg_box->exec() != QDialog::Accepted)
    {
        // 恢复之前的焦点窗口
        if (oldFocusWidget)
        {
            oldFocusWidget->setFocus();
        }
        return;
    }

    // 暂时断开 selectionChanged 信号连接
    disconnect(task_table->selectionModel(), &QItemSelectionModel::selectionChanged,
               this, &TaskListPanel::on_row_state_changed);

    disconnect(task_table->model(), &QAbstractItemModel::rowsRemoved,
               this, &TaskListPanel::on_row_state_changed);

    //===== 先行删除任务队列记录 + 模型任务 =====
    for (int i = 0; i < rowsToDelete.size(); ++i)
    {
        const QString &taskName = taskNamesToDelete[i];
    }
    
    task_model->removeTasks(rowsToDelete);
    task_table->clearSelection();
    task_table->viewport()->update();

    // ===== INI 文件删除任务记录 =====
    QSettings task_list_ini(ini_file, QSettings::IniFormat);

    // 1. 删除 TaskArrange 排序记录
    task_list_ini.beginGroup("TaskArrange");
    QString arrange_str = task_list_ini.value("Arrange", "").toString();
    QStringList arrange_str_list = arrange_str.split(",", Qt::SkipEmptyParts);
    for (const QString &taskName : taskNamesToDelete)
    {
        arrange_str_list.removeAll(taskName);
    }
    task_list_ini.setValue("Arrange", arrange_str_list.join(","));
    task_list_ini.endGroup();

    // 2. 删除 TaskList 中的键
    task_list_ini.beginGroup("TaskList");
    for (const QString &taskName : taskNamesToDelete)
    {
        task_list_ini.remove(taskName);
    }
    task_list_ini.endGroup();

    // 删除任务完成后，恢复信号连接
    connect(task_table->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &TaskListPanel::on_row_state_changed);

    connect(task_table->model(), &QAbstractItemModel::rowsRemoved,
            this, &TaskListPanel::on_row_state_changed);
    on_row_state_changed();

    // 恢复之前的焦点窗口
    if (oldFocusWidget)
    {
        oldFocusWidget->setFocus();
    }
}

void TaskListPanel::loadTaskListFromFile()
{
    if (ini_file.isEmpty())
        return;

    task_model->clearAllTasks(); // 清空旧任务

    QSettings task_list_ini(ini_file, QSettings::IniFormat);

    task_list_ini.beginGroup("TaskArrange");
    QString arrange_str = task_list_ini.value("Arrange", "").toString();
    task_list_ini.endGroup();

    QStringList keys = arrange_str.split(",", Qt::SkipEmptyParts);
    QList<TaskData> loadedTasks;

    task_list_ini.beginGroup("TaskList");
    for (const QString &key : keys)
    {
        QString task_text = task_list_ini.value(key, "").toString();
        qDebug() << "loadTaskListFromFile======>>>>>>>>>>>>>>>>>>>>" << task_text;
        if (!task_text.isEmpty())
        {
            TaskData task = buildTaskFromText(key, task_text);
            if (!task.name.isEmpty())
                loadedTasks << task;
        }
    }
    task_list_ini.endGroup();

    task_model->addTasks(loadedTasks, false);

    if (ini_file == PRINT_LIST_INI)
    {
        QStringList waitingTasks = extractWaitingTasksFromIni(ini_file);
    }
}

void TaskListPanel::setIntervalSymbol(const QString &symbol)
{
    interval_symbol = symbol;
}

QStringList TaskListPanel::addTextListToTaskTable(const QStringList &textList, TaskType taskType, bool isFront)
{
    try {
        // 添加空指针检查
        if (!task_model || !task_table)
        {
            return QStringList();
        }

        int row_cnt = task_model->rowCount();

        if(textList.size() + row_cnt > MAX_TASKS)
        {
            DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
            msg_box->setContent(QString("%1%2").arg(tr("超过打印列表最大任务数")).arg(MAX_TASKS));
            msg_box->exec();
            if (task_table)
            {
                task_table->clearSelection();
            }
            return QStringList();
        }

        QString date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        QString date_time_num = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");


        // 检查ini_file是否有效
        if (ini_file.isEmpty())
        {
            return QStringList();
        }

        QStringList task_list = textList;
        if(isFront)
        {
            std::reverse(task_list.begin(), task_list.end());
        }

        QList<TaskData> tasks_to_add;
        // 在内存中收集所有需要写入的数据，减少IO操作
        QMap<QString, QString> new_task_data; // 存储新任务数据
        QStringList arrange_str_list; // 存储排列列表

        // 先读取现有的排列列表
        QSettings task_list_ini_read(ini_file, QSettings::IniFormat);
        task_list_ini_read.beginGroup("TaskArrange");
        QString arrange_str = task_list_ini_read.value("Arrange", "").toString();
        if(!arrange_str.isEmpty())
        {
            arrange_str_list = arrange_str.split(",");
        }
        task_list_ini_read.endGroup();

        qDebug() << "[TIME] addTextListToTaskTable.写入INI开始:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

        for (int i=0; i<task_list.size(); i++)
        {
            QString s_num = QString("%1").arg(serial_number(),5,10,QChar('0'));
            QString task_name = QString("Task_%1%2").arg(date_time_num, s_num);

            //加上状态，类型，时间
            task_list[i].append(SPLIT_SYMBOL);
            task_list[i].append(QString::number(0));
            task_list[i].append(SPLIT_SYMBOL);
            task_list[i].append(QString::number(taskType));
            task_list[i].append(SPLIT_SYMBOL);
            task_list[i].append(date_time);
            task_list[i].append(SPLIT_SYMBOL);
            task_list[i].append(task_name);

            QString updated_taskText = task_list[i];

            // 在内存中存储新任务数据
            new_task_data[task_name] = task_list[i];

            // 在内存中构建新的排列列表
            if(isFront)
            {
                arrange_str_list.prepend(task_name);
            }
            else
            {
                arrange_str_list.append(task_name);
            }

            qDebug() << "addTextListToTaskTable.updated_taskText====>>>>" << updated_taskText;
            // 构造 TaskData（保持格式）
            TaskData task = buildTaskFromText(task_name, updated_taskText);
            if (!task.name.isEmpty())
            {
                tasks_to_add << task;
            }
        }

        // 批量写入INI文件，减少IO操作次数
        QSettings task_list_ini(ini_file, QSettings::IniFormat);

        // 写入排列列表
        task_list_ini.beginGroup("TaskArrange");
        task_list_ini.setValue("Arrange", arrange_str_list.join(","));
        task_list_ini.endGroup();

        // 批量写入所有新任务数据
        task_list_ini.beginGroup("TaskList");
        for (auto it = new_task_data.constBegin(); it != new_task_data.constEnd(); ++it)
        {
            task_list_ini.setValue(it.key(), it.value());
        }
        task_list_ini.endGroup();

        qDebug() << "[TIME] addTextListToTaskTable.写入INI结束:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

        // 添加空指针检查
        if (!task_model || !task_table)
        {
            return QStringList();
        }

        // 插入任务到模型,批量插入
        task_model->addTasks(tasks_to_add, isFront);

//        int new_row_cnt = task_model->rowCount();
//        task_table->clearSelection();

        // 获取selectionModel并检查是否为空
        QItemSelectionModel *selectionModel = task_table->selectionModel();
        if (!selectionModel)
        {
            return QStringList();
        }

//        创建任务去掉选取任务列表操作

        if(TaskType::LOCAL_TASK == taskType)
        {
            int new_row_cnt = task_model->rowCount();
            task_table->clearSelection();
            if (isFront)
            {
                QModelIndex topLeft = task_model->index(0, 0);
                QModelIndex bottomRight = task_model->index(new_row_cnt - row_cnt - 1, task_model->columnCount() - 1);

                QItemSelection range(topLeft, bottomRight);
                //selectionModel->select(range, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

                task_table->scrollTo(topLeft, QAbstractItemView::PositionAtTop);
            }
            else
            {
                QModelIndex topLeft = task_model->index(row_cnt, 0);
                QModelIndex bottomRight = task_model->index(new_row_cnt - 1, task_model->columnCount() - 1);

                QItemSelection range(topLeft, bottomRight);
                //selectionModel->select(range, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

                task_table->scrollToBottom();
            }
        }


        return  task_list;

    }
    catch (const std::exception &e)
    {
        Logger::instance().log("Exception in addTextListToTaskTable: ", e.what());

        return QStringList();
    }
    catch (...)
    {
        Logger::instance().log("Unknown exception in addTextListToTaskTable","");

        return QStringList();
    }

}


void TaskListPanel::addTaskListToPrintTable(const QStringList &taskList, bool isFront,bool isSelect)
{
    try {
        // 检查task_model是否为空
        if (!task_model) {
            qDebug() << "Error: task_model is null in addTaskListToPrintTable";
            return;
        }

        // 检查task_table是否为空
        if (!task_table) {
            qDebug() << "Error: task_table is null in addTaskListToPrintTable";
            return;
        }

        int row_cnt = task_model->rowCount();
        if (taskList.size() + row_cnt > MAX_TASKS)
        {
            DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
            if (msg_box) {
                msg_box->setContent(QString("%1%2").arg(tr("超过打印列表最大任务数")).arg(MAX_TASKS));
                msg_box->exec();
                delete msg_box; // 释放内存
            }
            task_table->clearSelection();
            return;
        }

        QString date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        QString date_time_num = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");

        // 检查ini_file是否为空或无效
        if (ini_file.isEmpty()) {
            qDebug() << "Error: ini_file path is empty in addTaskListToPrintTable";
            return;
        }

        QStringList print_list = taskList;

        if (isFront)
            std::reverse(print_list.begin(), print_list.end());

        QList<TaskData> tasks_to_add;

        // 在内存中收集所有需要写入的数据，减少IO操作
        QMap<QString, QString> new_task_data; // 存储新任务数据
        QStringList arrange_str_list; // 存储排列列表
        QStringList temp_new_task_names; // 临时存储新任务名，用于任务队列

        // 先读取现有的排列列表
        QSettings task_list_ini_read(ini_file, QSettings::IniFormat);
        task_list_ini_read.beginGroup("TaskArrange");
        QString arrange_str = task_list_ini_read.value("Arrange").toString();
        arrange_str_list = arrange_str.split(",", Qt::SkipEmptyParts);
        task_list_ini_read.endGroup();

        for (int i = 0; i < print_list.size(); ++i)
        {
            QString print_str = print_list[i];
            QStringList print_str_split = print_str.split(SPLIT_SYMBOL);
            if (print_str_split.size() < TaskTextPos::TASK_MAX_ITEM)
                continue;

            // 检查是否重复任务名
            if (!allow_duplicate_print_task)
            {
                bool has_same_task = false;
                for (int row = 0; row < task_model->rowCount(); ++row)
                {
                    //QString existing_task_name = task_model->getTask(row).name;
                    QString existing_task_name = task_model->getTask(row).fullText.right(24);
                    if (print_str_split[TaskTextPos::TASK_NAME] == existing_task_name)
                    {
                        has_same_task = true;
                        break;
                    }
                }
                if (has_same_task)
                    continue;
            }

            // 替换时间和状态
            print_str_split[TaskTextPos::TASK_TIME] = date_time;
            print_str_split[TaskTextPos::TASK_STA] = QString::number(PrintSta::PRINT_STA_WAIT);

            // 生成唯一任务名
            QString s_num = QString("%1").arg(serial_number(), 5, 10, QChar('0'));
            QString print_name = QString("Task_%1%2").arg(date_time_num, s_num);
            //不要修改任务名，否则找不到任务的图片路径
            //print_str_split[TaskTextPos::TASK_NAME] = print_name;

            QString updated_taskText = print_str_split.join(SPLIT_SYMBOL);

            // 在内存中存储新任务数据
            new_task_data[print_name] = updated_taskText;

            // 构造 TaskData（保持格式）
            TaskData task = buildTaskFromText(print_name, updated_taskText);

            // 在内存中构建新的排列列表
            if (isFront)
            {
                arrange_str_list.prepend(print_name);
                temp_new_task_names.prepend(print_name); // 记录新任务名，用于任务队列

                if (!task.name.isEmpty())
                {
                    tasks_to_add.prepend(task);
                }

            }
            else
            {
                arrange_str_list.append(print_name);
                temp_new_task_names.append(print_name); // 记录新任务名，用于任务队列

                if (!task.name.isEmpty())
                {
                    tasks_to_add.append(task);
                }
            }

//            // 构造 TaskData（保持格式）
//            TaskData task = buildTaskFromText(print_name, updated_taskText);
//            if (!task.name.isEmpty())
//            {
//                tasks_to_add << task;
//            }
        }

        // 批量写入INI文件，减少IO操作次数
        QSettings task_list_ini(ini_file, QSettings::IniFormat);

        // 写入排列列表
        task_list_ini.beginGroup("TaskArrange");
        task_list_ini.setValue("Arrange", arrange_str_list.join(","));
        task_list_ini.endGroup();

        // 批量写入所有新任务数据
        task_list_ini.beginGroup("TaskList");
        for (auto it = new_task_data.constBegin(); it != new_task_data.constEnd(); ++it)
        {
            task_list_ini.setValue(it.key(), it.value());
        }
        task_list_ini.endGroup();

        // 加入任务队列（批量处理）
//        for (const QString &print_name : temp_new_task_names)
//        {
//            if (isFront)
//                print_task_queue.enqueueFront(print_name);
//            else
//                print_task_queue.enqueue(print_name);
//        }

        // 插入任务到模型,批量插入
        task_model->addTasks(tasks_to_add, isFront);

        // 选中新插入行
//        int new_row_cnt = task_model->rowCount();
//        task_table->clearSelection();

        // 获取selectionModel并检查是否为空
        QItemSelectionModel *selectionModel = task_table->selectionModel();
        if (!selectionModel)
        {
            return;
        }

        // 检查tasks_to_add是否为空
        if (tasks_to_add.isEmpty())
        {
            return;
        }

        if(isSelect)
        {
            // 选中新插入行
            int new_row_cnt = task_model->rowCount();
            task_table->clearSelection();
            if (isFront)
            {
                QModelIndex top = task_model->index(0, 0);
                QModelIndex bottom = task_model->index(tasks_to_add.size() - 1, task_model->columnCount() - 1);
                QItemSelection selection(top, bottom);
                selectionModel->select(selection, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
                task_table->scrollTo(top, QAbstractItemView::PositionAtTop);
            }
            else
            {
                QModelIndex top = task_model->index(new_row_cnt - tasks_to_add.size(), 0);
                QModelIndex bottom = task_model->index(new_row_cnt - 1, task_model->columnCount() - 1);
                QItemSelection selection(top, bottom);
                selectionModel->select(selection, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
                task_table->scrollToBottom();
            }
        }

    }
    catch (const std::exception &e)
    {
        Logger::instance().log("Exception in addTaskListToPrintTable: ", e.what());
    }
    catch (...)
    {
        Logger::instance().log("Unknown exception in addTaskListToPrintTable","");
    }
}


TaskData TaskListPanel::buildTaskFromText(const QString &task_name, const QString &task_text)
{
    QStringList fields = task_text.split(SPLIT_SYMBOL);
    if (fields.size() < TaskTextPos::TASK_MAX_ITEM)
        return TaskData{}; // 无效

    TaskData task;
    task.name = task_name;
    task.status = static_cast<PrintSta>(fields[TaskTextPos::TASK_STA].toInt());
    task.statusText = sta_display_str(task.status);
    task.info = QString("%1 %2\n%3")
                    .arg(fields[TaskTextPos::TASK_TEMP],
                         task_type_display_str(fields[TaskTextPos::TASK_TYPE].toInt()),
                         fields[TaskTextPos::TASK_TIME]);


    QStringList contentList = fields.mid(TaskTextPos::TASK_TEXT, MAX_INPUT_PANEL);
    contentList.removeAll("");
    QString contentText = contentList.join(interval_symbol);
    QString slotText = QString(tr("槽盒：%1")).arg(slots_display_str(fields[TaskTextPos::TASK_SLOTS]));
    task.content = QString("%1\n%2").arg(contentText, slotText);

    task.fullText = task_text;

    return task;
}

void TaskListPanel::add_task_to_table(const QString &task_name, const QString &task_text, bool is_front)
{

    TaskData task = buildTaskFromText(task_name, task_text);
    if (task.name.isEmpty())
        return;

    task_model->addTasks({task}, is_front);
}

int TaskListPanel::getSelectedRowCnt()
{
    if (!task_table || !task_table->selectionModel())
        return 0;
    return task_table->selectionModel()->selectedRows().count();
}


QStringList TaskListPanel::getSelectedTaskList()
{
    QStringList taskList;

    if (!task_table || !task_table->selectionModel() || !task_model)
        return taskList;

    QModelIndexList selectedIndexes = task_table->selectionModel()->selectedRows();
    for (const QModelIndex &index : selectedIndexes)
    {
        int row = index.row();
        const TaskData &task = task_model->getTask(row);
        taskList << task.fullText;
    }

    return taskList;
}

QStringList TaskListPanel::addImageTaskToTaskTable(const QString &imageName, QByteArray imageData, bool isFront,bool isSelect)
{
    //int row_cnt = task_table->rowCount();
    int row_cnt = task_model->rowCount();
    if(1 + row_cnt > MAX_TASKS)
    {
        DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
        msg_box->setContent(QString("%1%2").arg(tr("超过打印列表最大任务数")).arg(MAX_TASKS));
        msg_box->exec();
        task_table->clearSelection();
        return QStringList();
    }

    QString date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString date_time_num = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");

    QSettings task_list_ini(ini_file, QSettings::IniFormat);
    QStringList task_text_list;

    //初始化数据
    for (int i=0; i<TaskTextPos::TASK_MAX_ITEM; i++)
    {
        task_text_list.append(QString());
    }

    QString s_num = QString("%1").arg(serial_number(),5,10,QChar('0'));
    QString task_name = QString("Task_%1%2").arg(date_time_num, s_num);

    task_text_list[TaskTextPos::TASK_TEXT] = imageName;
    task_text_list[TaskTextPos::TASK_STA] = QString::number(0);
    task_text_list[TaskTextPos::TASK_TYPE] = QString::number(TaskType::REMOTE_IMAGE_TASK);
    task_text_list[TaskTextPos::TASK_TIME] = date_time;
    task_text_list[TaskTextPos::TASK_NAME] = task_name;

    //保存图片
//    if(!image.isNull())
//    {
//        image.save(QString("%1/%2.png").arg(TASK_LIST_DATA_DIR, task_name));
//    }
    saveTaskImage(task_name,imageData);
    //写入文件
    task_list_ini.beginGroup("TaskArrange");
    QString arrange_str = task_list_ini.value("Arrange", "").toString();
    QStringList arrange_str_list;
    if(arrange_str.size() != 0)
    {
        arrange_str_list = arrange_str.split(",");
    }

    if(isFront)
    {
        arrange_str_list.prepend(task_name);
    }
    else
    {
        arrange_str_list.append(task_name);
    }

    task_list_ini.setValue("Arrange", arrange_str_list.join(","));
    task_list_ini.endGroup();

    QString task_text = task_text_list.join(SPLIT_SYMBOL);
    task_list_ini.beginGroup("TaskList");
    task_list_ini.setValue(task_name, task_text);
    task_list_ini.endGroup();

    //插入列表
    add_task_to_table(task_name, task_text, isFront);

    qDebug() << "Add ImageTask:" << task_text;

    QStringList task_str_list ;
    task_str_list << task_text;
    //选中新加行
    int new_row_cnt = task_model->rowCount();
    task_table->clearSelection();


    if(isSelect)
    {
        if (isFront)
        {
            QModelIndex topLeft = task_model->index(0, 0);
            QModelIndex bottomRight = task_model->index(new_row_cnt - row_cnt - 1, task_model->columnCount() - 1);

            QItemSelection range(topLeft, bottomRight);
            task_table->selectionModel()->select(range, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

            task_table->scrollTo(topLeft, QAbstractItemView::PositionAtTop);
        }
        else
        {
            QModelIndex topLeft = task_model->index(row_cnt, 0);
            QModelIndex bottomRight = task_model->index(new_row_cnt - 1, task_model->columnCount() - 1);

            QItemSelection range(topLeft, bottomRight);
            task_table->selectionModel()->select(range, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
            task_table->scrollToBottom();
        }
    }

    return task_str_list;
}

QStringList TaskListPanel::addImageTaskToTaskTable(const QString &imageName, const QImage &image, bool isFront)
{
    QString date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString date_time_num = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
    //int row_cnt = task_table->rowCount();
    int row_cnt = task_model->rowCount();

    if(1 + row_cnt > MAX_TASKS)
    {
        DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
        msg_box->setContent(QString("%1%2").arg(tr("超过打印列表最大任务数")).arg(MAX_TASKS));
        msg_box->exec();
        return QStringList();
    }

    QSettings task_list_ini(ini_file, QSettings::IniFormat);
    QStringList task_text_list;

    //初始化数据
    for (int i=0; i<TaskTextPos::TASK_MAX_ITEM; i++)
    {
        task_text_list.append(QString());
    }

    QString s_num = QString("%1").arg(serial_number(),5,10,QChar('0'));
    QString task_name = QString("Task_%1%2").arg(date_time_num, s_num);

    task_text_list[TaskTextPos::TASK_TEXT] = imageName;
    task_text_list[TaskTextPos::TASK_STA] = QString::number(0);
    task_text_list[TaskTextPos::TASK_TYPE] = QString::number(TaskType::REMOTE_IMAGE_TASK);
    task_text_list[TaskTextPos::TASK_TIME] = date_time;
    task_text_list[TaskTextPos::TASK_NAME] = task_name;

    //保存图片
//    if(!image.isNull())
//    {
//        image.save(QString("%1/%2.png").arg(TASK_LIST_DATA_DIR, task_name));
//    }

    saveTaskImage(task_name,image);

    //写入文件
    task_list_ini.beginGroup("TaskArrange");
    QString arrange_str = task_list_ini.value("Arrange", "").toString();
    QStringList arrange_str_list;
    if(arrange_str.size() != 0)
    {
        arrange_str_list = arrange_str.split(",");
    }

    if(isFront)
    {
        arrange_str_list.prepend(task_name);
    }
    else
    {
        arrange_str_list.append(task_name);
    }

    task_list_ini.setValue("Arrange", arrange_str_list.join(","));
    task_list_ini.endGroup();

    QString task_text = task_text_list.join(SPLIT_SYMBOL);
    task_list_ini.beginGroup("TaskList");
    task_list_ini.setValue(task_name, task_text);
    task_list_ini.endGroup();

    //插入列表
    add_task_to_table(task_name, task_text, isFront);

    qDebug() << "Add ImageTask:" << task_text;

    QStringList task_str_list ;
    task_str_list << task_text;

    //选中新加行

    int new_row_cnt = task_model->rowCount();
    task_table->clearSelection();

    if (isFront)
    {
        QModelIndex topLeft = task_model->index(0, 0);
        QModelIndex bottomRight = task_model->index(new_row_cnt - row_cnt - 1, task_model->columnCount() - 1);

        QItemSelection range(topLeft, bottomRight);
        task_table->selectionModel()->select(range, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

        task_table->scrollTo(topLeft, QAbstractItemView::PositionAtTop);
    }
    else
    {
        QModelIndex topLeft = task_model->index(row_cnt, 0);
        QModelIndex bottomRight = task_model->index(new_row_cnt - 1, task_model->columnCount() - 1);

        QItemSelection range(topLeft, bottomRight);
        task_table->selectionModel()->select(range, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
        task_table->scrollToBottom();
    }

    return task_str_list;
}

bool TaskListPanel::saveTaskImage(QString taskName, QByteArray imageData)
{
    // 创建文件路径
    QString datePart = taskName.mid(5, 8); // 从第 5 个字符开始提取 8 个字符
    QString filePath = getTaskdataDir() + datePart + "/";
    QString fullFileName = filePath + taskName + ".png";

    // 创建目录对象
    QDir dir(filePath);

    // 检查并创建目录
    if (!dir.exists()) {
        if (!dir.mkpath(filePath)) {
            qWarning() << "Failed to create directory:" << filePath;
            return false;
        }
    }

    // 打开文件以写入
    QFile file(fullFileName);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for writing:" << fullFileName;
        return false;
    }

    // 写入数据并关闭文件
    file.write(imageData);
    file.close();

    qDebug() << "Image saved successfully to:" << fullFileName;
    return true;
}

bool TaskListPanel::saveTaskImage(QString taskName, QImage imageData)
{
    // 创建文件路径
    QString datePart = taskName.mid(5, 8); // 从第 5 个字符开始提取 8 个字符
    QString filePath = getTaskdataDir() + datePart + "/";
    QString fullFileName = filePath + taskName + ".png";

    // 创建目录对象
    QDir dir(filePath);

    // 检查并创建目录
    if (!dir.exists()) {
        if (!dir.mkpath(filePath)) {
            qWarning() << "Failed to create directory:" << filePath;
            return false;
        }
    }

    // 打开文件以写入
//    QFile file(fullFileName);
//    if (!file.open(QIODevice::WriteOnly)) {
//        qWarning() << "Failed to open file for writing:" << fullFileName;
//        return false;
//    }

//    // 写入数据并关闭文件
//    file.write(imageData);
//    file.close();

    imageData.save(fullFileName);

    qDebug() << "Image saved successfully to:" << fullFileName;
    return true;
}

void TaskListPanel::increaseTaskSta(QString taskName)
{
    for (int i = 0; i < task_model->rowCount(); ++i)
    {
        TaskData task = task_model->getTask(i);
        if (task.name == taskName)
        {
            // 1. 更新模型中的状态
            PrintSta newStatus = static_cast<PrintSta>(static_cast<int>(task.status) + 1);
            task_model->updateTaskStatus(i, newStatus);

            // 2. 更新 INI 文件中对应任务状态（修改 TASK_STA 字段）
            QSettings task_list_ini(ini_file, QSettings::IniFormat);
            task_list_ini.beginGroup("TaskList");

            QStringList fields = task.fullText.split(SPLIT_SYMBOL);
            if (fields.size() >= TASK_MAX_ITEM)
            {
                fields[TASK_STA] = QString::number(static_cast<int>(newStatus));
                QString updatedTaskText = fields.join(SPLIT_SYMBOL);
                task_list_ini.setValue(taskName, updatedTaskText);

                //更新 TaskData 中的 fullText 内容（保持同步）
                TaskData updatedTask = task;
                updatedTask.status = newStatus;
                updatedTask.statusText = QString::number(static_cast<int>(newStatus));
                updatedTask.fullText = updatedTaskText;
                task_model->updateTaskData(i, updatedTask);
            }

            task_list_ini.endGroup();

            break;
        }
    }
}


//bool TaskListPanel::getPrintActionCheckedState()
//{
//    return action_btn->isChecked();
//}

//bool TaskListPanel::getPrintTask(QString &printName, QString &printTask)
//{
//    QMutexLocker locker(&m_mutex);

//    if(!action_btn->isChecked())
//        return false;

//    bool is_get_task  = false;

//    QSettings task_list_ini(ini_file, QSettings::IniFormat);
//    task_list_ini.beginGroup("TaskArrange");
//    QString arrange_str = task_list_ini.value("Arrange", "").toString();
//    task_list_ini.endGroup();

//    if(arrange_str == "")
//    {
//        return is_get_task;
//    }

//    QStringList print_names = arrange_str.split(",");

//    //寻找第一个等待状态的打印任务
//    task_list_ini.beginGroup("TaskList");
//    for (const QString &key : print_names)
//    {
//        QString task_str = task_list_ini.value(key, "").toString();
//        if(task_str.isEmpty())
//        {
//            continue;
//        }
//        QStringList task_str_list = task_str.split(SPLIT_SYMBOL);
//        if(task_str_list.size() < TaskTextPos::TASK_MAX_ITEM)
//        {
//            continue;
//        }
//        if(task_str_list[TaskTextPos::TASK_STA].toInt() == PrintSta::PRINT_STA_WAIT)
//        {
//            printName = key;
//            printTask = task_str;
//            is_get_task = true;
//            break;
//        }
//        else
//        {
//            is_get_task = false;
//        }
//    }

//    task_list_ini.endGroup();

//    return is_get_task;
//}

QStringList TaskListPanel::extractWaitingTasksFromIni(const QString& iniPath)
{
    QMutexLocker locker(&m_mutex);

    QStringList waitingList;
    QSettings task_list_ini(iniPath, QSettings::IniFormat);

    task_list_ini.beginGroup("TaskArrange");
    QString arrange_str = task_list_ini.value("Arrange", "").toString();
    task_list_ini.endGroup();

    QStringList task_names = arrange_str.split(",");
    task_list_ini.beginGroup("TaskList");

    for (const QString &key : task_names)
    {
        QString task_str = task_list_ini.value(key, "").toString();
        if (task_str.isEmpty())
            continue;

        QStringList task_str_list = task_str.split(SPLIT_SYMBOL);
        if (task_str_list.size() < TaskTextPos::TASK_MAX_ITEM)
            continue;

        if (task_str_list[TaskTextPos::TASK_STA].toInt() == PrintSta::PRINT_STA_WAIT)
        {
            waitingList << key;
        }
    }

    task_list_ini.endGroup();
    return waitingList;
}


QStringList TaskListPanel::getAllWaitingTaskNamesFromModel()
{
    QMutexLocker locker(&m_mutex);

    QStringList waitingList;
    if (!task_model) return waitingList;

    // 从内存中的task_model获取所有任务
    int rowCount = task_model->rowCount();
    for (int row = 0; row < rowCount; ++row)
    {
        TaskData task = task_model->getTask(row);
        if (task.status == PrintSta::PRINT_STA_WAIT)
        {
            waitingList << task.name;
        }
    }

    return waitingList;
}

QStringList TaskListPanel::getAllWaitingTaskNames()
{
    // 使用新方法从内存获取，减少文件I/O
    return getAllWaitingTaskNamesFromModel();
}


bool TaskListPanel::isTaskStillWaitingFromModel(const QString& taskName)
{
    if (!task_model) return false;

    // 遍历查找首个 WAIT 状态的任务（逻辑上第一个应该执行的任务）
    QString firstWaitTask;
    int rowCount = task_model->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        TaskData task = task_model->getTask(row);
        if (task.status == PrintSta::PRINT_STA_WAIT) {
            firstWaitTask = task.name;
            break;
        }
    }

    // 判断当前任务是否就是首个 WAIT 状态任务
    return !firstWaitTask.isEmpty() && firstWaitTask == taskName;
}

QString TaskListPanel::getFirstWaitingTaskNameFromModel()
{
    if (!task_model) return QString();

    // 遍历查找首个 WAIT 状态的任务
    int rowCount = task_model->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        TaskData task = task_model->getTask(row);
        if (task.status == PrintSta::PRINT_STA_WAIT) {
            return task.name;
        }
    }

    return QString();
}

bool TaskListPanel::isTaskStillWaiting(const QString& taskName)
{
    // 使用新方法从内存获取，减少文件I/O
    return isTaskStillWaitingFromModel(taskName);
}

TaskRecoverStatus TaskListPanel::getTaskRecoverStatus(const QString& taskName)
{
    QSettings task_list_ini(ini_file, QSettings::IniFormat);

    // 1. 获取排序列表
    task_list_ini.beginGroup("TaskArrange");
    QString arrangeStr = task_list_ini.value("Arrange", "").toString();
    task_list_ini.endGroup();

    if (arrangeStr.isEmpty()) return TaskRecoverStatus::NotExistOrFinished;

    QStringList arrangedTasks = arrangeStr.split(",", Qt::SkipEmptyParts);

    // 2. 遍历查找第一个 WAIT 状态的任务和 taskName 状态
    QString firstWaitTask;
    bool foundTask = false;
    bool isTaskWait = false;

    task_list_ini.beginGroup("TaskList");
    for (const QString& task : arrangedTasks) {
        QString task_str = task_list_ini.value(task, "").toString();
        if (task_str.isEmpty()) continue;

        QStringList parts = task_str.split(SPLIT_SYMBOL);
        if (parts.size() < TaskTextPos::TASK_MAX_ITEM) continue;

        int status = parts[TaskTextPos::TASK_STA].toInt();

        if (status == PrintSta::PRINT_STA_WAIT && firstWaitTask.isEmpty())
            firstWaitTask = task;

        if (task == taskName) {
            foundTask = true;
            isTaskWait = (status == PrintSta::PRINT_STA_WAIT);
        }

        if (!firstWaitTask.isEmpty() && foundTask) break; // 都找到了就退出
    }
    task_list_ini.endGroup();

    if (!foundTask || !isTaskWait)
        return TaskRecoverStatus::NotExistOrFinished;

    if (firstWaitTask == taskName)
        return TaskRecoverStatus::WaitingAndIsFirst;

    return TaskRecoverStatus::WaitingNotFirst;
}

// 使用新方法从内存直接获取状态，避免文件I/O
TaskRecoverStatus TaskListPanel::getTaskRecoverStatusFromModel(const QString& taskName)
{
    if (!task_model) return TaskRecoverStatus::NotExistOrFinished;

    // 遍历查找第一个 WAIT 状态的任务和 taskName 状态
    QString firstWaitTask;
    bool foundTask = false;
    bool isTaskWait = false;

    int rowCount = task_model->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        TaskData task = task_model->getTask(row);
        
        if (task.status == PrintSta::PRINT_STA_WAIT && firstWaitTask.isEmpty())
            firstWaitTask = task.name;

        if (task.name == taskName) {
            foundTask = true;
            isTaskWait = (task.status == PrintSta::PRINT_STA_WAIT);
        }

        if (!firstWaitTask.isEmpty() && foundTask) break; // 都找到了就退出
    }

    if (!foundTask || !isTaskWait)
        return TaskRecoverStatus::NotExistOrFinished;

    if (firstWaitTask == taskName)
        return TaskRecoverStatus::WaitingAndIsFirst;

    return TaskRecoverStatus::WaitingNotFirst;
}

// 从print_list_panel的TaskTableModel中获取任务恢复状态
TaskRecoverStatus TaskListPanel::getTaskRecoverStatusFromPrintListModel(const QString& taskName)
{
    if (!task_model) return TaskRecoverStatus::NotExistOrFinished;

    // 遍历查找第一个 WAIT 状态的任务和 taskName 状态
    QString firstWaitTask;
    bool foundTask = false;
    bool isTaskWait = false;

    int rowCount = task_model->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        TaskData task = task_model->getTask(row);
        
        if (task.status == PrintSta::PRINT_STA_WAIT && firstWaitTask.isEmpty())
            firstWaitTask = task.name;

        if (task.name == taskName) {
            foundTask = true;
            isTaskWait = (task.status == PrintSta::PRINT_STA_WAIT);
        }

        if (!firstWaitTask.isEmpty() && foundTask) break; // 都找到了就退出
    }

    if (!foundTask || !isTaskWait)
        return TaskRecoverStatus::NotExistOrFinished;

    if (firstWaitTask == taskName)
        return TaskRecoverStatus::WaitingAndIsFirst;

    return TaskRecoverStatus::WaitingNotFirst;
}

QStringList TaskListPanel::getTaskArrangeOrderFromModel() const
{
    QStringList arrangeList;
    if (!task_model) return arrangeList;

    // 从task_model中获取任务顺序，按行顺序收集任务名称
    int rowCount = task_model->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        TaskData task = task_model->getTask(row);
        arrangeList.append(task.name);
    }

    return arrangeList;
}

QStringList TaskListPanel::getTaskArrangeOrder() const
{
    QSettings task_list_ini(ini_file, QSettings::IniFormat);
    task_list_ini.beginGroup("TaskArrange");
    QString arrange_str = task_list_ini.value("Arrange", "").toString();
    task_list_ini.endGroup();
    return arrange_str.split(",");
}

int TaskListPanel::getListCount()
{
    //return task_table->rowCount();
    return task_model->rowCount();
}


void TaskListPanel::setPrintTaskState(const QString &printName,PrintSta sta)
{
    QMutexLocker locker(&m_mutex);
    emit set_print_sta(printName, sta);
}

void TaskListPanel::setPrintTaskFinished(const QString &printName, QString &taskName)
{
    QMutexLocker locker(&m_mutex);

    // 添加空指针检查
    if (!task_model)
    {
        qDebug() << "task_model is null in setPrintTaskFinished";
        return;
    }

    for (int i = 0; i < task_model->rowCount(); ++i)
    {
        TaskData task = task_model->getTask(i);

        if (task.name == printName)
        {
            // 1. 更新模型状态和任务名
            int rowToDelete = task_model->updateTaskFinished(i);
            if (rowToDelete == -1)
            {
                qDebug() << "Failed to update task status for:" << printName;
                return;
            }

            //先删除视图记录
            if (task_model && rowToDelete >= 0 && rowToDelete < task_model->rowCount())
            {
                task_model->removeTasks({ rowToDelete });
            }

            // 2. 更新 INI 文件
            if (!ini_file.isEmpty()) {
                QSettings task_list_ini(ini_file, QSettings::IniFormat);
                
                // ---- 更新 TaskList 数据 ----
                task_list_ini.beginGroup("TaskList");
                QString print_str = task_list_ini.value(printName, "").toString();
                QStringList print_str_list = print_str.split(SPLIT_SYMBOL);
                if(print_str_list.size() >= TaskTextPos::TASK_MAX_ITEM)
                {
                    taskName = print_str_list[TaskTextPos::TASK_NAME];
                }
                task_list_ini.remove(printName);
                task_list_ini.endGroup();

                // ---- 更新 TaskArrange 顺序列表 ----
                task_list_ini.beginGroup("TaskArrange");
                QString arrange_str = task_list_ini.value("Arrange", "").toString();
                QStringList print_names = arrange_str.split(",");
                print_names.removeAll(printName);
                task_list_ini.setValue("Arrange", print_names.join(","));
                task_list_ini.endGroup();
            }
            else
            {
                qDebug() << "ini_file is empty in setPrintTaskFinished";
            }

            // 3. 延迟删除该行，避免在遍历模型时修改模型结构导致的潜在问题
//            QTimer::singleShot(0, this, [this, rowToDelete]()
//            {
//                if (task_model && rowToDelete >= 0 && rowToDelete < task_model->rowCount()) {
//                    task_model->removeTasks({ rowToDelete });
//                }
//            });

            break;
        }
    }
}


void TaskListPanel::on_max_btn_clicked()
{
    emit max_btn_Clicked(max_btn->isChecked());
}

//void TaskListPanel::on_action_btn_clicked()
//{
//    qDebug() << "on_action_btn_clicked.isChecked() <<<==========" << action_btn->isChecked();
//    if(action_btn->isChecked())
//    {
//        action_btn->setText(action_btn_checked_text);
//    }
//    else
//    {
//        action_btn->setText(action_btn_text);
//    }
//    emit action_btn_Clicked(action_btn->isChecked());
//}

void TaskListPanel::on_delete_btn_clicked()
{
    // 保存当前焦点窗口
    QWidget *oldFocusWidget = QApplication::focusWidget();

    // 获取被选中行
    if (!task_table || !task_table->selectionModel())
        return;

    // 检查是否满足条件：print_list状态下且任务运行中
    if (mainWindow)
    {
        PrintProcessSta currentStatus;
        m_bIsPrintStart = mainWindow->isPrintListAutoMode(currentStatus);
        QVector<PrintProcessSta> statusVector;
        statusVector << FINISHED << EXCEPTION << STARTMARK << CHANGESLOT;
        if(statusVector.contains(currentStatus) && m_bIsPrintStart)
        {
            DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
            msg_box->setContent(tr("打印任务正在运行，请勿执行删除操作！"));
            msg_box->exec();

            return;
        }
    }

    QModelIndexList selectedIndexes = task_table->selectionModel()->selectedRows();

    if (selectedIndexes.isEmpty())
    {
        DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
        msg_box->setContent(tr("请选择需要删除的任务。"));
        msg_box->exec();
        return;
    }

    if (m_bIsPrintStart)
    {
        emit action_btn_Clicked(false);
    }

    if (!task_model)
        return;

    //把edit_panel里的input_panel编辑框的焦点转移到此处
    this->setFocus();

    QList<int> rowsToDelete;
    QStringList taskNamesToDelete;

    for (const QModelIndex &index : selectedIndexes)
    {
        int row = index.row();
        rowsToDelete << row;
        taskNamesToDelete << task_model->getTask(row).name;
    }

    // 降序排序，防止删除时行号错位
    std::sort(rowsToDelete.begin(), rowsToDelete.end(), std::greater<int>());

    // 可选：确认框
    DialogMsgBox *msg_box = new DialogMsgBox(ASK_DIALOG);
    msg_box->setContent(tr("该操作无法还原，\n是否确定删除？"));
    if (msg_box->exec() != QDialog::Accepted)
    {
        if (m_bIsPrintStart)
        {
            m_bIsPrintStart = false;
            emit action_btn_Clicked(true);
        }

        // 恢复之前的焦点窗口
        if (oldFocusWidget)
        {
            oldFocusWidget->setFocus();
        }
        return;
    }

    // 暂时断开 selectionChanged 信号连接
    disconnect(task_table->selectionModel(), &QItemSelectionModel::selectionChanged,
               this, &TaskListPanel::on_row_state_changed);

    disconnect(task_table->model(), &QAbstractItemModel::rowsRemoved,
               this, &TaskListPanel::on_row_state_changed);

//    parentWidget()->setEnabled(false);
    // ========== 删除进度窗口 ==========
//    QProgressDialog progress_dialog(tr("正在删除，请稍候..."), QString(), 0, 0, nullptr);
//    progress_dialog.setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint); // 无边框窗口
//    progress_dialog.setFixedSize(300, 100);
//    progress_dialog.setStyleSheet(SS_ProgressDialog);
//    progress_dialog.setWindowModality(Qt::WindowModal);
//    progress_dialog.show();


    //===== 先行删除任务队列记录 + 模型任务 =====
    for (int i = 0; i < rowsToDelete.size(); ++i)
    {
        const QString &taskName = taskNamesToDelete[i];
    }
    task_model->removeTasks(rowsToDelete);
    task_table->clearSelection();
    task_table->viewport()->update();

//    // 异常场景下恢复队列,插入/删除都要恢复
//    print_task_queue.recoverIfTaskAlreadyFinished();


    // ===== INI 文件删除任务记录 =====
    QSettings task_list_ini(ini_file, QSettings::IniFormat);

    // 1. 删除 TaskArrange 排序记录
    task_list_ini.beginGroup("TaskArrange");
    QString arrange_str = task_list_ini.value("Arrange", "").toString();
    QStringList arrange_str_list = arrange_str.split(",", Qt::SkipEmptyParts);
    for (const QString &taskName : taskNamesToDelete)
    {
        arrange_str_list.removeAll(taskName);
    }
    task_list_ini.setValue("Arrange", arrange_str_list.join(","));
    task_list_ini.endGroup();

    // 2. 删除 TaskList 中的键
    task_list_ini.beginGroup("TaskList");
    for (const QString &taskName : taskNamesToDelete)
    {
        task_list_ini.remove(taskName);
    }
    task_list_ini.endGroup();

    // ===== 删除任务队列记录 + 模型任务 =====
//    for (int i = 0; i < rowsToDelete.size(); ++i)
//    {
//        const QString &taskName = taskNamesToDelete[i];
//        print_task_queue.removeTask(taskName);
//        //task_model->removeRow(rowsToDelete[i]);
//    }
//    task_model->removeTasks(rowsToDelete);
//    task_table->clearSelection();
//    task_table->viewport()->update();

    //parentWidget()->setEnabled(true);
    // 删除任务完成后，恢复信号连接
    connect(task_table->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &TaskListPanel::on_row_state_changed);

    connect(task_table->model(), &QAbstractItemModel::rowsRemoved,
            this, &TaskListPanel::on_row_state_changed);
    on_row_state_changed();

    if (m_bIsPrintStart)
    {
        m_bIsPrintStart = false;
        emit action_btn_Clicked(true);
    }

    // 恢复之前的焦点窗口
    if (oldFocusWidget)
    {
        oldFocusWidget->setFocus();
    }
}


void TaskListPanel::on_table_cellclick(const QModelIndex &index)
{

    if (!index.isValid() || !task_model || !task_table)
         return;

     int row = index.row();
     int col = index.column();

     const TaskData &task = task_model->getTask(row);

     // 示例：打印任务名
     qDebug() << "[Task Clicked] Row:" << row << ", Name:" << task.name;
     qDebug() << "[Task Clicked] Row:" << row << ", fullText:" << task.fullText;

     // 可选：发射信号，通知状态栏
     emit signal_task_info(task.fullText);

     // 可选：进行选中项样式刷新
     task_table->viewport()->update();
}



void TaskListPanel::on_row_state_changed()
{
//    int totalRows = task_table->rowCount();
//    int selectedRows = task_table->selectionModel()->selectedRows().count();

    if (!task_model || !task_table || !task_table->selectionModel() || !task_stat)
        return;

    int totalRows = task_model->rowCount();
    int selectedRows = task_table->selectionModel()->selectedRows().count();

    QString labelText = QString("%1: <span style='color:#4d4d4d'>%3</span> | %2: <span style='color:#4d4d4d'>%4</span>").arg(tr("已选"), tr("全部"))
            .arg(selectedRows)
            .arg(totalRows);

    task_stat->setText(labelText);
}


void TaskListPanel::set_print_sta_slot(const QString &printName, PrintSta sta)
{
    if (!task_model)
        return;

    for (int i = 0; i < task_model->rowCount(); ++i)
    {
        if (task_model->getTask(i).name == printName)
        {
            task_model->updateTaskStatus(i, sta);
            break;
        }
    }
}


void TaskListPanel::show_context_menu(const QPoint &pos)
{
    // 获取点击的行
    int row = task_table->rowAt(pos.y());

    //    qDebug() << "Clicked Row" << row;
    if (row == 0) return;  // 检查是否点击在有效行上(未深究为何点击表格外是0，但最后一条是-1)

    // 创建右键菜单
    context_menu = new QMenu(this);
    context_menu->setStyleSheet(SS_context_menu);

    delete_action = new QAction(tr("删除"), this);
    extract_action = new QAction(tr("提取内容"), this);
    select_all_action = new QAction(tr("全选"), this);
    cancel_action = new QAction(tr("取消"), this);

    // 将动作添加到菜单
    context_menu->addAction(select_all_action);
    context_menu->addAction(cancel_action);
    context_menu->addSeparator();
    context_menu->addAction(extract_action);
    context_menu->addSeparator();
    context_menu->addAction(delete_action);

    // 连接动作信号到槽函数
    connect(select_all_action, &QAction::triggered, this, [=]() { task_table->selectAll(); });
    connect(cancel_action, &QAction::triggered, this, [=]() { task_table->clearSelection(); });
    connect(extract_action, &QAction::triggered, this, [=]() { emit task_content_list_Extracted(getSelectedTaskList()); });
    connect(delete_action, &QAction::triggered, this, [=]() { on_delete_btn_clicked(); });

    // 显示菜单
    context_menu->exec(mapToGlobal(pos));
}

void TaskListPanel::set_layout()
{
    this->setFixedWidth(780);

    title_area = new QWidget();
    title_area->setFixedHeight(50);

    title = new QLabel();
    title->setFixedSize(180,50);
    title->setText(tr("任务列表"));
    title->hide();

//    action_btn = new QPushButton();
//    action_btn->setFixedSize(130, 40);
//    action_btn->setCheckable(true);
//    action_btn->setText(tr("打印(&E)"));
//    action_btn->hide();
//    action_btn->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_E));

    delete_btn = new QPushButton();
    delete_btn->setFixedSize(130, 40);
    delete_btn->setText(tr("删除(&R)"));
    delete_btn->hide();
//    delete_btn->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_R));

    task_stat = new QLabel();
    task_stat->setFixedSize(250, 50);
    task_stat->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    task_stat->setText(tr("已选：0 | 全部：0"));

    max_btn = new QPushButton();
    max_btn->setCheckable(true);
    max_btn->setFixedSize(40, 40);
    max_btn->setText("-");
    max_btn->hide();

    QHBoxLayout *title_area_layout = new QHBoxLayout(title_area);
    title_area_layout->setContentsMargins(10,0,100,0);
    title_area_layout->setSpacing(14);

    title_area_layout->addWidget(title);
    title_area_layout->addStretch();
    //title_area_layout->addWidget(action_btn);
    title_area_layout->addWidget(delete_btn);
    title_area_layout->addStretch();
    title_area_layout->addWidget(task_stat);
    //title_area_layout->addWidget(max_btn);

    task_table = new QTableView(this);
    task_model = new TaskTableModel(this);
    task_table->setModel(task_model);
    task_table->horizontalHeader()->setStretchLastSection(true);
    task_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    task_table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    task_table->setItemDelegateForColumn(ColNum::COL_STA, new StatusItemDelegate(this));

    task_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    task_table->setColumnWidth(ColNum::COL_INDEX, 60); //50
    task_table->setColumnWidth(ColNum::COL_STA, 120);
    task_table->setColumnWidth(ColNum::COL_INFO, 200);
    task_table->setColumnWidth(ColNum::COL_CONTENT, 230); //240
    task_table->setColumnHidden(ColNum::COL_NAME, true);
    task_table->verticalHeader()->setDefaultSectionSize(65);
    task_table->horizontalHeader()->setFixedHeight(35);
    //task_table->setFixedHeight(400);
    task_table->verticalHeader()->setVisible(false);
    task_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    task_table->setFocusPolicy(Qt::NoFocus);
    task_table->setWordWrap(false);


    table_area = new QWidget();
    table_area->setFixedWidth(710);

    QVBoxLayout *table_area_layout = new QVBoxLayout(table_area);
    table_area_layout->setContentsMargins(10,0,10,0);
    table_area_layout->setSpacing(0);
    table_area_layout->addWidget(task_table);

    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(0,0,0,0);
    main_layout->setSpacing(0);
    main_layout->addWidget(title_area);
    main_layout->addWidget(table_area);
}

void TaskListPanel::set_style_sheet()
{
    this->setStyleSheet(SS_input_area);

    title_area->setStyleSheet(SS_Title_Area);
    title->setStyleSheet(SS_Task_Title);
//    action_btn->setStyleSheet(SS_Primary_Btn);
    delete_btn->setStyleSheet(SS_Normal_Btn);
    task_stat->setStyleSheet(SS_Normal_Label);
    max_btn->setStyleSheet(SS_Max_Btn);

    table_area->setStyleSheet(SS_Table_Area);
    task_table->setStyleSheet(SS_Task_Table_View);

}

QString TaskListPanel::task_type_display_str(int type)
{
    QString str_type;
    switch (type) {
    case TaskType::LOCAL_TASK:
        str_type = QString(tr("本地任务"));
        break;
    case TaskType::REMOTE_TEXT_TASK:
        str_type = QString(tr("远程文本"));
        break;
    case TaskType::REMOTE_IMAGE_TASK:
        str_type = QString(tr("远程图片"));
        break;
    default:
        str_type = QString(tr("未知任务"));
    }
    return str_type;
}

QString TaskListPanel::sta_display_str(int sta)
{
    //正数作打印次数用
    if(sta >= 0)
    {
        return QString::number(sta);
    }

    //负数作打印状态用
    QString str_sta;
    switch (sta) {
    case PrintSta::PRINT_STA_WAIT:
        str_sta = QString(tr("等待"));
        break;
    case PrintSta::PRINT_STA_FROZEN:
        str_sta = QString(tr("冻结"));
        break;
    case PrintSta::PRINT_STA_PRINTING:
        str_sta = QString(tr("打印中"));
        break;
    default:
        str_sta = QString(tr("未知"));
    }
    return str_sta;
}

/*
QString TaskListPanel::slots_display_str(const QString &slots_num)
{
    Logger::instance().log("slots_display_str.传入参数 =====>>>>>>", slots_num);

    QString slots_display;

    QString slot_group;
    QStringList slot_num_list;
    if(slots_num.contains(SUB_SPLIT_SYMBOL))    // 分组名0x1f槽盒号
    {
        slot_group = slots_num.split(SUB_SPLIT_SYMBOL)[0];
        slot_num_list = slots_num.split(SUB_SPLIT_SYMBOL)[1].split(",");
    }
    else
    {
        slot_num_list = slots_num.split(",");
    }

    for(QString &str : slot_num_list)
    {
        bool isInt;
        str.toInt(&isInt);
        if(str != "" && isInt)
        {
            slots_display.append(QString(QChar(0x2460 + str.toUInt() - 1)));
        }
    }

    if (!slot_group.isEmpty())
    {
        slots_display.prepend("\"" + slot_group + "\" ");
    }

    Logger::instance().log("slots_display_str.返回 =====>>>>>>", slots_display);

    return slots_display;
}
*/


QString TaskListPanel::slots_display_str(const QString &slots_num)
{
    //Logger::instance().log("slots_display_str.传入参数 =====>>>>>>", slots_num);

    QString slots_display;
    QString slot_group;
    QStringList slot_num_list;

    // 拆分 group + 槽号
    QStringList parts = slots_num.split(SUB_SPLIT_SYMBOL);
    if (parts.size() >= 2) {
        slot_group = parts.at(0).trimmed();  // 分组名
        slot_num_list = parts.at(1).split(",", Qt::SkipEmptyParts);
    } else {
        slot_num_list = slots_num.split(",", Qt::SkipEmptyParts);
    }

    if (!slot_group.isEmpty()) {
        // 分组关键字优先，只显示分组，不显示槽号
        slots_display = QString("\"%1\" ").arg(slot_group);
    } else {
        // 没有分组 → 显示槽号，限制在 1-6
        for (const QString &str : slot_num_list) {
            bool isInt = false;
            uint num = str.toUInt(&isInt);
            if (isInt && num >= 1 && num <= 6) {
                slots_display.append(QChar(0x2460 + num - 1));  // ① 到 ⑥
            }
        }
    }

    //Logger::instance().log("slots_display_str.返回 =====>>>>>>", slots_display);

    return slots_display;
}



int TaskListPanel::serial_number()
{
    s_num > 99999? s_num = 1 : s_num++;
    return s_num;
}


void TaskListPanel::deleteExpiredTasks(int expirationDay, QDate currentDate)
{

    if(expirationDay <=0)
    {
        return;
    }

    if (!task_model)
        return;

    int work_day = expirationDay;

    qDebug() << "deleteExpiredTasks====>>>>" << work_day;

    //QDate expiration_date = QDate::currentDate();
    QDate expiration_date = currentDate;

    qDebug() << "expiration_date====>>>>" << expiration_date;

    //计算指定工作日前的日期
    while (work_day > 0)
    {
        expiration_date = expiration_date.addDays(-1);  // 向前移动一天
        if (expiration_date.dayOfWeek() != Qt::Saturday && expiration_date.dayOfWeek() != Qt::Sunday)
        {
            --work_day;  // 仅计算工作日
        }
    }

//    QDateTime now = QDateTime::currentDateTime();
    QDateTime now = QDateTime::fromString(currentDate.toString("yyyy-MM-dd") + " 23:59:59", "yyyy-MM-dd HH:mm:ss");
    QList<int> rowsToDelete;

    for (int i = 0; i < task_model->rowCount(); ++i)
    {
        const TaskData &task = task_model->getTask(i);

        QString timeStr = task.info.section('\n', 1, 1).trimmed();
        QDateTime taskTime = QDateTime::fromString(timeStr, "yyyy-MM-dd HH:mm:ss");
        qDebug() << "Delete taskTime:" << taskTime;

        if (taskTime.isValid() && taskTime.daysTo(now) > expirationDay)
        {
            qDebug() << "Delete rowsToDelete:" << rowsToDelete;
            rowsToDelete << i;
        }
    }

    if (!rowsToDelete.isEmpty())
    {
        task_model->removeTasks(rowsToDelete);
        if (task_table)
        {
            task_table->clearSelection();  // 可选，清除残留选中
            task_table->viewport()->update();  // 强制刷新界面
        }
    }


        //删除配置文件过期任务
        QSettings task_list_ini(ini_file, QSettings::IniFormat);
        task_list_ini.beginGroup("TaskArrange");
        QString arrange_str = task_list_ini.value("Arrange", "").toString();
        QStringList task_names = arrange_str.split(",");
        QStringList del_task_names;
        for(int i=task_names.size()-1; i>=0; i--)
        {
            QDate task_date = QDate::fromString(task_names[i].mid(5, 8), "yyyyMMdd");
            if(task_date.isValid() && task_date < expiration_date)
            {
                qDebug() << "Delete expired tasks arr name:" << task_names[i];
                del_task_names.append(task_names[i]);
                task_names.removeAll(task_names[i]);
            }
        }
        task_list_ini.setValue("Arrange", task_names.join(","));
        task_list_ini.endGroup();

        task_list_ini.beginGroup("TaskList");
        for (int i=0; i< del_task_names.size(); i++)
        {
            QString task_text = task_list_ini.value(del_task_names[i], "").toString();
            QStringList task_text_list = task_text.split(SPLIT_SYMBOL);
            QString datePart = del_task_names[i].mid(5, 8);
            QString task_data_dir = getTaskdataDir() + datePart;

            if(task_text_list.size() >= TaskTextPos::TASK_MAX_ITEM)
            {
                if(task_text_list[TaskTextPos::TASK_TYPE].toInt() == TaskType::REMOTE_IMAGE_TASK)
                {
                    //同时删除图片
                    QFile::remove(QString("%1/%2.png").arg(task_data_dir, del_task_names[i]));
                }
            }

            task_list_ini.remove(del_task_names[i]);
            qDebug() << "Delete expired tasks text:" << del_task_names[i];
        }
        task_list_ini.endGroup();
}

QDateTime TaskListPanel::extractTaskTime(const TaskData &task) const
{
    QString timeStr = task.info.section('\n', 1, 1).trimmed();
    return QDateTime::fromString(timeStr, "yyyy-MM-dd HH:mm:ss");
}

void TaskListPanel::setAllowDuplicatePrintTask(bool isAllow)
{
    allow_duplicate_print_task = isAllow;
}

bool TaskListPanel::isPanelMax()
{
    return task_table->isVisible();
}

void TaskListPanel::changeEvent(QEvent *event)
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

void TaskListPanel::retranslateUi()
{
    //按钮
//    if(action_btn->isChecked())
//    {
//        action_btn->setText(action_btn_checked_text);
//    }
//    else
//    {
//        action_btn->setText(action_btn_text);
//    }

    //刷新统计和选中数量标签
    on_row_state_changed();

    //刷新表头
    //task_table->setHorizontalHeaderItem(ColNum::COL_INDEX, new QTableWidgetItem(tr("全选")));
    task_table->model()->setHeaderData(ColNum::COL_INDEX, Qt::Horizontal, tr("全选"));

    //重新读取列表内容
    //loadTaskListFromFile_OnThread();
    updateTableRowTexts();
}

void TaskListPanel::loadTaskListFromFile_OnThread()
{
    //线程加载数据
    QThread* thread = new QThread();
    connect(thread, &QThread::started, thread, [=]()mutable{
        qDebug()<<"started!";
        thread->quit();
    }, Qt::DirectConnection);
    connect(thread, &QThread::finished, this, [=]()mutable{
        thread->wait();
        thread->deleteLater();
        qDebug()<<"finished!";

        loadTaskListFromFile();
    });
    thread->start();
}

void TaskListPanel::updateTableRowTexts()
{
    if (!task_model)
        return;

    int rowCount = task_model->rowCount();
    if (rowCount <= 0)
        return;

    for (int row = 0; row < rowCount; ++row)
    {
        TaskData &task = task_model->getTaskRef(row); // 获取可修改引用
        task.statusText = sta_display_str(task.status); // 根据枚举重新生成状态文本
    }

    // 通知视图所有行的 COL_STA 列更新
    QModelIndex topLeft = task_model->index(0, ColNum::COL_STA);
    QModelIndex bottomRight = task_model->index(rowCount - 1, ColNum::COL_STA);
    emit task_model->dataChanged(topLeft, bottomRight);
}
