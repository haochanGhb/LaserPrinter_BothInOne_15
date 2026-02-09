#include "template_dialog.h"
#include "menu_dialog_style.h"

TemplateDialog::TemplateDialog(BaseDialog *parent) :
    BaseDialog(parent)
   ,p_selection_temp_attr(NULL)
   ,m_enableTouchKeyboard(true) // 默认为true，启用触摸键盘功能
{
    setTitle(tr("模板"));
    create_widget();
    setup_layout_v2();
    set_stylesheet();
    connect_signals_and_slots();

    load_template_names(DEFAULT_TEMPLATES_DIR);
    load_template_names(USER_TEMPLATES_DIR);

    //读取最后选择的模板名
    QSettings main_win_cfg_ini(LAST_STATUS_INI, QSettings::IniFormat);
    main_win_cfg_ini.beginGroup(QString("last_selected_template"));
    QString name = main_win_cfg_ini.value("name").toString();
    main_win_cfg_ini.endGroup();

    template_selector->setCurrentText(name);
    set_template_edit_enable(false);

    //初始化设置二维码宽高滑块
    code_width_slider->setValue(edit_temp_attr.code_attr.width);
    code_height_slider->setValue(edit_temp_attr.code_attr.height);
}

TemplateDialog::~TemplateDialog()
{

    if(!p_selection_temp_attr)
    {
       delete p_selection_temp_attr;
       p_selection_temp_attr = nullptr;
    }
}

void TemplateDialog::create_widget()
{
    //上下结构区域模板
    upper_edit_area = new QWidget(this);
    upper_edit_area->setFixedSize(780,650);

    upper_btn_panel = new QWidget(this);
    upper_btn_panel->setFixedSize(780,50);

    upper_preview_panel = new QWidget(this);
    upper_preview_panel->setFixedSize(780,600);

    upper_preview_left_panel = new QWidget(this);
    upper_preview_left_panel->setFixedSize(480,600);

    upper_preview_right_panel = new QWidget(this);
    upper_preview_right_panel->setFixedSize(300,600);

    lower_content_area = new QWidget(this);
    //lower_content_area->setFixedSize(780,500);

    // 创建滚动区域
    scrollArea = new QScrollArea(this);
    scrollArea->setObjectName("scrollArea");
    scrollArea->setWidget(lower_content_area);
    scrollArea->setWidgetResizable(true);   // 保证内容区可以自动扩展
    scrollArea->setFixedSize(780, 500);     // 保持原有显示区域大小
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 横向不需要
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);    // 纵向需要时显示

    lower_content_area->setFixedWidth(780);

    //各区域面板
    //temp_panel = new QWidget(this);
    //temp_panel->setFixedSize(300,420); //240,420

    //code_type_panel = new QWidget(this);
    //code_type_panel->setFixedSize(300,370);

    code_param_panel = new QWidget(upper_preview_right_panel);
    code_param_panel->setFixedSize(300,600); //300,400
    preview_panel = new QWidget(upper_preview_left_panel);
    preview_panel->setFixedSize(480,600); //476,597
//    content_panel = new QWidget(this);
//    content_panel->setFixedSize(480,470);

    //temp_panel->hide();
    //code_type_panel->hide();
//    content_panel->hide();

    temp_title = new QLabel(tr("模板"));
    temp_title->setFixedSize(100,40);
    temp_title->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    template_selector = new QComboBox();
    template_selector->setFixedSize(130,40);
    template_selector->setMaxVisibleItems(20);

    temp_create_btn = new QPushButton(tr("新建"));
    temp_create_btn->setFixedSize(120,40);

    //temp_panel
    //temp_list = new QListWidget(temp_panel);
    //temp_list->setGeometry(10,40,280,370); //10,40,220,370

    //code_param_panel
    code_type_title = new QLabel(tr("类型"), code_param_panel);
    code_type_title->setGeometry(20,45,200,35);
    code_type_title->hide();

    code_type_selector = new QComboBox(code_param_panel);
    code_type_selector->setGeometry(20,55,250,40); //20,90,250,40
    code_type_selector->addItem(tr("空"),QVariant::fromValue(-1));
    code_type_selector->addItem(QString("QR Code"),static_cast<int>(QBarcode::QR_Code));
    code_type_selector->addItem(QString("Data Matrix"),static_cast<int>(QBarcode::Data_Matrix));
    code_type_selector->addItem(QString("Code 39"),static_cast<int>(QBarcode::Code_39));
    code_type_selector->addItem(QString("Code 93"),static_cast<int>(QBarcode::Code_93));
    code_type_selector->addItem(QString("EAN 8"),static_cast<int>(QBarcode::EAN_8));
    code_type_selector->addItem(QString("EAN 13"),static_cast<int>(QBarcode::EAN_13));
    code_type_selector->addItem(QString("EAN 128C"),static_cast<int>(QBarcode::EAN_128C));
    code_type_selector->addItem(QString("Code 128A"),static_cast<int>(QBarcode::Code_128A));
    code_type_selector->addItem(QString("Code 128B"),static_cast<int>(QBarcode::Code_128B));
    code_type_selector->addItem(QString("Code 128C"),static_cast<int>(QBarcode::Code_128C));
    code_type_selector->addItem(QString("ITF25"),static_cast<int>(QBarcode::ITF25));

//    code_type_list = new QListWidget(code_type_panel);
//    code_type_list->setGeometry(10,40,280,320); //10,40,220,320
//    code_type_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    code_type_list->setVerticalScrollMode(QListWidget::ScrollPerPixel);//设置为像素滚动

    //code_param_panel
    code_param_title = new QLabel(tr("条码设置"), code_param_panel);
    code_param_title->setGeometry(10,3,200,35);

    code_param_pic = new QLabel(code_param_panel);
    code_param_pic->setGeometry(10,45,217,65);
    code_param_pic->hide();

    code_fixed_size_checkbox = new QCheckBox(tr("固定条码大小"), code_param_panel);
    code_fixed_size_checkbox->setGeometry(20,120,200,40);
    code_fixed_size_checkbox->hide();

    code_size_title  = new QLabel(tr("大小"), code_param_panel);
    code_size_title->setGeometry(20,140,200,35);
    code_size_title->hide();

    code_width_label = new QLabel(tr("宽"), code_param_panel);
    code_width_label->setGeometry(10,100,50,40); //10,170,50,40
    code_height_label = new QLabel(tr("高"), code_param_panel);
    code_height_label->setGeometry(10,150,50,40); //10,220,50,40

    code_width_slider = new QSlider(Qt::Horizontal, code_param_panel);
    code_width_slider->setGeometry(60,100,190,40); //60,170,190,40
    //code_width_slider->setRange(1,30);
    code_width_slider_val = new QLabel(code_param_panel);
    code_width_slider_val->setGeometry(260,100,40,40); //260,170,40,40

    code_height_slider = new QSlider(Qt::Horizontal, code_param_panel);
    code_height_slider->setGeometry(60,150,190,40); //60,220,190,40
    code_height_slider->setRange(1,30);
    code_height_slider_val = new QLabel(code_param_panel);
    code_height_slider_val->setGeometry(260,150,40,40); //260,220,40,40

    code_seperator_title  = new QLabel(tr("内容分隔符"), code_param_panel);
    code_seperator_title->setGeometry(10,195,200,40); //10,265,200,35

    code_seperator_lineedit = new QLineEdit(code_param_panel);
    code_seperator_lineedit->setGeometry(20,240,210,40); //20,310,210,40

    code_seperator_checkbox = new QCheckBox(code_param_panel);
    code_seperator_checkbox->setGeometry(240,240,40,40); //240,310,40,40

    code_width_spinbox = new QSpinBox(code_param_panel);
    code_width_spinbox->setGeometry(60,95,150,40); //60,165,150,40
    //code_width_spinbox->setRange(1,30);
    code_width_spinbox->setVisible(false);
    code_height_spinbox = new QSpinBox(code_param_panel);
    code_height_spinbox->setGeometry(60,145,150,40); //60,215,150,40
    code_height_spinbox->setRange(1,30);
    code_height_spinbox->setVisible(false);

    logo_setting_title  = new QLabel(tr("徽标设置"), code_param_panel);
    logo_setting_title->setGeometry(10,290,200,35); //10,360,200,35

    logo_setting_display  = new QLabel(tr("logo"), code_param_panel);
    logo_setting_display->setGeometry(20,330,210,100); //20,400,210,100
    logo_setting_display->setText("");

    logo_setting_show_checkbox = new QCheckBox(code_param_panel);
    logo_setting_show_checkbox->setGeometry(240,360,40,40); //240,430,40,40

    logo_width_label = new QLabel(tr("宽"), code_param_panel);
    logo_width_label->setGeometry(10,440,50,40); //10,510,50,40
    logo_height_label = new QLabel(tr("高"), code_param_panel);
    logo_height_label->setGeometry(10,490,50,40); //10,560,50,40

    logo_width_slider = new QSlider(Qt::Horizontal, code_param_panel);
    logo_width_slider->setGeometry(60,440,190,40); //60,510,190,40
    logo_width_slider->setRange(1,30);
    //logo_width_slider->setRange(16, PIXEL_VIEW_WIDTH);   // 宽度范围
    logo_width_slider_val = new QLabel(code_param_panel);
    logo_width_slider_val->setGeometry(260,440,40,40); //260,510,40,40

    logo_height_slider = new QSlider(Qt::Horizontal, code_param_panel);
    logo_height_slider->setGeometry(60,490,190,40);  //60,560,190,40
    logo_height_slider->setRange(1,30);
    //logo_height_slider->setRange(16, PIXEL_VIEW_HEIGHT); // 高度范围
    logo_height_slider_val = new QLabel(code_param_panel);
    logo_height_slider_val->setGeometry(260,490,40,40); //260,560,40,40

//    logo_setting_title->hide();
//    logo_setting_display->hide();
//    logo_setting_show_checkbox->hide();
//    logo_width_label->hide();
//    logo_height_label->hide();
//    logo_width_slider->hide();
//    logo_width_slider_val->hide();
//    logo_height_slider->hide();
//    logo_height_slider_val->hide();
    //code_param_panel->hide();
    //preview_panel->hide();

    //preview_panel
    m_pTemplatePreview = new TemplatePreview(preview_panel);
    m_pTemplatePreview->move(1, 1);


    //==============以下 组件父窗体content_panel 改 为lower_content_area==============
    QButtonGroup *name_btn_group = new QButtonGroup(lower_content_area);
    name_btn_group->setExclusive(true);

    QFontDatabase font_database;
    QStringList fontFamilies = font_database.families();

    for (int i=0; i<MAX_INPUT_PANEL; i++)
    {
        //text_page
        name_btn[i] = new QPushButton(lower_content_area);
        name_btn[i]->setFixedSize(100,40);
        name_btn[i]->setCheckable(true);
        name_btn[i]->setObjectName(QString::number(i));
        name_btn_group->addButton(name_btn[i]);

        sta_selector[i] = new QComboBox(lower_content_area);
        sta_selector[i]->setFixedSize(105,40);

        if(i==MAX_INPUT_PANEL-1)
        {
            time_format_selector = new QComboBox(lower_content_area);
            time_format_selector->setFixedSize(227,40);
            //time_format_selector->setFixedHeight(40);
            sta_selector[i]->addItem(tr("显示"), "display");
            sta_selector[i]->addItem(tr("隐藏"), "invisible"); //“隐藏”的显示名和变量名与非时间字段保持统一，但区分处理
        }
        else
        {
            input_box[i] = new QLineEdit(lower_content_area);
            //input_box[i]->setFixedHeight(40);
            input_box[i]->setFixedSize(167,40);//105,40
            //input_box[i]->setFixedHeight(40);
            input_box[i]->setMaxLength(50);
            sta_selector[i]->addItem(tr("可编辑"), "editable");
            sta_selector[i]->addItem(tr("固定"), "fixed");
            sta_selector[i]->addItem(tr("隐藏"), "invisible");  //“隐藏”变量名用invisible
            sta_selector[i]->addItem(tr("不可用"), "hidden");  //为了兼容以往的模版，“不可用”变量名用hidden，相当于disabled
//            sta_selector[i]->setObjectName(QString::number(i));
            input_box[i]->setObjectName(QString::number(i));
        }
        sta_selector[i]->setObjectName(QString::number(i));

        min_width[i] = new QSpinBox(lower_content_area);
        min_width[i]->setFixedSize(55,40);
        min_width[i]->setRange(-1,20);
        min_width[i]->setValue(-1);
        min_width[i]->setObjectName(QString::number(i));

        if(i < MAX_INPUT_PANEL-1)
        {
            min_width[i]->setVisible(true);
        }
        else
        {
            min_width[i]->setVisible(false);
        }

        //font_page
        font_selector[i] = new QComboBox(lower_content_area);
        //font_selector[i]->setFixedHeight(40);
        font_selector[i]->setFixedSize(180,40);
        font_selector[i]->addItems(fontFamilies);
        //font_selector[i]->setVisible(false);
        font_selector[i]->setObjectName(QString::number(i));
        font_size[i] = new QSpinBox();
        font_size[i]->setFixedSize(65,40);
        font_size[i]->setRange(9,60);
        //font_size[i]->setVisible(false);
        font_size[i]->setObjectName(QString::number(i));

        barcode_checkbox[i] = new QCheckBox(lower_content_area);
    }

    time_format_selector->addItem("yyyy-MM-dd HH:mm:ss");
    time_format_selector->addItem("yyyy.MM.dd HH:mm:ss");
    time_format_selector->addItem("MM/dd/yyyy HH:mm:ss");
    time_format_selector->addItem("dd/MM/yyyy HH:mm:ss");
    time_format_selector->addItem("yyyy-MM-dd");
    time_format_selector->addItem("yyyy.MM.dd");
    time_format_selector->addItem("MM/dd/yyyy");
    time_format_selector->addItem("dd/MM/yyyy");
    time_format_selector->addItem("HH:mm:ss");
    //高亮功能：时间格式添加事件过滤器
    time_format_selector->setObjectName(QString::number(MAX_INPUT_PANEL-1));

    name_btn[0]->setText("病理号");
    name_btn[1]->setText("小号");
    name_btn[2]->setText("备注1");
    for(int i = 3;i < MAX_INPUT_PANEL-1 ;i++)
    {
        name_btn[i]->setText("");
    }
    name_btn[MAX_INPUT_PANEL-1]->setText("时间");

    content_panel_title = new QWidget(lower_content_area);
    content_panel_title->setFixedHeight(40);

    for(int i = 0; i< 7;i++)
    {
        column_title[i] = new QLabel(lower_content_area);
        column_title[i]->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    }

    //底部按钮
    delete_btn = new QPushButton();
    delete_btn->setFixedSize(120,40);
    delete_btn->setText(tr("删除"));
    copy_btn = new QPushButton();
    copy_btn->setFixedSize(120,40);
    copy_btn->setText(tr("复制"));
    edit_btn = new QPushButton();
    edit_btn->setFixedSize(120,40);
    edit_btn->setText(tr("编辑"));
    save_btn = new QPushButton();
    save_btn->setFixedSize(120,40);
    save_btn->setText(tr("保存"));
    cancel_btn = new QPushButton();
    cancel_btn->setFixedSize(120,40);
    cancel_btn->setText(tr("取消"));

    delete_btn->setEnabled(false);
    edit_btn->setEnabled(false);
}


//新布局
/*
void TemplateDialog::setup_layout_v2()
{
    this->bottomBar_hide();
    QVBoxLayout *main_v_layout = new QVBoxLayout();
    QVBoxLayout *upper_layout =  new QVBoxLayout(upper_edit_area);

    QHBoxLayout *upper_btn_panel_h_layout = new QHBoxLayout(upper_btn_panel);
    upper_btn_panel_h_layout->setContentsMargins(0,0,20,0);
    upper_btn_panel_h_layout->setSpacing(2);
    upper_btn_panel_h_layout->addWidget(temp_title);
    upper_btn_panel_h_layout->addWidget(template_selector);
    upper_btn_panel_h_layout->addStretch();

    upper_btn_panel_h_layout->addWidget(edit_btn);
    upper_btn_panel_h_layout->addWidget(copy_btn);
    upper_btn_panel_h_layout->addWidget(delete_btn);
    upper_btn_panel_h_layout->addWidget(save_btn);
    upper_btn_panel_h_layout->addWidget(cancel_btn);
    upper_btn_panel_h_layout->addWidget(temp_create_btn);
    //upper_btn_panel_h_layout->addStretch();

    //编辑上部分二维码预览面板布局：二维码预览(左)、条码设置(右)
    QHBoxLayout *upper_preview_panel_h_layout = new QHBoxLayout(upper_preview_panel);
    upper_preview_panel_h_layout->setContentsMargins(0,0,0,0);
    upper_preview_panel_h_layout->setSpacing(0);
    // 添加左侧面板
    upper_preview_panel_h_layout->addWidget(upper_preview_left_panel,1);

    // 创建垂直线分割器
    QFrame *verticalLine = new QFrame(upper_preview_panel);
    verticalLine->setFixedHeight(590);
    verticalLine->setFrameShape(QFrame::VLine);
    verticalLine->setStyleSheet(SS_SeparateLine);
    //verticalLine->setLineWidth(1);
    upper_preview_panel_h_layout->addWidget(verticalLine);

    upper_preview_panel_h_layout->addWidget(upper_preview_right_panel,2);

    //二维码预览(左)布局
    QVBoxLayout *upper_preview_left_panel_v_layout = new QVBoxLayout(upper_preview_left_panel);
    upper_preview_left_panel_v_layout->setContentsMargins(0,0,0,0);
    upper_preview_left_panel_v_layout->setSpacing(0);
    upper_preview_left_panel_v_layout->addWidget(preview_panel);

    //条码设置(右)布局
    QVBoxLayout *upper_preview_right_panel_v_layout = new QVBoxLayout(upper_preview_right_panel);
    upper_preview_right_panel_v_layout->setContentsMargins(0,0,0,0);
    upper_preview_right_panel_v_layout->setSpacing(0);
    upper_preview_right_panel_v_layout->addWidget(code_param_panel, 0, Qt::AlignTop);

    //编辑上部分区域布局：操作按钮面板(上)、二维码预览面板(下)
    upper_layout->setContentsMargins(0,0,0,0);
    upper_layout->setSpacing(0);
    upper_layout->addWidget(upper_btn_panel, 0, Qt::AlignHCenter);
    upper_layout->addWidget(upper_preview_panel, 0, Qt::AlignHCenter);

    //==============以下 组件父窗体content_panel 改 为lower_content_area==============
    //下部分文本设置区域布局
    //content_panel
    QVBoxLayout *content_panel_layout = new QVBoxLayout(lower_content_area);
    content_panel_layout->setContentsMargins(10,10,10,10);

    QHBoxLayout * content_panel_h_layout_title = new QHBoxLayout();
    content_panel_h_layout_title->addWidget(content_panel_title);

    QHBoxLayout *content_table_column_title_h_layout = new QHBoxLayout();
    column_title[0]->setFixedSize(name_btn[0]->width()+10,40);

    column_title[0]->setText(tr("名称"));
    column_title[1]->setFixedSize(input_box[0]->width()-15,40);

    column_title[1]->setText(tr("内容"));
    column_title[2]->setFixedSize(min_width[0]->width()+35,40);

    column_title[2]->setText(tr("有效长度"));
    column_title[3]->setFixedSize(font_selector[0]->width()-35,40);

    column_title[3]->setText(tr("字体"));
    column_title[4]->setFixedSize(font_size[0]->width()+10,40);

    column_title[4]->setText(tr("字号"));
    column_title[5]->setFixedSize(sta_selector[0]->width()-35,40);

    column_title[5]->setText(tr("状态"));
    column_title[6]->setFixedSize(column_title[5]->width(),40);

    column_title[6]->setText(tr("条码"));
    for(int i = 0;i<MAX_INPUT_PANEL -1 ;i++)
    {
         content_table_column_title_h_layout->addWidget(column_title[i]);
    }
    content_panel_layout->addLayout(content_table_column_title_h_layout);

    QHBoxLayout *content_panel_h_layout[MAX_INPUT_PANEL];
    for(int i=0; i<MAX_INPUT_PANEL; i++)
    {
        content_panel_h_layout[i] = new QHBoxLayout();
        content_panel_h_layout[i]->addWidget(name_btn[i]);
        content_panel_h_layout[i]->addWidget(i==MAX_INPUT_PANEL-1?(QWidget*)time_format_selector:(QWidget*)input_box[i]);
        content_panel_h_layout[i]->addWidget(min_width[i]);
        content_panel_h_layout[i]->addWidget(font_selector[i]);
        content_panel_h_layout[i]->addWidget(font_size[i]);
        content_panel_h_layout[i]->addWidget(sta_selector[i]);
        content_panel_h_layout[i]->addWidget(barcode_checkbox[i]);
        content_panel_layout->addLayout(content_panel_h_layout[i]);
    }

    main_v_layout->addWidget(upper_edit_area, 0, Qt::AlignHCenter);
    main_v_layout->addWidget(scrollArea, 0, Qt::AlignHCenter);

    main_layout->addLayout(main_v_layout);
}
*/

void TemplateDialog::setup_layout_v2()
{
    this->bottomBar_hide();
    QVBoxLayout *main_v_layout = new QVBoxLayout();
    main_v_layout->setContentsMargins(10, 0, 10, 0);
    main_v_layout->setSpacing(10);

    // 上部编辑区域
    QVBoxLayout *upper_layout = new QVBoxLayout(upper_edit_area);
    upper_layout->setContentsMargins(0, 0, 0, 0);
    upper_layout->setSpacing(0);

    // 上部按钮面板
    QHBoxLayout *upper_btn_panel_h_layout = new QHBoxLayout(upper_btn_panel);
    upper_btn_panel_h_layout->setContentsMargins(10, 0, 10, 0);
    upper_btn_panel_h_layout->setSpacing(10);
    upper_btn_panel_h_layout->addWidget(temp_title);
    upper_btn_panel_h_layout->addWidget(template_selector);
    upper_btn_panel_h_layout->addStretch();
    upper_btn_panel_h_layout->addWidget(edit_btn);
    upper_btn_panel_h_layout->addWidget(copy_btn);
    upper_btn_panel_h_layout->addWidget(delete_btn);
    upper_btn_panel_h_layout->addWidget(save_btn);
    upper_btn_panel_h_layout->addWidget(cancel_btn);
    upper_btn_panel_h_layout->addWidget(temp_create_btn);

    // 上部预览面板（二维码预览 + 条码设置）
    QHBoxLayout *upper_preview_panel_h_layout = new QHBoxLayout(upper_preview_panel);
    upper_preview_panel_h_layout->setContentsMargins(0, 0, 0, 0);
    upper_preview_panel_h_layout->setSpacing(0);

    // 左侧预览面板
    QVBoxLayout *upper_preview_left_panel_v_layout = new QVBoxLayout(upper_preview_left_panel);
    upper_preview_left_panel_v_layout->setContentsMargins(0, 0, 0, 0);
    upper_preview_left_panel_v_layout->addWidget(preview_panel);

    // 右侧条码设置面板
    QVBoxLayout *upper_preview_right_panel_v_layout = new QVBoxLayout(upper_preview_right_panel);
    upper_preview_right_panel_v_layout->setContentsMargins(0, 0, 0, 0);
    upper_preview_right_panel_v_layout->addWidget(code_param_panel, 0, Qt::AlignTop);

    // 添加左右面板到上部预览面板，中间用分割线隔开
    upper_preview_panel_h_layout->addWidget(upper_preview_left_panel, 1);
    QFrame *verticalLine = new QFrame(upper_preview_panel);
    verticalLine->setFixedHeight(590);
    verticalLine->setFrameShape(QFrame::VLine);
    verticalLine->setStyleSheet(SS_SeparateLine);
    upper_preview_panel_h_layout->addWidget(verticalLine);
    upper_preview_panel_h_layout->addWidget(upper_preview_right_panel, 2);

    // 将按钮面板和预览面板添加到上部编辑区域
    upper_layout->addWidget(upper_btn_panel);
    upper_layout->addWidget(upper_preview_panel);

    // 下部内容区域（滚动区域）
    QVBoxLayout *lower_content_layout = new QVBoxLayout(lower_content_area);
    lower_content_layout->setContentsMargins(10, 10, 10, 10);
    lower_content_layout->setSpacing(5);

    // 创建列标题行
    QHBoxLayout *column_titles_layout = new QHBoxLayout();
    column_titles_layout->setSpacing(5);

    column_title[0]->setFixedSize(name_btn[0]->width()+10,40);
    column_title[0]->setText(tr("名称"));

    column_title[1]->setFixedSize(input_box[0]->width()-30,40);  //-15
    column_title[1]->setText(tr("内容"));

    column_title[2]->setFixedSize(min_width[0]->width()+25,40); // +35
    column_title[2]->setText(tr("有效长度"));

    column_title[3]->setFixedSize(font_selector[0]->width()-30,40); //-35
    column_title[3]->setText(tr("字体"));

    column_title[4]->setFixedSize(font_size[0]->width()+10,40);
    column_title[4]->setText(tr("字号"));

    column_title[5]->setFixedSize(sta_selector[0]->width()-35,40);
    column_title[5]->setText(tr("状态"));

    column_title[6]->setFixedSize(column_title[5]->width(),40);
    column_title[6]->setText(tr("条码"));

    for (int i = 0; i < 7; i++)
    {
        column_titles_layout->addWidget(column_title[i]);
    }
    lower_content_layout->addLayout(column_titles_layout);

    // 添加每一行的控件
    for (int i = 0; i < MAX_INPUT_PANEL; i++)
    {
        QHBoxLayout *row_layout = new QHBoxLayout();
        row_layout->setSpacing(5);
        row_layout->addWidget(name_btn[i]);
        if (i == MAX_INPUT_PANEL - 1)
        {
            row_layout->addWidget(time_format_selector);
        }
        else
        {
            row_layout->addWidget(input_box[i]);
        }
        row_layout->addWidget(min_width[i]);
        row_layout->addWidget(font_selector[i]);
        row_layout->addWidget(font_size[i]);
        row_layout->addWidget(sta_selector[i]);
        row_layout->addWidget(barcode_checkbox[i]);
        lower_content_layout->addLayout(row_layout);
    }

    // 设置滚动区域
    scrollArea->setWidget(lower_content_area);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // 将上部编辑区域和滚动区域添加到主布局
    main_v_layout->addWidget(upper_edit_area);
    main_v_layout->addWidget(scrollArea);

    // 设置主布局
    main_layout->addLayout(main_v_layout);

    // 初始隐藏部分面板（根据需求）
//    code_param_panel->hide();
//    preview_panel->hide();
}


//作废此布局
void TemplateDialog::setup_layout()
{
    /*
    QHBoxLayout *main_h_layout = new QHBoxLayout();
    QVBoxLayout *left_layout = new QVBoxLayout();
    QVBoxLayout *right_layout = new QVBoxLayout();

    main_h_layout->addLayout(left_layout);
    main_h_layout->addLayout(right_layout);

    main_layout->addLayout(main_h_layout);

    left_layout->addWidget(temp_panel, 0, Qt::AlignHCenter);
    left_layout->addWidget(code_type_panel, 0, Qt::AlignHCenter);
    left_layout->addWidget(code_param_panel, 0, Qt::AlignHCenter);
    right_layout->addWidget(preview_panel, 0, Qt::AlignHCenter);
    right_layout->addWidget(content_panel, 0, Qt::AlignHCenter);

    //content_panel
    QVBoxLayout *content_panel_layout = new QVBoxLayout(content_panel);
    content_panel_layout->setContentsMargins(10,10,10,10);
    QHBoxLayout *tab_btn_h_layout = new QHBoxLayout();
    tab_btn_h_layout->setSpacing(0);
    tab_btn_h_layout->addWidget(text_tab_btn);
    tab_btn_h_layout->addWidget(font_tab_btn);
    content_panel_layout->addLayout(tab_btn_h_layout);

    QHBoxLayout *content_panel_h_layout[8];
    for(int i=0; i<MAX_INPUT_PANEL; i++)
    {
        content_panel_h_layout[i] = new QHBoxLayout();
        content_panel_h_layout[i]->addWidget(name_btn[i]);
        content_panel_h_layout[i]->addWidget(i==MAX_INPUT_PANEL-1?(QWidget*)time_format_selector:(QWidget*)input_box[i]);
        content_panel_h_layout[i]->addWidget(min_width[i]);
        content_panel_h_layout[i]->addWidget(sta_selector[i]);
        content_panel_h_layout[i]->addWidget(font_selector[i]);
        content_panel_h_layout[i]->addWidget(font_size[i]);
        content_panel_layout->addLayout(content_panel_h_layout[i]);
    }

    //bottom_bar
    bottom_bar_layout->addStretch();
    bottom_bar_layout->addWidget(delete_btn, 0, Qt::AlignHCenter);
    bottom_bar_layout->addWidget(copy_btn, 0, Qt::AlignHCenter);
    bottom_bar_layout->addWidget(edit_btn, 0, Qt::AlignHCenter);
    bottom_bar_layout->addWidget(save_btn, 0, Qt::AlignHCenter);
    bottom_bar_layout->addWidget(cancel_btn, 0, Qt::AlignHCenter);
    */
}

void TemplateDialog::set_stylesheet()
{
    upper_edit_area->setStyleSheet(SS_Panels);
    upper_btn_panel->setStyleSheet(SS_Panels);
    upper_preview_panel->setStyleSheet(SS_Panels);
    // 将lower_content_area改为白色背景以解决滚动条样式问题
    lower_content_area->setStyleSheet(SS_Panels);

    //temp_panel->setStyleSheet(SS_Panels);
    //code_type_panel->setStyleSheet(SS_Panels);
    code_param_panel->setStyleSheet(SS_Panels);
    preview_panel->setStyleSheet(SS_Panels);
    //content_panel->setStyleSheet(SS_Panels);

    //temp_panel
    temp_title->setStyleSheet(SS_Title_Label);
    template_selector->setStyleSheet(SS_template_seletor);
    template_selector->setView(new QListView());

    temp_create_btn->setStyleSheet(SS_Normal_Menu_Btn);
    //temp_list->setStyleSheet(SS_List);

    //code_type_panel
    code_type_title->setStyleSheet(SS_Title_Label);
    //code_type_list->setStyleSheet(SS_List);

    //code_param_panel
    code_param_title->setStyleSheet(SS_Title_Label);
    code_fixed_size_checkbox->setStyleSheet(SS_CheckBox);
    code_type_selector->setStyleSheet(SS_template_seletor);
    code_type_selector->setView(new QListView());
    code_seperator_checkbox->setStyleSheet(SS_barcode_checkbox);

    code_size_title->setStyleSheet(SS_Title_Label);
    code_seperator_title->setStyleSheet(SS_Normal_Menu_Label);

    code_width_label->setStyleSheet(SS_Normal_Menu_Label);
    code_width_slider->setStyleSheet(SS_code_width_slider);
    code_width_slider_val->setStyleSheet(SS_Title_Label);

    code_height_label->setStyleSheet(SS_Normal_Menu_Label);
    code_height_slider->setStyleSheet(SS_code_width_slider);
    code_height_slider_val->setStyleSheet(SS_Title_Label);

    code_width_spinbox->setStyleSheet(SS_SpinBox);
    code_height_spinbox->setStyleSheet(SS_SpinBox);


    logo_setting_title->setStyleSheet(SS_Title_Label);
    logo_setting_display->setStyleSheet(SS_Logo_Label);
//    QPixmap logo_pixmap("./images_about/logo.png");
//    QPixmap logo_scaled = logo_pixmap.scaled(QSize(210, 100), Qt::KeepAspectRatio, Qt::SmoothTransformation);
//    logo_setting_display->setPixmap(logo_scaled);
//    logo_setting_display->setFixedSize(210,100);
//    logo_setting_display->setAlignment(Qt::AlignCenter);
    logo_setting_show_checkbox->setStyleSheet(SS_barcode_checkbox);
    //reload_logo_file("./images_about/logo.png");

    logo_width_label->setStyleSheet(SS_Normal_Menu_Label);
    logo_width_slider->setStyleSheet(SS_code_width_slider);
    logo_width_slider_val->setStyleSheet(SS_Title_Label);
    logo_height_label->setStyleSheet(SS_Normal_Menu_Label);
    logo_height_slider->setStyleSheet(SS_code_width_slider);
    logo_height_slider_val->setStyleSheet(SS_Title_Label);

    //preview_panel

    //content_panel
    //text_tab_btn->setStyleSheet(SS_Tab_Btn);
    //font_tab_btn->setStyleSheet(SS_Tab_Btn);

    for(int i = 0;i<7;i++)
    {
         column_title[i]->setStyleSheet(SS_Title_Label);
    }

    for (int i=0; i<MAX_INPUT_PANEL; i++)
    {
        name_btn[i]->setStyleSheet(SS_name_btn);
        (i==MAX_INPUT_PANEL-1)?time_format_selector->setStyleSheet(SS_ComboBox):input_box[i]->setStyleSheet(SS_Input_Box);
        min_width[i]->setStyleSheet(SS_SpinBoxWithLittleArrow);
        sta_selector[i]->setStyleSheet(SS_ComboBox);
        sta_selector[i]->setView(new QListView());
        font_selector[i]->setStyleSheet(SS_ComboBox);
        font_selector[i]->setView(new QListView());
        font_size[i]->setStyleSheet(SS_SpinBoxWithLittleArrow); //SS_SpinBox
        barcode_checkbox[i]->setStyleSheet(SS_barcode_checkbox);
    }
    time_format_selector->setView(new QListView());

    //bottom_bar
    delete_btn->setStyleSheet(SS_Normal_Menu_Btn);
    copy_btn->setStyleSheet(SS_Normal_Menu_Btn);
    edit_btn->setStyleSheet(SS_Primary_Menu_Btn);
    save_btn->setStyleSheet(SS_Primary_Menu_Btn);
    cancel_btn->setStyleSheet(SS_Normal_Menu_Btn);
}

void TemplateDialog::connect_signals_and_slots()
{
    connect(temp_create_btn, &QPushButton::clicked, this, &TemplateDialog::on_temp_create_btn_clicked);
    //connect(temp_list, &QListWidget::itemSelectionChanged, this, &TemplateDialog::on_temp_list_selection_changed);
    connect(template_selector, QOverload<const QString&>::of(&QComboBox::currentIndexChanged),this,&TemplateDialog::on_template_selector_changed);

    //connect(code_type_list, &QListWidget::itemSelectionChanged, this, &TemplateDialog::on_code_type_list_selection_changed);
    connect(code_type_selector, QOverload<const QString&>::of(&QComboBox::currentIndexChanged), this, &TemplateDialog::on_code_selector_changed);
    connect(this, &TemplateDialog::signal_code_selector_changed, this, &TemplateDialog::on_code_selector_changed);

    connect(code_fixed_size_checkbox, &QCheckBox::stateChanged, this, &TemplateDialog::on_code_param_fixed_size_changed);
    connect(code_width_slider, &QSlider::valueChanged, this,  &TemplateDialog::slot_code_width_slider_valueChanged);
    connect(code_height_slider, &QSlider::valueChanged, this, &TemplateDialog::slot_code_height_slider_valueChanged);

    connect(code_seperator_lineedit, &QLineEdit::textChanged, this, &TemplateDialog::slot_code_separator_changed);
    // 为code_seperator_lineedit安装事件过滤器以支持触摸键盘
    code_seperator_lineedit->installEventFilter(this);
    connect(code_seperator_checkbox, &QCheckBox::clicked, this, &TemplateDialog::slot_code_separator_changed);

    connect(logo_width_slider, &QSlider::valueChanged, this,  &TemplateDialog::slot_logo_width_slider_valueChanged);
    connect(logo_height_slider, &QSlider::valueChanged, this, &TemplateDialog::slot_logo_height_slider_valueChanged);
    connect(logo_setting_show_checkbox, &QCheckBox::clicked, this, &TemplateDialog::slot_logo_show_changed);

    for (int i=0; i<MAX_INPUT_PANEL; i++)
    {
        connect(name_btn[i], &QPushButton::clicked, this, &TemplateDialog::on_name_btn_clicked);
        // 为name_btn安装事件过滤器
        name_btn[i]->installEventFilter(this);

        if(i < MAX_INPUT_PANEL - 1)
        {
            connect(input_box[i], &QLineEdit::textChanged, this, &TemplateDialog::on_input_text_changed);
            connect(sta_selector[i], QOverload<const QString&>::of(&QComboBox::currentIndexChanged), this, &TemplateDialog::on_input_sta_selector_changed);
            connect(min_width[i], QOverload<int>::of(&QSpinBox::valueChanged), this,  &TemplateDialog::slot_min_width_spinbox_valueChanged);
            
            // 为input_box和min_width安装事件过滤器以支持触摸键盘
            input_box[i]->installEventFilter(this);
            min_width[i]->installEventFilter(this);
        }
        sta_selector[i]->installEventFilter(this);

        connect(font_selector[i], QOverload<const QString&>::of(&QComboBox::currentIndexChanged), this, &TemplateDialog::on_font_selector_changed);
        // 为font_selector安装事件过滤器
        font_selector[i]->installEventFilter(this);
        connect(font_size[i], QOverload<int>::of(&QSpinBox::valueChanged), this, &TemplateDialog::on_font_size_value_changed);
        
        // 为font_size安装事件过滤器以支持触摸键盘
        font_size[i]->installEventFilter(this);
        connect(barcode_checkbox[i],&QCheckBox::clicked, this, &TemplateDialog::slot_barcode_checked);

    }

    connect(time_format_selector, QOverload<const QString&>::of(&QComboBox::currentIndexChanged),this,&TemplateDialog::on_time_format_selector_changed);
    time_format_selector->installEventFilter(this);

    connect(this, &TemplateDialog::signal_time_format_selector_changed, this, &TemplateDialog::on_time_format_selector_changed);

    connect(sta_selector[MAX_INPUT_PANEL - 1], QOverload<const QString&>::of(&QComboBox::currentIndexChanged),this,&TemplateDialog::on_time_sta_selector_changed);

    connect(delete_btn, &QPushButton::clicked, this, &TemplateDialog::on_delete_btn_clicked);
    connect(copy_btn, &QPushButton::clicked, this, &TemplateDialog::on_copy_btn_clicked);
    connect(edit_btn, &QPushButton::clicked, this, &TemplateDialog::on_edit_btn_clicked);
    connect(save_btn, &QPushButton::clicked, this, &TemplateDialog::on_save_btn_clicked);
    connect(cancel_btn, &QPushButton::clicked, this, &TemplateDialog::on_cancel_btn_clicked);
    connect(m_pTemplatePreview,&TemplatePreview::signalHighLightTextIndex,this,&TemplateDialog::slot_HighLightTextIndex);
    connect(m_pTemplatePreview,&TemplatePreview::signalHighLightTime,this,&TemplateDialog::slot_HighLightTime);
    connect(m_pTemplatePreview,&TemplatePreview::signalBarCodeDlgClosed,this,&TemplateDialog::slot_code_edit_close);
    connect(m_pTemplatePreview,&TemplatePreview::signalHighLightBarCode,this,&TemplateDialog::slot_HighLightBarCode);

    logo_setting_display -> installEventFilter(this);
    this->installWheelBlocker(this);

}

void TemplateDialog::set_template_edit_enable(bool isEnable)
{
    m_bEditEnable = isEnable;

    //按钮可见设置
    delete_btn->setVisible(!isEnable);
    copy_btn->setVisible(!isEnable);
    edit_btn->setVisible(!isEnable);
    save_btn->setVisible(isEnable);
    cancel_btn->setVisible(isEnable);
    temp_create_btn->setVisible(!isEnable);

    //面板可用设置
    //temp_panel->setEnabled(!isEnable);
    template_selector->setEnabled(!isEnable);
    //code_type_panel->setEnabled(isEnable);
    code_param_panel->setEnabled(isEnable);

    code_seperator_lineedit->setEnabled(code_seperator_checkbox->checkState());

    for (int i=0; i<MAX_INPUT_PANEL; i++)
    {
        //text_page
        name_btn[i]->setEnabled(isEnable);

        (i==MAX_INPUT_PANEL-1)?time_format_selector->setEnabled(isEnable):input_box[i]->setEnabled(isEnable);

        min_width[i]->setEnabled(isEnable);
        sta_selector[i]->setEnabled(isEnable);
        //font_page
        font_selector[i]->setEnabled(isEnable);
        font_size[i]->setEnabled(isEnable);

        barcode_checkbox[i]->setEnabled(isEnable);

        //根据sta_selector 状态设置，相应的input_box setEnable()
        if(sta_selector[i]->currentData().toString().compare("hidden")==0)
        {
            if(i < MAX_INPUT_PANEL-1)
            {
               input_box[i]->setEnabled(false);
            }
        }

        //不可用项可显示字段信息，可加入二维码，但不能在标刻预览视窗显示和打印
//        if(sta_selector[i]->currentData().toString().compare("invisible")==0)
//        {
//            if(i < MAX_INPUT_PANEL-1)
//            {
//               input_box[i]->setEnabled(false);
//            }
//        }
    }

    m_pTemplatePreview->setEditMode(isEnable);

    clearNameHighLight();
}

void TemplateDialog::set_code_seperator_enable(int barcodeType)
{
    if((static_cast<int>(QBarcode::QR_Code) == barcodeType) || (static_cast<int>(QBarcode::Data_Matrix) == barcodeType))
    {
       code_seperator_lineedit->setText(edit_temp_attr.code_attr.separator);
       code_seperator_lineedit->setEnabled(true);
       code_seperator_checkbox->setEnabled(true);
    }
    else
    {
        code_seperator_lineedit->setText("");
        code_seperator_checkbox->setChecked(false);
        code_seperator_lineedit->setEnabled(false);
        code_seperator_checkbox->setEnabled(false);
    }
}

void TemplateDialog::set_barcode_checkbox_enable(int barcodeType)
{
    for (int i=0; i<MAX_INPUT_PANEL; i++)
    {
        barcode_checkbox[i]->setEnabled(false);
        barcode_checkbox[i]->setChecked(false);
    }

    if(barcodeType >= static_cast<int>(QBarcode::QR_Code))
    {
       barcode_checkbox[0]->setChecked(true);

       //重置预览编辑状态条码只选病理号
       QVector<int> vec ;
       vec << 0;
       m_pTemplatePreview->setBarCodeTextIndex(vec);
    }

    if((static_cast<int>(QBarcode::QR_Code) == barcodeType) || (static_cast<int>(QBarcode::Data_Matrix) == barcodeType))
    {
        for (int i=1; i<MAX_INPUT_PANEL; i++)
        {
           barcode_checkbox[i]->setEnabled(true);
        }

        QVector<int> vec;
        QStringList stringList = edit_temp_attr.code_attr.text_index.split(';');
        bool timeIsBind = false;
        for (const QString& s : stringList) {
            bool ok;
            int num = s.trimmed().toInt(&ok);
            if (ok)
            {
                barcode_checkbox[num]->setChecked(true);
                if(num == TIME_ALIAS_INDEX)
                {
                   timeIsBind = true;
                }
                else
                {
                   vec << num;
                }
            }
            else
            {
                qWarning() << "Conversion error for:" << s;
            }
        }
        if(vec.size()<1)
        {
            //如果选择二维码类型不为空，默认勾选一个标签
            if(edit_temp_attr.code_attr.type.toInt() < 0)
            {
                vec << -1;
            }
            else
            {
                vec << 0;
            }
        }

        m_pTemplatePreview->setBarCodeTextIndex(vec);
        m_pTemplatePreview->setBarCodeTime(timeIsBind);
    }

}

void TemplateDialog::on_time_format_selector_changed()
{
    edit_temp_attr.panel_attr[MAX_INPUT_PANEL -1].input_text = time_format_selector->currentText();
    edit_temp_attr.panel_attr[MAX_INPUT_PANEL -1].sta = sta_selector[MAX_INPUT_PANEL-1]->currentData().toString();
    set_template_preview_datetime(edit_temp_attr);
}

void TemplateDialog::time_format_selector_OnThread()
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
        on_time_format_selector_changed();
    });
    thread->start();
}

void TemplateDialog::on_input_sta_selector_changed()
{
     QComboBox *input_sta_selector = qobject_cast<QComboBox*>(QObject::sender());

     //根据input_sta_selector 索引号 定位相应的input_box
     for (int i=0; i<MAX_INPUT_PANEL-1; i++)
     {
         if(i == input_sta_selector->objectName().toInt())
         {
             //编辑变量记下所选状态
             edit_temp_attr.panel_attr[i].sta = input_sta_selector->currentData().toString();
             //先把当前edit_temp_attr保存到p_selection_temp_attr
             //copy_editTemplate(&edit_temp_attr,*p_selection_temp_attr);

             input_box[i]->blockSignals(true);
             if(input_sta_selector->currentData().toString().compare("hidden")==0)
             {
                input_box[i]->setEnabled(false);
                input_box[i]->setText("");
                m_pTemplatePreview->setText(i, "");
                qDebug()<<"hidden!";
             }

             if(input_sta_selector->currentData().toString().compare("invisible")==0)
             {
                 qDebug()<<"invisible!";
                input_box[i]->setEnabled(true);
                input_box[i]->setText(edit_temp_attr.panel_attr[i].input_text);

                //隐藏状态:预览不显示文本，但是二维码可见，折中做法，需要修改setText()接口
                m_pTemplatePreview->setText(i, edit_temp_attr.panel_attr[i].input_text);
                QPoint textPos(-9999,-9999);
                m_pTemplatePreview->setTextPosition(i, textPos);
             }

             if((input_sta_selector->currentData().toString().compare("editable")==0)
                 ||
                (input_sta_selector->currentData().toString().compare("fixed")==0))
             {
                 qDebug()<<"editable!";
                input_box[i]->setEnabled(true);
                input_box[i]->setText(edit_temp_attr.panel_attr[i].input_text);
                set_template_preview_text(i,edit_temp_attr);
             }
             input_box[i]->blockSignals(false);
             break;
         }
     }
}

void TemplateDialog::on_time_sta_selector_changed()
{
    bool displayTime = false;

    QString datetimeStr = QDateTime::currentDateTime().toString(time_format_selector->currentText());
    if(sta_selector[MAX_INPUT_PANEL-1]->currentData().toString().compare("display")==0)
    {
        m_pTemplatePreview->setText(MAX_INPUT_PANEL-1, datetimeStr);
        displayTime = true;
    }

    m_pTemplatePreview->setTimeDisplay(displayTime);

    //编辑参数的变更可以由一个专门的函数处理
    edit_temp_attr.panel_attr[MAX_INPUT_PANEL-1].sta = sta_selector[MAX_INPUT_PANEL-1]->currentData().toString();
}

void TemplateDialog::on_font_selector_changed()
{
    QComboBox *font_selector = qobject_cast<QComboBox*>(QObject::sender());

    get_template_preview_position(edit_temp_attr);
    edit_temp_attr.panel_attr[font_selector->objectName().toInt()].font = font_selector->currentText();
    if (font_selector->objectName().toInt() < MAX_INPUT_PANEL-1)
    {
       set_template_preview_text(font_selector->objectName().toInt(),edit_temp_attr);
    }
    else
    {
       set_template_preview_datetime(edit_temp_attr);
    }
}

void TemplateDialog::on_font_size_value_changed(int value)
{
    QSpinBox *font_size = qobject_cast<QSpinBox*>(QObject::sender());

    get_template_preview_position(edit_temp_attr);
    edit_temp_attr.panel_attr[font_size->objectName().toInt()].font_size = value;

    if(font_size->objectName().toInt() < MAX_INPUT_PANEL-1)
    {
        set_template_preview_text(font_size->objectName().toInt(), edit_temp_attr);
    }
    else
    {
        set_template_preview_datetime(edit_temp_attr);
    }

}

void TemplateDialog::on_code_param_fixed_size_changed()
{
    if(code_fixed_size_checkbox->isChecked())
    {
//        code_width_slider->setVisible(false);
//        code_width_slider_val->setVisible(false);
//        code_height_slider->setVisible(false);
//        code_height_slider_val->setVisible(false);
//        code_width_spinbox->setVisible(true);
//        code_height_spinbox->setVisible(true);
    }
    else
    {
        code_width_slider->setVisible(true);
        code_width_slider_val->setVisible(true);
        code_height_slider->setVisible(true);
        code_height_slider_val->setVisible(true);
        code_width_spinbox->setVisible(false);
        code_height_spinbox->setVisible(false);
    }
}

void TemplateDialog::on_text_tab_btn_clicked()
{
    for (int i=0; i<MAX_INPUT_PANEL; i++)
    {
        //text_page
        (i==MAX_INPUT_PANEL-1)?time_format_selector->setVisible(true):input_box[i]->setVisible(true);
        min_width[i]->setVisible(i<MAX_INPUT_PANEL-1?true:false);
        sta_selector[i]->setVisible(true);
        //font_page
        font_selector[i]->setVisible(false);
        font_size[i]->setVisible(false);
    }
}

void TemplateDialog::on_font_tab_btn_clicked()
{
    for (int i=0; i<MAX_INPUT_PANEL; i++)
    {
        //text_page
        (i==MAX_INPUT_PANEL-1)?time_format_selector->setVisible(false):input_box[i]->setVisible(false);
        min_width[i]->setVisible(false);
        sta_selector[i]->setVisible(false);
        //font_page
        font_selector[i]->setVisible(true);
        font_size[i]->setVisible(true);
    }
}

void TemplateDialog::on_name_btn_clicked()
{
    QPushButton *name_btn = qobject_cast<QPushButton*>(sender());

    DialogMsgBox *msg_box = new DialogMsgBox(INPUT_DIALOG);
    msg_box->setTitle(name_btn->text());
    msg_box->setInputBoxText(name_btn->text());
    msg_box->setInputTips(tr("请输入新名称："));

    //点新建按钮后，p_selection_temp_attr被置空，
    //先把当前edit_temp_attr保存到p_selection_temp_attr
    copy_editTemplate(&edit_temp_attr,*p_selection_temp_attr);

    if(msg_box->exec() == QDialog::Accepted)
    {
        name_btn->setText(msg_box->getInputText());

        //修改名字后，刷新到二维码编辑器里
        int name_Id = name_btn->objectName().toInt();
        QString name_text = name_btn->text();
        p_selection_temp_attr->panel_attr[name_Id].name = name_text;
        copy_editTemplate(p_selection_temp_attr,edit_temp_attr);
        m_pTemplatePreview->setTextAlias(name_Id, name_text);
    }
}

void TemplateDialog::on_input_text_changed(QString text)
{
    QLineEdit *input_box = qobject_cast<QLineEdit*>(QObject::sender());

    int input_box_id = input_box->objectName().toInt();
    qDebug() << QString("on_input_text_changed.input_box_id=%1").arg(input_box_id) <<"text changed:" << text;
    qDebug() << QString("input_box[%1]").arg(input_box_id) <<"text changed:" << text;

    get_template_preview_position(edit_temp_attr);
    edit_temp_attr.panel_attr[input_box_id].input_text = text;

    set_template_preview_text(input_box_id,edit_temp_attr);

    set_template_preview_barcode(edit_temp_attr);

    //设置分隔符到模版里面  放在set_template_preview_barcode执行
    //slot_code_separator_changed();

    //不可用项[hidden]可显示字段信息，可加入二维码，但不能在标刻预览视窗显示和打印
    qDebug() << QString("on_input_text_changed[%1].sta====%2").arg(input_box_id).arg(edit_temp_attr.panel_attr[input_box_id].sta);
    if(edit_temp_attr.panel_attr[input_box_id].sta.compare("hidden") == 0)
    {
         m_pTemplatePreview->setText(input_box_id, "");
    }

    //先把当前edit_temp_attr保存到p_selection_temp_attr
    //copy_editTemplate(&edit_temp_attr,*p_selection_temp_attr);
}


void TemplateDialog::on_temp_create_btn_clicked()
{
    m_isNewTemplate = true;
    m_StrNewTempName = "";

    m_StrLastSelectTemp = template_selector->currentText();
    //temp_list->clearSelection();
    //禁止刷新模板，赋值edit_temp_attr
    template_selector->blockSignals(true);
    template_selector->setCurrentIndex(-1);
    template_selector->blockSignals(false);

    set_template_edit_enable(true);

    //清空选择模板内容
    *p_selection_temp_attr = {};
    copy_editTemplate(p_selection_temp_attr,edit_temp_attr);
    //清空input_box
    for (int i=0; i<MAX_INPUT_PANEL - 1; i++)
    {
        name_btn[i] ->setText("");
        input_box[i]->setText("");
        min_width[i]->setValue(-1);
        font_size[i]->setValue(18);
        edit_temp_attr.panel_attr[i].font_size = font_size[i]->value();
        font_selector[i]->setCurrentText(edit_temp_attr.panel_attr[i].font);
        sta_selector[i]->setCurrentIndex(0);

        if(i == 2)
        {
            sta_selector[i]->setCurrentText(tr("固定"));
        }
        else if(i > 2)
        {
            sta_selector[i]->setCurrentText(tr("不可用"));
        }

        m_pTemplatePreview->setTextPosition(i,QPoint(0,0));
    }

    sta_selector[MAX_INPUT_PANEL-1]->setCurrentText(tr("隐藏"));

    name_btn[MAX_INPUT_PANEL-1]->setText("");
   //所有(包括时间)字体默认"Microsoft YaHei",大小默认18
    font_size[MAX_INPUT_PANEL - 1]->setValue(18);
    font_selector[MAX_INPUT_PANEL - 1]->blockSignals(true);
    font_selector[MAX_INPUT_PANEL - 1]->setCurrentText(edit_temp_attr.panel_attr[MAX_INPUT_PANEL - 1].font);
    font_selector[MAX_INPUT_PANEL - 1]->blockSignals(false);
    edit_temp_attr.panel_attr[MAX_INPUT_PANEL - 1].font_size = font_size[MAX_INPUT_PANEL - 1]->value();

    //时间位置初始化
    QPoint timePos(0,0);
    m_pTemplatePreview->setTimePosition(timePos);
    m_pTemplatePreview->setTimeFont(QFont(edit_temp_attr.panel_attr[MAX_INPUT_PANEL-1].font, edit_temp_attr.panel_attr[MAX_INPUT_PANEL-1].font_size, QFont::Normal));

    //显示时间
    on_time_sta_selector_changed();

    code_width_slider->setValue(1);
    code_height_slider->setValue(1);

    //条码大小调节器初始化
    code_width_slider_val->setText(QString::number(code_width_slider->value()));
    code_height_slider_val->setText(QString::number(code_height_slider->value()));
    edit_temp_attr.code_attr.width_factor = 1;
    edit_temp_attr.code_attr.height_factor = 1;
    m_pTemplatePreview->setBarCodePosition(QPoint(0,0));

    //logo大小调节器初始化
    logo_width_slider->setValue(1);
    logo_height_slider->setValue(1);
    logo_width_slider_val->setText(QString::number(logo_width_slider->value()));
    logo_height_slider_val->setText(QString::number(logo_height_slider->value()));
    edit_temp_attr.logo_attr.width_factor = 1;
    edit_temp_attr.logo_attr.height_factor = 1;
    logo_setting_display->clear();
    logo_setting_display->setObjectName("");
    logo_setting_show_checkbox->setChecked(false);

    //条形码类型默认空
    //code_type_list->setCurrentRow(0);
    //code_type_selector->blockSignals(true);
    code_type_selector->setCurrentIndex(0);
    //code_type_selector->blockSignals(false);

    if(m_bEditEnable)
    {
       set_barcode_checkbox_enable(code_type_selector->currentData(Qt::UserRole).toInt());
       m_pTemplatePreview->setLogoPosition(QPoint(0,0));
    }

    set_template_preview_logo(edit_temp_attr);

}

void TemplateDialog::on_template_selector_changed()
{

    qDebug() << "on_template_selector_changed=======>>>>>>>" ;
    //模版里面为空退出
    if(template_selector->currentIndex() < 0)
    {
        return;
    }
    QString current_temp_name = template_selector->currentText();
    QStringList default_temp_names = ReadTemplateNames(DEFAULT_TEMPLATES_DIR);
    qDebug() << "on_template_selector_changed.选择用的模版==" << current_temp_name;
    if(default_temp_names.contains(current_temp_name))
    {//固定模板，且不可删除和编辑
        load_template_param(DEFAULT_TEMPLATES_DIR, current_temp_name);
        delete_btn->setEnabled(false);
        edit_btn->setEnabled(false);
    }
    else
    {
        load_template_param(USER_TEMPLATES_DIR, current_temp_name);
        delete_btn->setEnabled(true);
        edit_btn->setEnabled(true);
    }

    copy_editTemplate(p_selection_temp_attr,edit_temp_attr);
    set_template_preview_position(edit_temp_attr);

    //在load_template_param()里已经通过code_type_list->setCurrentItem(item)
    //再执行函数：on_code_type_list_selection_changed();
    //on_code_type_list_selection_changed();
    //on_code_selector_changed();

    QTimer::singleShot(50, this, [this]() {
      code_selector_OnThread();
    });
    
    // 加载完成后延时恢复触摸键盘功能
    QTimer::singleShot(300, this, [this]() { m_enableTouchKeyboard = true; });
}

void TemplateDialog::template_selector_OnThread()
{
    // 先禁用触摸键盘，确保在加载模板过程中不会弹出
    m_enableTouchKeyboard = false;
    
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
        on_template_selector_changed();
    });
    thread->start();
}

void TemplateDialog::on_temp_list_selection_changed()
{

}

void TemplateDialog::set_template_preview_barcode(const TemplateAttr  & temp_attr)
{ 
    CodeAttr  code_attr = temp_attr.code_attr;
    QVector<int> vec;
    QStringList stringList = code_attr.text_index.split(';');
    bool timeIsBind = false;
    for (const QString& s : stringList) {
        bool ok;
        int num = s.trimmed().toInt(&ok);
        if (ok) {
            if(num == TIME_ALIAS_INDEX)
            {
               timeIsBind = true;
            }
            else
            {
               vec << num;
            }
        } else {
            qWarning() << "Conversion error for:" << s;
        }
    }
    if(vec.size()<1)
    {
        //如果选择二维码类型不为空，默认勾选一个标签
        if(temp_attr.code_attr.type.toInt() < 0)
        {
            vec << -1;
        }
        else
        {
            vec << 0;
        }
    }

    if(temp_attr.code_attr.type.toInt() < 0)
    {
        //条形码显示效果跟setBarCodeSize、setBarCodeType的调用顺序有关
        m_pTemplatePreview->setBarCodeVisible(false);
        m_pTemplatePreview->setBarCodeType(QBarcode::BarCodeType(0));
        m_pTemplatePreview->setBarCodeSize(1, 1);
    }
    else
    {
        m_pTemplatePreview->setBarCodeVisible(true);
        m_pTemplatePreview->setBarCodeType(QBarcode::BarCodeType(code_attr.type.toInt()));
        m_pTemplatePreview->setBarCodePosition(QPoint(code_attr.pos_x,code_attr.pos_y));


        code_width_slider->setRange(edit_temp_attr.code_attr.width_factor,300);
        code_height_slider->setRange(edit_temp_attr.code_attr.height_factor,300);

        if(m_bEditEnable)
        {

            int barcode_min_width , barcode_min_height ;
            int barcode_width, barcode_height;
            m_pTemplatePreview->getBarCodeMinSize(barcode_min_width, barcode_min_height);

            edit_temp_attr.code_attr.width = code_width_slider->value();
            edit_temp_attr.code_attr.height = code_height_slider->value();

            //二维码最小宽高存在width_factor、height_factor 变量
            edit_temp_attr.code_attr.width_factor = barcode_min_width;
            edit_temp_attr.code_attr.height_factor = barcode_min_height;
        }

        m_pTemplatePreview->setBarCodeSize(edit_temp_attr.code_attr.width, code_attr.height);

        m_pTemplatePreview->setBarCodeTextIndex(vec);
        m_pTemplatePreview->setBarCodeTextSeparator(code_attr.separator_ischecked,code_attr.separator);

        //修复：点击仿真界面，条形/二维码总是显示编辑前选择的条码类型问题
        m_pTemplatePreview->setText(0,"0");
        int max_width = temp_attr.panel_attr[0].min_width;
        m_pTemplatePreview->setText(0,input_box[0]->text().left(max_width));

    }
    //设置条形码是否绑定时间信息
    m_pTemplatePreview->setBarCodeTime(timeIsBind);

    //设置分隔符到模版里面
    slot_code_separator_changed();
}

void TemplateDialog::set_template_preview_text(int inputId,const TemplateAttr  & temp_attr)
{
     QPoint textPos = QPoint(temp_attr.panel_attr[inputId].pos_x, temp_attr.panel_attr[inputId].pos_y);

     //恢复隐藏字段(invisible)设置的坐标(-9999,-9999)
     if(textPos.x()< 0 || textPos.y() < 0)
     {
         m_pTemplatePreview->setTextPosition(inputId, QPoint(0,0));
     }
     else
     {
         m_pTemplatePreview->setTextPosition(inputId, textPos);
     }

     int font_size = temp_attr.panel_attr[inputId].font_size;
     if(font_size < 9)
     {
         font_size = 9;
     }
     m_pTemplatePreview->setTextFont(inputId, QFont(temp_attr.panel_attr[inputId].font,font_size, QFont::Normal));

     int max_width = temp_attr.panel_attr[inputId].min_width;

     m_pTemplatePreview->setText(inputId, temp_attr.panel_attr[inputId].input_text.left(max_width));

    if(temp_attr.panel_attr[inputId].sta.compare("invisible")==0)
    {
        //隐藏状态:预览不显示文本，但是二维码可见，折中做法，需要修改setText()接口
        QPoint textPos(-9999,-9999);
        m_pTemplatePreview->setTextPosition(inputId, textPos);
    }

     m_pTemplatePreview->setTextAlias(inputId, temp_attr.panel_attr[inputId].name);
}

void TemplateDialog::set_template_preview_datetime(const TemplateAttr  & temp_attr)
{
    //datetime
    QPoint timePos(QPoint(temp_attr.panel_attr[MAX_INPUT_PANEL-1].pos_x, temp_attr.panel_attr[MAX_INPUT_PANEL-1].pos_y));
    m_pTemplatePreview->setTimePosition(timePos);
    qDebug() << "set_template_preview_datetime.timePos================================>>>>" <<  timePos;

    m_pTemplatePreview->setTimeFont(QFont(temp_attr.panel_attr[MAX_INPUT_PANEL-1].font, temp_attr.panel_attr[MAX_INPUT_PANEL-1].font_size, QFont::Normal));
    QString dt_format = temp_attr.panel_attr[MAX_INPUT_PANEL -1].input_text;
    m_pTemplatePreview->setTimeFormat(dt_format);
    QString task_datetime = QDateTime::currentDateTime().toString(dt_format);
    //m_pTemplatePreview->setText(MAX_INPUT_PANEL-1, task_datetime);
    m_pTemplatePreview->setTimeAlias(temp_attr.panel_attr[MAX_INPUT_PANEL-1].name);

    bool displayTime = false;
    if(temp_attr.panel_attr[MAX_INPUT_PANEL-1].sta.compare("display") == 0)
    {
        displayTime = true;
        m_pTemplatePreview->setText(MAX_INPUT_PANEL-1, task_datetime);
    }
    else
    {
        m_pTemplatePreview->setText(MAX_INPUT_PANEL-1, "");
    }

    m_pTemplatePreview->setTimeDisplay(displayTime);
}


void TemplateDialog::set_template_preview_logo(const TemplateAttr  & temp_attr)
{
    QPoint logoPos(temp_attr.logo_attr.pos_x, temp_attr.logo_attr.pos_y);

    int logo_min_width = 16 , logo_min_height = 16;
    int logo_width, logo_height;
    logo_width = logo_min_width * (1 + (temp_attr.logo_attr.width_factor - 1) * 0.2);
    logo_height = logo_min_height * (1 + (temp_attr.logo_attr.height_factor - 1) * 0.2);

//    int logo_width  = temp_attr.logo_attr.width;
//    int logo_height = temp_attr.logo_attr.height;

    if(m_bEditEnable)
    {
//        edit_temp_attr.logo_attr.width = logo_width;
//        edit_temp_attr.logo_attr.height = logo_height;
        logoPos = m_pTemplatePreview->getLogoPosition();
    }

    QString temp_logo_file = logo_setting_display->objectName();
//    if (temp_logo_file.isEmpty())
//       return;
    qDebug() << "set_template_preview_logo====>>>>" <<temp_logo_file;
    qDebug() << "set_template_preview_logo.logoPos()======>>>" << logoPos;

    m_pTemplatePreview->setLogoSize(logo_width, logo_height);

    m_pTemplatePreview->setLogoPixMap(temp_logo_file);
    m_pTemplatePreview->setLogoPosition(logoPos);
    m_pTemplatePreview->setLogoDisplay(temp_attr.logo_attr.is_display);

    edit_temp_attr.logo_attr.width = m_pTemplatePreview->getlogoSize().width();
    edit_temp_attr.logo_attr.height = m_pTemplatePreview->getlogoSize().height();
}


void TemplateDialog::set_slider_range()
{

}

void TemplateDialog::set_template_preview_position(const TemplateAttr  & temp_attr)
{
    QPoint barCodePoint(temp_attr.code_attr.pos_x,temp_attr.code_attr.pos_y);
    m_pTemplatePreview->setBarCodePosition(barCodePoint);

    //qDebug() << "set_template_preview_position.barCodePoint==" << barCodePoint;

    //设置文本位置信息
    for (int i=0; i<MAX_INPUT_PANEL -1; i++)
    {
        QPoint textPoint(temp_attr.panel_attr[i].pos_x,temp_attr.panel_attr[i].pos_y);

        if(temp_attr.panel_attr[i].sta.compare("invisible")==0)
        {
            //隐藏状态:预览不显示文本，但是二维码可见，折中做法，需要修改setText()接口
            textPoint = QPoint(-9999,-9999);
        }

        m_pTemplatePreview->setTextPosition(i, textPoint);
    }
    //设置时间位置信息
    QPoint timePoint(temp_attr.panel_attr[MAX_INPUT_PANEL -1].pos_x,temp_attr.panel_attr[MAX_INPUT_PANEL -1].pos_y);
    m_pTemplatePreview->setTimePosition(timePoint);

    m_pTemplatePreview->setDirect(TemplatePreview::Direct(temp_attr.code_attr.direct_val));

}

void TemplateDialog::get_template_preview_position(TemplateAttr  & temp_attr)
{
    for (int i=0; i<MAX_INPUT_PANEL-1; i++)
    {
        //获取文本、时间位置信息
        QPoint textPoint = m_pTemplatePreview->getTextPosition(i);
        temp_attr.panel_attr[i].pos_x = textPoint.x();
        temp_attr.panel_attr[i].pos_y = textPoint.y();
    }

    QPoint timePoint = m_pTemplatePreview->getTimePosition();
    qDebug() << "get_template_preview_position,timePoint==" << timePoint;
    temp_attr.panel_attr[MAX_INPUT_PANEL-1].pos_x = timePoint.x();
    temp_attr.panel_attr[MAX_INPUT_PANEL-1].pos_y = timePoint.y();

    qDebug() << "get_template_preview_position,temp_attr.panel_attr[MAX_INPUT_PANEL-1].pos_x==" << temp_attr.panel_attr[MAX_INPUT_PANEL-1].pos_x;
    qDebug() << "get_template_preview_position,temp_attr.panel_attr[MAX_INPUT_PANEL-1].pos_y==" << temp_attr.panel_attr[MAX_INPUT_PANEL-1].pos_y;

    //获取条形码位置信息
    QPoint barCodePoint = m_pTemplatePreview->getBarCodePosition();
    temp_attr.code_attr.pos_x = barCodePoint.x();
    temp_attr.code_attr.pos_y = barCodePoint.y();

    //位置限制
    if(temp_attr.code_attr.pos_x < 0)
    {
        temp_attr.code_attr.pos_x = 0;
    }
    //限制Y坐标
    if(temp_attr.code_attr.pos_y < 0)
    {
        temp_attr.code_attr.pos_y = 0;
    }

    if(temp_attr.code_attr.pos_y > MAX_PREVIEW_TEXT_HEIGHT)
    {
        temp_attr.code_attr.pos_y = MAX_PREVIEW_TEXT_HEIGHT;
    }

}

QString TemplateDialog::get_barCode_textIndex()
{
    //取条形码索引号
    QVector<int> vec = m_pTemplatePreview->getBarCodeTextIndex();
    qDebug() << "get_barCode_textIndex==" << vec;

    //'ini文件中不支持字符串以','作为分隔符，故以下以';'作为分隔符'
    QString text_index;
    for (int i = 0; i < vec.count(); i++)
    {
        text_index += (i == vec.count() -1) ? QString::number(vec[i]) : QString::number(vec[i]) + ";";
    }

    bool timeIsBind = m_pTemplatePreview->getBarCodeTime();
    if(timeIsBind)
    {
        //15索引号表示二维码有时间信息显示
        text_index += QString(";%1").arg(TIME_ALIAS_INDEX);
    }

    return text_index;
}

void TemplateDialog::copy_editTemplate(const TemplateAttr* source, TemplateAttr& destination) {
    if (!source) return;

    destination = {};
    destination.name = source->name;
    destination.code_attr = source->code_attr;
    destination.logo_attr = source->logo_attr;

    for (int i = 0; i < MAX_INPUT_PANEL; ++i) {
        destination.panel_attr[i] = source->panel_attr[i];
    }
}

void TemplateDialog::on_code_selector_changed()
{
    qDebug() << "on_code_selector_changed=======>>>>>>>" ;
    edit_temp_attr.code_attr.type = code_type_selector->currentData(Qt::UserRole).toString();

    get_template_preview_position(edit_temp_attr);

//    for (int i=0; i<MAX_INPUT_PANEL -1; i++)
//    {
//        set_template_preview_text(i,edit_temp_attr);
//    }

    //设置时间
    set_template_preview_datetime(edit_temp_attr);

    //保持二维码编辑器勾选的标签索引
    //edit_temp_attr.code_attr.text_index = get_barCode_textIndex();
    qDebug() << "保持二维码编辑器勾选的标签索引==" <<  edit_temp_attr.code_attr.text_index;

    set_template_preview_barcode(edit_temp_attr);

    QTimer::singleShot(50, this, [this]() {
      time_format_selector_OnThread();
    });

    if(m_bEditEnable)
    {
       set_barcode_checkbox_enable(code_type_selector->currentData(Qt::UserRole).toInt());
       set_code_seperator_enable(code_type_selector->currentData(Qt::UserRole).toInt());
    }

    //放到函数后面再加载预览文本，否则第一个文本来不及加载
    for (int i=0; i<MAX_INPUT_PANEL -1; i++)
    {
        if(edit_temp_attr.panel_attr[i].sta.compare("invisible")==0)
        {
           //m_pTemplatePreview->setText(i, "");
           //set_template_preview_text(i,edit_temp_attr);
           //这里不能改变位置(-9999,-9999)写到配置文件里?m_pTemplatePreview->getTextPosition(i)要特殊处理
           QPoint textPos(-9999,-9999);
           m_pTemplatePreview->setTextPosition(i, textPos);
        }
        else
        {
           set_template_preview_text(i,edit_temp_attr);
        }
    }

}

void TemplateDialog::code_selector_OnThread()
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
        on_code_selector_changed();

        QString current_temp_name = template_selector->currentText();
        QString temp_logo_file = QString("%1/%2/%3")
                .arg(USER_TEMPLATES_DIR)
                .arg(current_temp_name)
                .arg(edit_temp_attr.logo_attr.file_name);
        reload_logo_file(temp_logo_file);
        set_template_preview_logo(edit_temp_attr);
    });
    thread->start();
}

void TemplateDialog::on_code_type_list_selection_changed()
{
//    edit_temp_attr.code_attr.type = code_type_list->currentItem()->data(Qt::UserRole).toString();

//    get_template_preview_position(edit_temp_attr);

//    for (int i=0; i<MAX_INPUT_PANEL -1; i++)
//    {
//        set_template_preview_text(i,edit_temp_attr);
//    }
//    //设置时间
//    set_template_preview_datetime(edit_temp_attr);

//    //保持二维码编辑器勾选的标签索引
//    //edit_temp_attr.code_attr.text_index = get_barCode_textIndex();
//    qDebug() << "保持二维码编辑器勾选的标签索引==" <<  edit_temp_attr.code_attr.text_index;

//    set_template_preview_barcode(edit_temp_attr);

//    on_time_format_selector_changed();
//    set_slider_range();
}

void TemplateDialog::on_delete_btn_clicked()
{
    QString current_temp_name = template_selector->currentText();
    QStringList default_temp_names = ReadTemplateNames(DEFAULT_TEMPLATES_DIR);

    if(default_temp_names.contains(current_temp_name))
    {//固定模板，不可删除
        return;
    }

    //删除当前模板
    QDir temp_dir(QString("%1/%2").arg(USER_TEMPLATES_DIR,current_temp_name));
    DialogMsgBox *msg_box = new DialogMsgBox(ASK_DIALOG);
    msg_box->setContent(tr("是否删除当前模板？"));
    if(msg_box->exec()==QDialog::Accepted)
    {

//        for (int i = 0; i < template_selector->count(); ++i) {
//            QString itemText = template_selector->itemText(i);
//            if (itemText == current_temp_name)
//            {
//                temp_dir.removeRecursively();
//                template_selector->removeItem(i);
//                break;
//            }
//        }

        if (temp_dir.exists()) {
            if (temp_dir.removeRecursively()) {
                // 刷新模板选择器
                refreshTemplateSelector();
            } else {
                // 显示删除失败消息
                DialogMsgBox *error_box = new DialogMsgBox(INFO_DIALOG);
                error_box->setContent(tr("删除模板失败，请检查权限"));
                error_box->exec();
            }
        } else {
            // 显示目录不存在消息
            DialogMsgBox *error_box = new DialogMsgBox(INFO_DIALOG);
            error_box->setContent(tr("模板目录不存在"));
            error_box->exec();
        }

    }
}


// 添加刷新模板选择器的辅助函数
void TemplateDialog::refreshTemplateSelector()
{
    template_selector->clear();
    load_template_names(DEFAULT_TEMPLATES_DIR);
    load_template_names(USER_TEMPLATES_DIR);
    template_selector->setCurrentIndex(0);
}

void TemplateDialog::on_copy_btn_clicked()
{
    if(template_selector->currentIndex() < 0)
    {
        return;
    }

    QString template_name;

    //保存名称对话框
    input_temp_name:
    DialogMsgBox *msg_box = new DialogMsgBox(INPUT_DIALOG);
    msg_box->setInputTips(tr("输入新的模板名称："));

    if(msg_box->exec()!=QDialog::Accepted)
    {
        return ;
    }
    template_name = msg_box->getInputText();

    if(template_name.isEmpty() || template_name.contains('/') || template_name.contains(':') || template_name.contains('*')
            || template_name.contains('?') || template_name.contains('<') || template_name.contains('>')
            || template_name.contains('|') || template_name.contains("\"") || template_name.contains("\\")|| template_name.contains("."))
    {
        DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
        msg_box->setContent(tr("文件名不能为空或特殊字符！"));
        msg_box->exec();
        goto input_temp_name;
    }

    int index = -1;
    for (int i = 0; i < template_selector->count(); ++i) {
        QString itemText = template_selector->itemText(i);
        //都转成小写后对比
        if (itemText.toLower().compare(template_name.toLower())==0)
        {
            index = i;
            break;
        }
    }

    if(index > -1)
    {
        DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
        msg_box->setContent(tr("该名字已存在！"));
        msg_box->exec();
        goto input_temp_name;
    }
    else
    {
//        template_selector->blockSignals(true);
//        template_selector->addItem(template_name);
//        template_selector->setCurrentText(template_name);
//        template_selector->blockSignals(false);

        //考虑到新建和复制的情况
        m_StrNewTempName = template_name;
        //保存到文件
        write_template_param(USER_TEMPLATES_DIR, template_name);

        template_selector->addItem(template_name);
        template_selector->setCurrentText(template_name);

        m_StrNewTempName = "";
    }
}

void TemplateDialog::on_edit_btn_clicked()
{
    m_isNewTemplate = false;
    m_StrNewTempName = "";
    QString current_temp_name = template_selector->currentText();
    QStringList default_temp_names = ReadTemplateNames(DEFAULT_TEMPLATES_DIR);
    if(default_temp_names.contains(current_temp_name))
    {//固定模板，不可编辑
        return;
    }
    set_template_edit_enable(true);

    copy_editTemplate(p_selection_temp_attr,edit_temp_attr);
    //这里不要设置位置，会导致被隐藏的字段显示到预览里面
    set_template_preview_position(edit_temp_attr);

    if(m_bEditEnable)
    {
       set_barcode_checkbox_enable(code_type_selector->currentData(Qt::UserRole).toInt());
    }

}

void TemplateDialog::on_save_btn_clicked()
{

    QString template_name;
    //if(temp_list->selectedItems().isEmpty())//默认不选中的情况是新建
    if(template_selector->currentIndex() < 0)
    {
        //保存名称对话框
        input_temp_name:
        DialogMsgBox *msg_box = new DialogMsgBox(INPUT_DIALOG);
        msg_box->setInputTips(tr("输入新的模板名称："));

        if(msg_box->exec()!=QDialog::Accepted)
        {
            return ;
        }
        template_name = msg_box->getInputText();

        if(template_name.isEmpty() || template_name.contains('/') || template_name.contains(':') || template_name.contains('*')
                || template_name.contains('?') || template_name.contains('<') || template_name.contains('>')
                || template_name.contains('|') || template_name.contains("\"") || template_name.contains("\\")|| template_name.contains("."))
        {
            DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
            msg_box->setContent(tr("文件名不能为空或特殊字符！"));
            msg_box->exec();
            goto input_temp_name;
        }

        //获取新建的模版名称
        m_StrNewTempName = template_name;

        int index = -1;
        for (int i = 0; i < template_selector->count(); ++i) {
            QString itemText = template_selector->itemText(i);
            //都转成小写后对比
            if (itemText.toLower().compare(template_name.toLower())==0)
            {
                index = i;
                break;
            }
        }

        if(index > -1)
        {
            DialogMsgBox *msg_box = new DialogMsgBox(INFO_DIALOG);
            msg_box->setContent(tr("该名字已存在！"));
            msg_box->exec();
            goto input_temp_name;
        }
        else
        {
            template_selector->addItem(template_name);
        }
    }
    else
    {
        template_name = template_selector->currentText();
    }

    // 禁用触摸键盘
   m_enableTouchKeyboard = false;
    // 保存到文件
   write_template_param(USER_TEMPLATES_DIR, template_name);

   template_selector->blockSignals(true);
   template_selector->setCurrentText(template_name);
   template_selector->blockSignals(false);

   //重新刷新模版参数
   QTimer::singleShot(50, this, [this]() {
     template_selector_OnThread();
   });

   set_template_edit_enable(false);

}

void TemplateDialog::on_cancel_btn_clicked()
{
    // 禁用触摸键盘
    m_enableTouchKeyboard = false;
    
    //读取最后选择的模板名
    //QString name = edit_temp_attr.name;
    qDebug() << "on_cancel_btn_clicked.edit_temp_attr.name======>>>>>>>" <<  edit_temp_attr.name;

    template_selector->blockSignals(true);
    template_selector->setCurrentText(m_StrLastSelectTemp);
    template_selector->blockSignals(false);

    //重新刷新模版参数
    QTimer::singleShot(50, this, [this]() {
      template_selector_OnThread();
    });

    set_template_edit_enable(false);
}

void TemplateDialog::on_back_btn_clicked()
{
    // 禁用触摸键盘
    m_enableTouchKeyboard = false;

    //如果正在编辑中提示一下
    if(save_btn->isVisible())
    {
        DialogMsgBox *msg_box = new DialogMsgBox(ASK_DIALOG);
        msg_box->setContent(tr("有模板在编辑状态。\n是否确定退出？"));
        if(msg_box->exec()==QDialog::Accepted)
        {
            this->close();
        }
    }
    else
    {
        this->close();
    }
}

void TemplateDialog::slot_HighLightTextIndex(int index)
{
    qDebug() << "select index = " << index;
    for (int i=0; i<MAX_INPUT_PANEL-1; i++)
    {
        if(i != index)
        {
           name_btn[i]->setChecked(false);
        }
        else
        {
            name_btn[i]->setChecked(true);
            name_btn[MAX_INPUT_PANEL-1]->setChecked(false);
        }
    }

}

void TemplateDialog::slot_HighLightTime()
{
    name_btn[MAX_INPUT_PANEL-1]->setChecked(true);
}

void TemplateDialog::slot_HighLightBarCode()
{
    //点二维码，焦点定位到时间
    name_btn[MAX_INPUT_PANEL-1]->setChecked(true);
}

void TemplateDialog::slot_code_width_slider_valueChanged(int value)
{
    code_width_slider_val->setText(QString::number(value));
    if(
            (code_type_selector->currentData(Qt::UserRole).toInt() == static_cast<int>(QBarcode::QR_Code))
              ||
            (code_type_selector->currentData(Qt::UserRole).toInt() == static_cast<int>(QBarcode::Data_Matrix))
      )
    {
       code_height_slider->setValue(value);
    }

    get_template_preview_position(edit_temp_attr);
    //edit_temp_attr.code_attr.width_factor = code_width_slider_val->text().toUInt();
    //edit_temp_attr.code_attr.height_factor = code_height_slider_val->text().toUInt();
    set_template_preview_barcode(edit_temp_attr);
}

void TemplateDialog::slot_code_height_slider_valueChanged(int value)
{
    code_height_slider_val->setText(QString::number(value));
    if(
          (code_type_selector->currentData(Qt::UserRole).toInt() == static_cast<int>(QBarcode::QR_Code))
            ||
          (code_type_selector->currentData(Qt::UserRole).toInt() == static_cast<int>(QBarcode::Data_Matrix))
      )
    {
        code_width_slider->setValue(value);
    }

    get_template_preview_position(edit_temp_attr);
    //edit_temp_attr.code_attr.width_factor = code_width_slider_val->text().toUInt();
    //edit_temp_attr.code_attr.height_factor = code_height_slider_val->text().toUInt();
    set_template_preview_barcode(edit_temp_attr);
}

void TemplateDialog::slot_code_separator_changed()
{
    //取分隔符
    QString separator = code_seperator_lineedit->text();
    bool isChecked = code_seperator_checkbox->checkState();
    m_pTemplatePreview->setBarCodeTextSeparator(isChecked,separator);

    code_seperator_lineedit->setEnabled(isChecked);
}

void TemplateDialog::slot_barcode_checked()
{
    QVector<int> vec;
    bool timeIsBind = false;
    for (int i=0; i<MAX_INPUT_PANEL; i++)
    {
        if(barcode_checkbox[i]->isChecked())
        {
            if(i < TIME_ALIAS_INDEX)
            {
                vec << i;
            }
            else
            {
                timeIsBind = true;
            }
        }
    }

    if(vec.size()<1)
    {
        //如果选择二维码类型不为空，默认勾选一个标签
        if(edit_temp_attr.code_attr.type.toInt() < 0)
        {
            vec << -1;
        }
        else
        {
            vec << 0;
        }
    }

    m_pTemplatePreview->setBarCodeTextIndex(vec);
    m_pTemplatePreview->setBarCodeTime(timeIsBind);

    edit_temp_attr.code_attr.text_index = get_barCode_textIndex();

    set_template_preview_barcode(edit_temp_attr);
}

void TemplateDialog::slot_code_edit_close()
{
    //取分隔符
    QString separator ;
    bool isChecked;
    m_pTemplatePreview->getBarCodeTextSeparator(isChecked,separator);
    edit_temp_attr.code_attr.separator_ischecked = isChecked;
    edit_temp_attr.code_attr.separator = separator;

    //保持二维码编辑器勾选的标签索引
    edit_temp_attr.code_attr.text_index = get_barCode_textIndex();
    qDebug() << "保持二维码编辑器勾选的标签索引==" <<  edit_temp_attr.code_attr.text_index;

    set_template_preview_barcode(edit_temp_attr);
}

void TemplateDialog::slot_min_width_spinbox_valueChanged(int value)
{
      QSpinBox *min_width = qobject_cast<QSpinBox*>(QObject::sender());
      int min_width_index = min_width->objectName().toInt();
      edit_temp_attr.panel_attr[min_width_index].min_width = value;
}


void TemplateDialog::slot_logo_width_slider_valueChanged(int value)
{
    // 宽度 slider 改变
     logo_width_slider_val->setText(QString::number(value));

     // 计算高度（保持比例）
     int newHeight = qRound(value / LOGO_ASPECT_RATIO);
     if (newHeight < logo_height_slider->minimum()) newHeight = logo_height_slider->minimum();
     if (newHeight > logo_height_slider->maximum()) newHeight = logo_height_slider->maximum();

     // 避免死循环：仅在值不一致时更新
//     if (logo_height_slider->value() != newHeight) {
//         logo_height_slider->blockSignals(true);   // 阻止递归触发
//         logo_height_slider->setValue(newHeight);
//         logo_height_slider->blockSignals(false);
//         logo_height_slider_val->setText(QString::number(newHeight));
//     }

     // 更新模型参数

     edit_temp_attr.logo_attr.width_factor = logo_width_slider->value();
     edit_temp_attr.logo_attr.height_factor = logo_height_slider->value();

     set_template_preview_logo(edit_temp_attr);
}

void TemplateDialog::slot_logo_height_slider_valueChanged(int value)
{
    // 高度 slider 改变
    logo_height_slider_val->setText(QString::number(value));

    // 计算宽度（保持比例）
    int newWidth = qRound(value * LOGO_ASPECT_RATIO);
    if (newWidth < logo_width_slider->minimum()) newWidth = logo_width_slider->minimum();
    if (newWidth > logo_width_slider->maximum()) newWidth = logo_width_slider->maximum();

//    if (logo_width_slider->value() != newWidth) {
//        logo_width_slider->blockSignals(true);
//        logo_width_slider->setValue(newWidth);
//        logo_width_slider->blockSignals(false);
//        logo_width_slider_val->setText(QString::number(newWidth));
//    }

    // 更新模型参数

    edit_temp_attr.logo_attr.width_factor = logo_width_slider->value();
    edit_temp_attr.logo_attr.height_factor = logo_height_slider->value();

    set_template_preview_logo(edit_temp_attr);
}


void TemplateDialog::slot_logo_show_changed()
{
   bool isChecked = logo_setting_show_checkbox->checkState();
   edit_temp_attr.logo_attr.is_display = isChecked;
   set_template_preview_logo(edit_temp_attr);
}


void TemplateDialog::load_template_names(const QString &Dir)
{
    qDebug() << "load_template_names =="  << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QStringList temp_names = ReadTemplateNames(Dir);
    qDebug() << "ReadTemplateNames ==>>>>>" << temp_names;

    template_selector->addItems(temp_names);
}

void TemplateDialog::load_template_param(const QString &Dir, const QString &tempName)
{
    TemplateAttr *temp_attr = new TemplateAttr();
    if(!ReadTemplateParam(Dir, tempName, temp_attr))
    {
        delete temp_attr;
        return;
    }

    if(!p_selection_temp_attr)
    {
       delete p_selection_temp_attr;
       p_selection_temp_attr = nullptr;
    }
    p_selection_temp_attr = temp_attr;

    //code_type_panel 条码类型面板
    CodeAttr *code_attr = &temp_attr->code_attr;

    qDebug() << "panel_attr[MAX_INPUT_PANEL - 1].pos_y =="  << p_selection_temp_attr->panel_attr[MAX_INPUT_PANEL - 1].pos_y;

    //条码类型选择器改用code_type_selector
    int index = -1;
    for (int i = 0; i < code_type_selector->count(); ++i)
    {
        int userData = code_type_selector->itemData(i, Qt::UserRole).toInt();
        if (userData == code_attr->type.toInt())
        {
            index = i;
            break;
        }
    }
    if (index != -1)
    {
        code_type_selector->blockSignals(true);
        code_type_selector->setCurrentIndex(index);
        code_type_selector->blockSignals(false);
    }
    else
    {
        // 如果匹配不到，设置到第一个 item
        if (code_type_selector->count() > 0)
        {
            code_type_selector->blockSignals(true);
            code_type_selector->setCurrentIndex(0);
            code_type_selector->blockSignals(false);
        }
    }

    //code_param_panel 条码参数面板
//    code_width_slider->setValue(code_attr->width_factor);
//    code_height_slider->setValue(code_attr->height_factor);
//    code_width_slider_val->setText(QString::number(code_attr->width_factor));
//    code_height_slider_val->setText(QString::number(code_attr->height_factor));
    qDebug() << "load_template_param.code_attr->width=======" << code_attr->width;
    code_width_slider->setValue(code_attr->width);
    code_height_slider->setValue(code_attr->height);

    code_fixed_size_checkbox->setChecked(code_attr->fixed_size);
    code_width_spinbox->setValue(code_attr->fixed_width);
    code_height_spinbox->setValue(code_attr->fixed_height);
    code_seperator_lineedit->setText(code_attr->separator);
    code_seperator_checkbox->setChecked(code_attr->separator_ischecked);

    //logo_param logo参数
    LogoAttr *logo_attr = &temp_attr->logo_attr;
    logo_width_slider->setValue(logo_attr->width_factor);
    logo_height_slider->setValue(logo_attr->height_factor);
    logo_width_slider_val->setText(QString::number(logo_attr->width_factor));
    logo_height_slider_val->setText(QString::number(logo_attr->height_factor));
    logo_setting_show_checkbox->setChecked(logo_attr->is_display);

    QStringList stringList = code_attr->text_index.split(';');
    QVector<int> vec;
    for (const QString& s : stringList)
    {
        bool ok;
        int num = s.trimmed().toInt(&ok);
        if (ok)
        {
             vec << num;
        }
        else
        {
            qWarning() << "Conversion error for:" << s;
        }
    }

    //content_panel 内容面板
    PanelAttr *panel_attr = temp_attr->panel_attr;
    for (int i=0; i<MAX_INPUT_PANEL; i++)
    {
        // 阻止状态选择组合框提前触发currentIndexChanged() 信号
        name_btn[i]->blockSignals(true);
        //input_box[i]->blockSignals(true);
        min_width[i]->blockSignals(true);
        sta_selector[i]->blockSignals(true);
        font_selector[i]->blockSignals(true);
        font_size[i]->blockSignals(true);

        name_btn[i]->setText(panel_attr[i].name);

        if(i==MAX_INPUT_PANEL-1)
        {
            time_format_selector->blockSignals(true);
            time_format_selector->setCurrentText(panel_attr[i].input_text);
            time_format_selector->blockSignals(false);
        }
        else
        {
            // 阻止输入框 TextChanged() 信号
            //input_box[i]->blockSignals(true);
            // 设置文本时不触发事件过滤器
            input_box[i]->setText(panel_attr[i].input_text);
            //input_box[i]->blockSignals(false);
        }

        min_width[i]->setValue(panel_attr[i].min_width);
        sta_selector[i]->setCurrentIndex(sta_selector[i]->findData(panel_attr[i].sta));
        font_selector[i]->setCurrentText(panel_attr[i].font);
        font_size[i]->setValue(panel_attr[i].font_size);

        //条码复选框，字段信息是否附加到条码里面
        barcode_checkbox[i]->setChecked(false);
        if(vec.indexOf(i)>=0)
        {
            barcode_checkbox[i]->setChecked(true);
        }

        name_btn[i]->blockSignals(false);
        //input_box[i]->blockSignals(false);
        min_width[i]->blockSignals(false);
        sta_selector[i]->blockSignals(false);
        font_selector[i]->blockSignals(false);
        font_size[i]->blockSignals(false);
    }
    translateDefaultTemp();
    

}

QStringList TemplateDialog::ReadTemplateNames(const QString &Dir)
{
    QDir temp_dir(Dir);
    QStringList temp_names;
    QStringList folder_list = temp_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (QString &fileName: folder_list )
    {
        //QFileInfo file_info(fileName);
        // 直接使用目录名，不需要通过QFileInfo处理
        //temp_names.append(file_info.completeBaseName());
        temp_names.append(fileName);
    }
    return temp_names;
}

bool TemplateDialog::ReadTemplateParam(const QString &Dir, const QString &tempName, TemplateAttr *tempAttr)
{
    if(tempName.isEmpty())
    {
        qDebug() << "Template name could not be Empty!" ;
        return false;
    }

    QString file_path = (QString("%1/%2/param.ini").arg(Dir, tempName));

    QFile file(file_path);
    if(!file.exists())
    {
        qDebug() << "Template file not exist:" << file_path;
        return false;
    }

    QSettings template_ini(file_path, QSettings::IniFormat);

    //模板名称
    tempAttr->name = tempName;

    //条码参数
    template_ini.beginGroup(QString("code_param"));
    tempAttr->code_attr.type = template_ini.value("type", "0").toString();
    tempAttr->code_attr.width = template_ini.value("width", 0).toInt();
    tempAttr->code_attr.height = template_ini.value("height", 0).toInt();
    tempAttr->code_attr.width_factor = template_ini.value("width_factor", 16).toInt();
    tempAttr->code_attr.height_factor = template_ini.value("height_factor", 16).toInt();
    tempAttr->code_attr.fixed_size = template_ini.value("fixed_size", false).toBool();
    tempAttr->code_attr.fixed_width = template_ini.value("fixed_width", 0).toInt();
    tempAttr->code_attr.fixed_height = template_ini.value("fixed_height", 0).toInt();
    tempAttr->code_attr.pos_x = template_ini.value("pos_x", 0).toInt();
    tempAttr->code_attr.pos_y = template_ini.value("pos_y", 0).toInt();
    tempAttr->code_attr.separator = template_ini.value("separator", "").toString();
    tempAttr->code_attr.separator_ischecked = template_ini.value("separator_ischecked", false).toBool();
    tempAttr->code_attr.text_index = template_ini.value("text_index", "0").toString();
    tempAttr->code_attr.direct_val = template_ini.value("direct_val", 0).toInt();
    template_ini.endGroup();

    //编辑内容
    for (int i = 0; i < MAX_INPUT_PANEL; i++)
    {
        template_ini.beginGroup(QString("panel_%1").arg(i));
        tempAttr->panel_attr[i].name = template_ini.value("name", "").toString(); //Name
        tempAttr->panel_attr[i].input_text = template_ini.value("input_text", "").toString(); //Input Text
        tempAttr->panel_attr[i].min_width = template_ini.value("min_width", -1).toInt();
        tempAttr->panel_attr[i].font = template_ini.value("font", "").toString();
        tempAttr->panel_attr[i].font_size = template_ini.value("font_size", 12).toInt();
        tempAttr->panel_attr[i].pos_x = template_ini.value("pos_x", 0).toInt();
        tempAttr->panel_attr[i].pos_y = template_ini.value("pos_y", 0).toInt();
        tempAttr->panel_attr[i].sta = template_ini.value("sta", "hidden").toString(); //editable

        template_ini.endGroup();
    }

    //logo参数
    template_ini.beginGroup(QString("logo_param"));
    tempAttr->logo_attr.width = template_ini.value("width", 16).toInt();
    tempAttr->logo_attr.height = template_ini.value("height", 16).toInt();
    tempAttr->logo_attr.width_factor = template_ini.value("width_factor", 1).toInt();
    tempAttr->logo_attr.height_factor = template_ini.value("height_factor", 1).toInt();
    tempAttr->logo_attr.fixed_size = template_ini.value("fixed_size", false).toBool();
    tempAttr->logo_attr.fixed_width = template_ini.value("fixed_width", 0).toInt();
    tempAttr->logo_attr.fixed_height = template_ini.value("fixed_height", 0).toInt();
    tempAttr->logo_attr.pos_x = template_ini.value("pos_x", 0).toInt();
    tempAttr->logo_attr.pos_y = template_ini.value("pos_y", 0).toInt();
    tempAttr->logo_attr.direct_val = template_ini.value("direct_val", 0).toInt();
    tempAttr->logo_attr.file_name = template_ini.value("file_name", "").toString();
    tempAttr->logo_attr.is_display = template_ini.value("is_display", false).toBool();

    template_ini.endGroup();

    return true;
}

void TemplateDialog::Exec()
{
   retranslateUi();
   exec();
}

void TemplateDialog::setMarkHandle(MarkHandle *handle)
{
    m_pMarkHandle = handle;
}

void TemplateDialog::installWheelBlocker(QWidget *root)
{
    const auto combos = root->findChildren<QComboBox*>();
    for (QComboBox* cb : combos)
    {
        cb->installEventFilter(this);
    }

    const auto spins = root->findChildren<QSpinBox*>();
    for (QSpinBox* sb : spins)
    {
        sb->installEventFilter(this);
    }
}

void TemplateDialog::changeEvent(QEvent *event)
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

bool TemplateDialog::eventFilter(QObject *obj,QEvent *ev)
{
    //禁用鼠标滚轮
    if (ev->type() == QEvent::Wheel)
    {
        if (qobject_cast<QComboBox*>(obj) || qobject_cast<QSpinBox*>(obj))
        {
            return true;
        }
    }
    // 处理焦点进入事件，支持触摸键盘
    if(ev->type() == QEvent::FocusIn)
    {
        // 对于QLineEdit，调用TouchKeyboardManager处理焦点进入
        if(QLineEdit *lineEdit = qobject_cast<QLineEdit *>(obj))
        {
            // 只有在启用触摸键盘功能时才执行
            if(m_enableTouchKeyboard)
            {
                TouchKeyboardManager::instance().handleFocusIn(lineEdit);
            }
            // 调用封装的高亮处理方法
            handleFocusHighlight(obj);
        }
        // 对于QSpinBox，也调用TouchKeyboardManager处理焦点进入
        else if(QSpinBox *spinBox = qobject_cast<QSpinBox *>(obj))
        {
            // 只有在启用触摸键盘功能时才执行
            if(m_enableTouchKeyboard)
            {
                TouchKeyboardManager::instance().handleFocusIn(spinBox);
            }
            // 调用封装的高亮处理方法
            handleFocusHighlight(obj);
        }
        // 对于QPushButton，处理焦点进入
        else if(QPushButton *btn = qobject_cast<QPushButton *>(obj))
        {
            // 调用封装的高亮处理方法
            handleFocusHighlight(obj);
        }
        // 对于QComboBox，处理焦点进入
        else if(QComboBox *comboBox = qobject_cast<QComboBox *>(obj))
        {
            // 调用封装的高亮处理方法
            handleFocusHighlight(obj);
        }
        
        // 无论是什么控件，获得焦点时都确保不被阻塞
        obj->setProperty("blocked", false);
        if(QWidget *widget = qobject_cast<QWidget *>(obj))
        {
            // 确保窗口属性正确设置
//            if(TouchKeyboardManager::isEnabled())
//            {
//                TouchKeyboardManager::allowAutoInvoke(widget);
//            }
            widget->update();
            widget->setEnabled(true); // 确保控件启用
        }
    }

    // 处理焦点离开事件，关闭触摸键盘
    else if(ev->type() == QEvent::FocusOut)
    {
//        TouchKeyboardManager::hide();
//        // 确保控件不被阻塞
//        obj->setProperty("blocked", false);
//        if(QWidget *widget = qobject_cast<QWidget *>(obj))
//        {
//            widget->update();
//            widget->setEnabled(true);
//        }

        if(QLineEdit *lineEdit = qobject_cast<QLineEdit *>(obj))
        {
            QApplication::setOverrideCursor(Qt::ArrowCursor);
        }
    }
    
    // 原有logo_setting_display的处理逻辑保持不变
    if((obj == logo_setting_display) && (ev -> type() == QEvent::MouseButtonPress))
    {
        return true;
    }
    else if((obj == logo_setting_display) && (ev -> type() == QEvent::MouseButtonRelease))
    {
        if(m_bEditEnable)
        {
            on_select_logofile_pressed();
        }
        return true;
    }
    
    return QDialog::eventFilter(obj, ev);
}


void TemplateDialog::reload_logo_file(const QString logofile)
{
//    QPixmap logo_pixmap(logofile);
//    QPixmap logo_scaled = logo_pixmap.scaled(QSize(210, 100), Qt::KeepAspectRatio, Qt::SmoothTransformation);
//    logo_setting_display->setPixmap(logo_scaled);
//    logo_setting_display->setAlignment(Qt::AlignCenter);
//    logo_setting_display->setScaledContents(false);
//    logo_setting_display->setObjectName(logofile);


    QPixmap logo_pixmap(logofile);
     if (logo_pixmap.isNull())
     {
         logo_setting_display->clear();
         logo_setting_display->setObjectName("");
         return;
     }

     // 1. 保持原始分辨率，但适配在 210x100 内显示
     QPixmap logo_scaled = logo_pixmap.scaled(
         QSize(210, 100),
         Qt::KeepAspectRatio,            // 保持宽高比
         Qt::SmoothTransformation        // 平滑缩放
     );

     // 2. 转换为二值图像（白底黑色前景）
     QImage bwImage = logo_scaled.toImage().convertToFormat(QImage::QImage::Format_Grayscale8);
     QPixmap bwPixmap = QPixmap::fromImage(bwImage);

     // 3. 设置到 QLabel
     logo_setting_display->setPixmap(bwPixmap);
     logo_setting_display->setFixedSize(210, 100);  // QLabel 固定大小 210x100
     logo_setting_display->setAlignment(Qt::AlignCenter); // 居中显示
     logo_setting_display->setScaledContents(false); // 不强制拉伸填充
     logo_setting_display->setObjectName(logofile);

}


bool TemplateDialog::saveLogoToTemplate(QString &logoFileName)
{
    qDebug() << "saveLogoToTemplate1.logoFileName=========>>>>>>>" << logoFileName;
    // 1. 检查QLabel是否有有效图片
//    QPixmap logoPixmap = *logo_setting_display->pixmap();
//    if (logoPixmap.isNull())
//    {
//        return false;
//    }

    const QPixmap *pixmapPtr = logo_setting_display->pixmap();
    if (!pixmapPtr || pixmapPtr->isNull())  // 先检查指针，再检查内容
    {
        return false;
    }

    QPixmap logoPixmap = *pixmapPtr;  // 安全解引用

    // 2. 构造目标路径
    QString editLogoPath = logo_setting_display->objectName();

    qDebug() << "saveLogoToTemplate1.构造目标路径.editLogoPath=========>>>>>>>" << editLogoPath;
    if (editLogoPath.isEmpty())
    {
        return false;
    }
    QFileInfo editFileInfo(editLogoPath);
    QString current_temp_name = template_selector->currentText();

    //current_temp_name为空,视为新建
//    if(current_temp_name.isEmpty())
//    {
//        current_temp_name = m_StrNewTempName;
//    }

    //考虑到新建和复制的情况
    if(!m_StrNewTempName.isEmpty())
    {
        current_temp_name = m_StrNewTempName;
    }

    QString logoFilePath = QString("%1/%2/%3")
            .arg(USER_TEMPLATES_DIR)
            .arg(current_temp_name)
            .arg(editFileInfo.fileName());

    qDebug() << "saveLogoToTemplate1.构造目标路径.logoFilePath=========>>>>>>>" << logoFilePath;

    // 3. 确保目标目录存在
    QDir targetDir(QString("%1/%2").arg(USER_TEMPLATES_DIR).arg(current_temp_name));
    if (!targetDir.exists())
    {
        targetDir.mkpath(".");
    }

    // 4. 保存QLabel中的图片到目标路径
    //保存前先删除可能存在的图片
    QFile targetFile(logoFilePath);
    if (targetFile.exists())
    {
        if (!targetFile.remove())
        {
            //tr("无法删除旧文件：%1").arg(targetFile.errorString()));
            return false;
        }
        QThread::msleep(100); // 延时100毫秒（确保文件系统释放资源）
    }

    //保存文件
    if (!logoPixmap.save(logoFilePath))
    {
        return false;
    }
    else
    {
        if(0 != logoFileName.compare(editFileInfo.fileName()))
        {
            //删除配置文件里旧有的logo文件
            QString logoFilePath = QString("%1/%2/%3")
                    .arg(USER_TEMPLATES_DIR)
                    .arg(current_temp_name)
                    .arg(logoFileName);
            QFile oldLogoFile(logoFilePath);

            if(oldLogoFile.exists())
            {
                oldLogoFile.remove();
            }
            qDebug() << "删除配置文件里旧有的logo文件" << logoFilePath;
        }
        //返回更换logo后新的文件名
        logoFileName = editFileInfo.fileName();
    }
    return true;
}

void TemplateDialog::clearNameHighLight()
{
    for (int i=0; i<MAX_INPUT_PANEL; i++)
    {
        name_btn[i]->setCheckable(false);
        name_btn[i]->setChecked(false);
        name_btn[i]->setCheckable(true);
    }
}

void TemplateDialog::handleFocusHighlight(QObject *obj)
{

    qDebug() << "handleFocusHighlight===========";
    // 获得焦点时，调用setHighLightText高亮对应的文本
    int textIndex = obj->objectName().toInt();
    if(m_bEditEnable && m_pTemplatePreview && textIndex >= 0 && textIndex < MAX_INPUT_PANEL)
    {
        if(textIndex < MAX_INPUT_PANEL - 1)
        {
            m_pTemplatePreview->setHighLightText(textIndex);
            m_pTemplatePreview->setHighLightTime(false);
            slot_HighLightTextIndex(textIndex);
        }
        else
        {
            m_pTemplatePreview->setHighLightTime(true);
            slot_HighLightTime();
        }
    }
}


void TemplateDialog::on_select_logofile_pressed()
{
    QString logo_from_file_path = QDir::homePath();
    QString current_temp_name = template_selector->currentText();

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile); // 只能选择单个文件
    dialog.setNameFilter(tr("所有文件 (*.*);;图片文件 (*.png *.jpg *.jpeg *.bmp)"));
    dialog.setDirectory(logo_from_file_path);

    if (dialog.exec())
    {
        QStringList files = dialog.selectedFiles();
        QString sourceFilePath = files.first(); // 获取第一个（唯一）文件

        reload_logo_file(sourceFilePath);

        m_StrNewlogofile = sourceFilePath;
        qDebug() << "on_select_logofile_pressed.sourceFilePath====>>>>>" << sourceFilePath;
        slot_logo_show_changed();
    }
}

void TemplateDialog::retranslateUi()
{
    MainSettings settings;
    SettingDialog::ReadSettings(&settings);
    code_param_pic->setPixmap(QPixmap(":/image/code_param_sample_en.png"));
    if(settings.language_setting.language_cbb.compare("简体中文") == 0)
    {
         code_param_pic->setPixmap(QPixmap(":/image/code_param_sample.png"));
    }
    else if(settings.language_setting.language_cbb.compare("English") == 0)
    {
        code_param_pic->setPixmap(QPixmap(":/image/code_param_sample_en.png"));
    }

    translateDefaultTemp();
}

void TemplateDialog::translateDefaultTemp()
{
    //翻译默认固定模板里的“病理号”、“小号”、“备注1”
   if(p_selection_temp_attr->panel_attr[0].name.compare("病理号") == 0)
   {
       name_btn[0]->setText(tr("病理号"));
   }
   if(p_selection_temp_attr->panel_attr[1].name.compare("小号") == 0)
   {
       name_btn[1]->setText(tr("小号"));
   }
   if(p_selection_temp_attr->panel_attr[2].name.compare("备注1") == 0)
   {
       name_btn[2]->setText(tr("备注1"));
   }
}

void TemplateDialog::write_template_param(const QString &Dir, const QString &tempName)
{
    //保存配置到文件
    QSettings template_ini(QString("%1/%2/param.ini").arg(Dir, tempName), QSettings::IniFormat);

    //获取条形码位置信息
    QPoint barCodePoint = m_pTemplatePreview->getBarCodePosition();
    //取分隔符
    QString separator ;
    bool isChecked;
    m_pTemplatePreview->getBarCodeTextSeparator(isChecked,separator);

    QString text_index = get_barCode_textIndex();

    //QVariant codetype = code_type_list->currentItem()->data(Qt::UserRole);
    QVariant codetype = code_type_selector->currentData(Qt::UserRole).toInt();
    //条形码为空的情况
    if(codetype.toInt() < 0)
    {
        separator = "";
        text_index = "";
        isChecked = false;
    }
    else
    {
        //条形码在非QR_Code和Data_Matrix情况下，条形码固定附加第一个标签
        if ((QBarcode::QR_Code != codetype) && (QBarcode::Data_Matrix != codetype))
        {
            text_index = "0";
        }
    }
    qDebug() << "write_template_param.get_barCode_textIndex=====" << text_index;

    edit_temp_attr.code_attr.direct_val = m_pTemplatePreview->getDirect();

    //条码参数
    template_ini.beginGroup(QString("code_param"));
    template_ini.setValue(QString("type"), codetype);
//    template_ini.setValue(QString("width"), edit_temp_attr.code_attr.width);
//    template_ini.setValue(QString("height"), edit_temp_attr.code_attr.height);
//    template_ini.setValue(QString("width_factor"), code_width_slider_val->text());
//    template_ini.setValue(QString("height_factor"), code_height_slider_val->text());

    template_ini.setValue(QString("width"), code_width_slider_val->text());
    template_ini.setValue(QString("height"), code_height_slider_val->text());
    template_ini.setValue(QString("width_factor"), edit_temp_attr.code_attr.width_factor);
    template_ini.setValue(QString("height_factor"), edit_temp_attr.code_attr.height_factor);

    template_ini.setValue(QString("fixed_size"), code_fixed_size_checkbox->isChecked());
    template_ini.setValue(QString("fixed_width"), code_width_spinbox->text());
    template_ini.setValue(QString("fixed_height"), code_height_spinbox->text());
    template_ini.setValue(QString("pos_x"), barCodePoint.x());
    template_ini.setValue(QString("pos_y"), barCodePoint.y());
    template_ini.setValue(QString("separator"), separator);
    template_ini.setValue(QString("separator_ischecked"),isChecked);
    template_ini.setValue(QString("text_index"), text_index);
    template_ini.setValue(QString("direct_val"), edit_temp_attr.code_attr.direct_val);
    template_ini.endGroup();

    QPoint timePoint = m_pTemplatePreview->getTimePosition();
    qDebug() << "write_template_param=>getTimePosition ===" << timePoint;
    //编辑内容
    for (int i=0; i<MAX_INPUT_PANEL; i++)
    {
        QPoint textPoint = m_pTemplatePreview->getTextPosition(i);
        template_ini.beginGroup(QString("panel_%1").arg(i));
        template_ini.setValue(QString("name"), name_btn[i]->text());
        template_ini.setValue(QString("input_text"), (i==MAX_INPUT_PANEL-1)? time_format_selector->currentText(): input_box[i]->text());
        template_ini.setValue(QString("min_width"), min_width[i]->text());
        template_ini.setValue(QString("font"), font_selector[i]->currentText());
        template_ini.setValue(QString("font_size"), font_size[i]->value());
        template_ini.setValue(QString("pos_x"), (i==MAX_INPUT_PANEL-1)? timePoint.x():textPoint.x());//待修改数据来源
        template_ini.setValue(QString("pos_y"), (i==MAX_INPUT_PANEL-1)? timePoint.y():textPoint.y());//待修改数据来源
        template_ini.setValue(QString("sta"), sta_selector[i]->currentData());
        template_ini.endGroup();
    }

    QString logofileName = edit_temp_attr.logo_attr.file_name;
    qDebug() << "write_template_param.logofileName===========>" << logofileName ;

    if(saveLogoToTemplate(logofileName))
    {
       edit_temp_attr.logo_attr.file_name = logofileName;
    }

    qDebug() << "write_template_param.edit_temp_attr.logo_attr.file_name===========>" << edit_temp_attr.logo_attr.file_name ;
    //获取logo位置信息
    QPoint logoPoint = m_pTemplatePreview->getLogoPosition();
    //logo参数
    template_ini.beginGroup(QString("logo_param"));
    template_ini.setValue(QString("width"), edit_temp_attr.logo_attr.width);
    template_ini.setValue(QString("height"), edit_temp_attr.logo_attr.height);
    template_ini.setValue(QString("width_factor"), edit_temp_attr.logo_attr.width_factor);
    template_ini.setValue(QString("height_factor"), edit_temp_attr.logo_attr.height_factor);
    template_ini.setValue(QString("fixed_size"), edit_temp_attr.logo_attr.fixed_size);
    template_ini.setValue(QString("fixed_width"), edit_temp_attr.logo_attr.fixed_width);
    template_ini.setValue(QString("fixed_height"), edit_temp_attr.logo_attr.fixed_height);
    template_ini.setValue(QString("pos_x"), logoPoint.x());
    template_ini.setValue(QString("pos_y"), logoPoint.y());
    template_ini.setValue(QString("direct_val"), edit_temp_attr.logo_attr.direct_val);
    template_ini.setValue(QString("file_name"), edit_temp_attr.logo_attr.file_name);
    template_ini.setValue(QString("is_display"), edit_temp_attr.logo_attr.is_display);
    template_ini.endGroup();

}
