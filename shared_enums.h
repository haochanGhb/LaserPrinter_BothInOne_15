#ifndef SHARED_ENUMS_H
#define SHARED_ENUMS_H

#include "public_fun.h"

enum TaskTextPos{
    TASK_TEXT,
    TASK_TEMP = MAX_INPUT_PANEL,
    TASK_SLOTS,
    TASK_STA,
    TASK_TYPE,
    TASK_TIME,
    TASK_NAME,
    TASK_MAX_ITEM
};

enum TaskType{
    LOCAL_TASK,
    REMOTE_TEXT_TASK,
    REMOTE_IMAGE_TASK
};

enum class TaskRecoverStatus {
    NotExistOrFinished,   // 任务不存在或者已经完成
    WaitingNotFirst,      // 任务在等待非队首
    WaitingAndIsFirst     // 任务在等待是队首
};

enum PrintProcessSta{
    READY,
    STARTMARK,
    CHANGESLOT,  //更换槽盒
    FINISHED,
    ENDED,
    EXCEPTION
};

#endif // SHARED_ENUMS_H