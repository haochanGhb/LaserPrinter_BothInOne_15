#ifndef PUBLIC_FUN_H
#define PUBLIC_FUN_H

#include <QList>
#include <QString>
#include <QDebug>
#include <QRegularExpression>

#define MAX_INPUT_PANEL 16
#define MAX_INPUT_PANEL_SHOW 7
#define MAX_SLOTS 6
#define MAX_SLOT_MENU_ACTIONS 8

#define MAX_FIXED_ITEM 20
#define MAX_EDIT_LIST MAX_FIXED_ITEM + 30
#define MAX_EDIT_LIST_HEIGHT 600

#define BUTTONS_PER_GROUP 6
#define MAX_TASKS 5000

#define SPLIT_SYMBOL            "../.."
#define HOPPER_NAME_AND_NUM_SPLIT_SYMBOL "\t"
#define SUB_SPLIT_SYMBOL        QChar(0x1f)

#define SETTINGS_INI            "./settings.ini"
#define PRESET_INPUT_INI        "./preset_input.ini"
#define DEFAULT_TEMPLATES_DIR   "./templates/default"
#define USER_TEMPLATES_DIR      "./templates/user"

#define EDIT_LIST_INI           "./edit_list.ini"
#define TASK_LIST_INI           "./task_list.ini"
#define PRINT_LIST_INI          "./print_list.ini"

#define LAST_STATUS_INI         "./last_status.ini"
#define ABOUT_INFO_INI          "./about_info.ini"

#define MARK_AREA_INI               "./BslModule.ini"
#define COLOR_MARK_LEVEL_INI        "./color_mark_level.ini"
#define MARK_LEVEL_ARGS_INI         "./mark_level_args.ini"
#define MARK_IMAGE_LEVEL_ARGS_INI   "./mark_image_level_args.ini"

#define STATISTICS_MAX_COUNT_DAY      10000

#define PRINTER_DEVICE_DISCOVER_PORT 50000
#define SCANNER_RECEIVER_DISCOVER_PORT 50001

#ifdef USE_CASSETTE
#define MAX_PREVIEW_TEXT_HEIGHT      41
#elif defined(USE_SLIDE) || defined(USE_SLIDE_DOUBLE)
#define MAX_PREVIEW_TEXT_HEIGHT      261
#endif

enum PrintSta{
    PRINT_STA_WAIT = -10000,
    PRINT_STA_FROZEN,
    PRINT_STA_PRINTING
};

Q_DECLARE_METATYPE(PrintSta)

enum ColNum{
    COL_INDEX,
    COL_NAME,
    COL_STA,
    COL_INFO,
    COL_CONTENT,
    COL_DATETIME
};


QString incTailNumber(const QString &text);

bool    isAlpha(const QString &text);
QString incTailAlpha(const QString &text);
QString decTailAlpha(const QString &text);
int     lettersCompare(const QString letters1, const QString letters2);

QStringList generateNumberTailSequence(const QString text1, const QString text2);
QStringList generateAlphaTailSequence(const QString startStr, const QString endStr);

int  calculateTotalCombinations(const QList<QStringList> &lists);
void GenerateCombinations(const QList<QStringList> &lists, QStringList &result, QString currentCombination = "", int depth = 0);
QString decTailNumber(const QString &text);
QString decTailAlpha(const QString &text);

qulonglong countNumberTailSequence(const QString text1, const QString text2);
qulonglong countAlphaTailSequence(const QString text1, const QString text2);

#endif // PUBLIC_FUN_H
