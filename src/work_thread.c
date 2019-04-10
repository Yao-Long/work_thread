#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "work_thread.h"


/******************************************************************************/

struct _work_task{
    work_task_t entry;                          //任务的具体数据
    STAILQ_ENTRY(_work_task) tasks;             //单指针队列
};

typedef struct{
    pthread_t tid;                              //当前线程id
    pthread_mutex_t mutex;                      //当前线程私有数据的互斥量
    pthread_cond_t cond;                        //当前线程私有数据的条件变量
    STAILQ_HEAD(stailhead, _work_task) head;    //任务队列的头
    int work_task_cnt;                          //当前线程的任务个数
}work_thread_t;

/******************************************************************************/
static work_thread_t work_thread[WORK_THREAD_NUM];  //工作线程


/******************************************************************************/
static void *work_thread_routine(void *arg){
    int thread_idx = (int)arg;

    while(1){
        pthread_mutex_lock(&work_thread[thread_idx].mutex);
        struct _work_task *_work_task = STAILQ_FIRST(&work_thread[thread_idx].head);
        //等待的条件为任务队列不为空        
        while(_work_task == NULL){
            pthread_cond_wait(&work_thread[thread_idx].cond, &work_thread[thread_idx].mutex);
            _work_task = STAILQ_FIRST(&work_thread[thread_idx].head);
        }
        //把任务从队列中删除
        STAILQ_REMOVE_HEAD(&work_thread[thread_idx].head, tasks);
        work_thread[thread_idx].work_task_cnt--;
        pthread_mutex_unlock(&work_thread[thread_idx].mutex);
        if(_work_task->entry.handler){
            _work_task->entry.handler(&_work_task->entry);
            //处理完成之后，释放任务空间
            free(_work_task);
        }
    }
    return NULL;
}

/******************************************************************************/

void work_thread_dump(void){
    int i;

    for(i = 0; i < WORK_THREAD_NUM; i++){
        pthread_mutex_lock(&work_thread[i].mutex);
        printf("thread_idx = %d, work_task_cnt=%d\n", i,  work_thread[i].work_task_cnt);
        pthread_mutex_unlock(&work_thread[i].mutex);
    }
}

void work_thread_init(void){
    int i = 0;
    int ret = 0;

    for(i = 0; i < WORK_THREAD_NUM; i++){
        ret = pthread_create(&work_thread[i].tid, NULL, work_thread_routine, (void *)i);
        if(ret != 0){
            perror("pthread_create failed!\n");
            exit(EXIT_FAILURE);
        }
        //初始化队列
        STAILQ_INIT(&work_thread[i].head);
        //初始化保护队列的互斥量和条件变量
        pthread_mutex_init(&work_thread[i].mutex, NULL);
        pthread_cond_init(&work_thread[i].cond, NULL);
        work_thread[i].work_task_cnt = 0;
    }
    //初始化随机数种子
    srandom(time(NULL));
}

//分配任务给指定的线程
void work_thread_dispatch_task(int thread_idx, work_task_t *work_task){
    struct _work_task *_work_task;
    _work_task = (struct _work_task *)malloc(sizeof(struct _work_task));
    if(_work_task == NULL){
        perror("malloc error!\n");
        return;
    }
    work_task->thread_idx = thread_idx;
    _work_task->entry = *work_task;
    pthread_mutex_lock(&work_thread[thread_idx].mutex);
    //向队列尾部添加任务
    STAILQ_INSERT_TAIL(&work_thread[thread_idx].head, _work_task, tasks);
    work_thread[thread_idx].work_task_cnt++;
    //printf("thread_idx = %d, work_task_cnt=%d\n", thread_idx,  work_thread[thread_idx].work_task_cnt);
    pthread_cond_broadcast(&work_thread[thread_idx].cond);
    pthread_mutex_unlock(&work_thread[thread_idx].mutex);
}

//自动分配任务给工作线程，分配给当前任务数最少的那个
void work_thread_dispatch_task_auto(work_task_t *work_task){
    int i;    
    int thread_idx = 0;
    int min_cnt;

    pthread_mutex_lock(&work_thread[0].mutex);
    min_cnt = work_thread[0].work_task_cnt;
    pthread_mutex_unlock(&work_thread[0].mutex);
    for(i = 1; i < WORK_THREAD_NUM; i++){
        pthread_mutex_lock(&work_thread[i].mutex);
        if(work_thread[i].work_task_cnt < min_cnt){
            min_cnt = work_thread[i].work_task_cnt;
            thread_idx = i;
        }
        pthread_mutex_unlock(&work_thread[i].mutex);
    }
    work_thread_dispatch_task(thread_idx, work_task); 
}

//随机分配任务给工作线程
void work_thread_dispatch_task_random(work_task_t *work_task){
    work_thread_dispatch_task(random() % WORK_THREAD_NUM, work_task);
}

//等待工作线程结束
void work_thread_join(void){
    for(int i = 0; i < WORK_THREAD_NUM; i++){
        pthread_join(work_thread[i].tid, NULL);
    }
}
