#ifndef __WORK_THREAD_H__
#define __WORK_THREAD_H__

#include <pthread.h>
#include <sys/queue.h>




#define WORK_THREAD_NUM     4



typedef struct work_task work_task_t;

struct work_task{
    void (*handler) (work_task_t *);            //任务的处理接口，在派发任务时初始化
    //其他需要的参数
    char *task_data;
    int task_data_len;
    int thread_idx;
};

void work_thread_dump(void);
void work_thread_init(void);
void work_thread_dispatch_task(int thread_idx, work_task_t *work_task);
void work_thread_dispatch_task_auto(work_task_t *work_task);
void work_thread_dispatch_task_random(work_task_t *work_task);
void work_thread_join(void);

#endif
