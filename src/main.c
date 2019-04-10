#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "work_thread.h"


#define handle_error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)




//屏蔽特定的警告
//方法1
//-Wno-pointer-to-int-cast -Wno-int-to-pointer-cast
//方法2
/*
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#endif
.....
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
*/


void work_task_handler_test(work_task_t *work_task){
    //printf("thread idx = %d\n", thread_idx);
    //pthread_t tid = pthread_self();
    //int idx = 0;
    //int i;
    /*for(i = 0; i < WORK_THREAD_NUM; i++){
        if(tid == work_thread[i].tid){
            printf("idx = %d, task_data_len = %d, work_task_cnt=%d\n", i, work_task->task_data_len, work_thread[i].work_task_cnt);
            break;
        }
    }*/
    /*printf("thread_idx = %d, work_task->task_data_len = %d work_task_cnt=%d\n", 
        work_task->thread_idx, work_task->task_data_len, work_thread[work_task->thread_idx].work_task_cnt);*/
    //printf("tid = %ld\n", pthread_self());
#if 0
    struct timespec t;
    t.tv_sec = 0;
    t.tv_nsec = 100000000;
    //printf("sizeof(t) = %ld\n", sizeof(t));
    nanosleep(&t, NULL);
#endif
    //sleep(1);
    usleep(30);
}

static void *dump_func(void *arg){
    while(1){
        work_thread_dump();
        usleep(50000);
    }
    return NULL;
}

int main(int argc, char* argv[]){
    int i = 0;
    int thread_idx = 0;
 /*   
       #ifdef _POSIX_SOURCE
           printf("_POSIX_SOURCE defined\n");
       #endif

       #ifdef _POSIX_C_SOURCE
           printf("_POSIX_C_SOURCE defined: %ldL\n", (long) _POSIX_C_SOURCE);
       #endif

       #ifdef _ISOC99_SOURCE
           printf("_ISOC99_SOURCE defined\n");
       #endif

       #ifdef _ISOC11_SOURCE
           printf("_ISOC11_SOURCE defined\n");
       #endif

       #ifdef _XOPEN_SOURCE
           printf("_XOPEN_SOURCE defined: %d\n", _XOPEN_SOURCE);
       #endif

       #ifdef _XOPEN_SOURCE_EXTENDED
           printf("_XOPEN_SOURCE_EXTENDED defined\n");
       #endif

       #ifdef _LARGEFILE64_SOURCE
           printf("_LARGEFILE64_SOURCE defined\n");
       #endif

       #ifdef _FILE_OFFSET_BITS
           printf("_FILE_OFFSET_BITS defined: %d\n", _FILE_OFFSET_BITS);
       #endif

       #ifdef _BSD_SOURCE
           printf("_BSD_SOURCE defined\n");
       #endif

       #ifdef _SVID_SOURCE
           printf("_SVID_SOURCE defined\n");
       #endif

       #ifdef _DEFAULT_SOURCE
           printf("_DEFAULT_SOURCE defined\n");
       #endif

       #ifdef _ATFILE_SOURCE
           printf("_ATFILE_SOURCE defined\n");
       #endif
        
       #ifdef _GNU_SOURCE
           printf("_GNU_SOURCE defined\n");
       #endif

       #ifdef _REENTRANT
           printf("_REENTRANT defined\n");
       #endif

       #ifdef _THREAD_SAFE
           printf("_THREAD_SAFE defined\n");
       #endif

       #ifdef _FORTIFY_SOURCE
           printf("_FORTIFY_SOURCE defined\n");
       #endif
*/

    work_thread_init();

    pthread_t dump_tid;
    pthread_create(&dump_tid, NULL, dump_func, NULL);

    /*printf("直接分发\n");
    for(i = 0; i < 10; i++){
        thread_idx = i % WORK_THREAD_NUM;
        work_task_t work_task;
        memset(&work_task, 0, sizeof(work_task));
        work_task.handler = work_task_handler_test;
        work_task.task_data_len = i;

        work_thread_dispatch_task(thread_idx, &work_task);   
    }*/
    sleep(2);
    printf("自动分发\n");
    for( i = 0; i < 100000; i++){
        work_task_t work_task;
        memset(&work_task, 0, sizeof(work_task));
        work_task.handler = work_task_handler_test;
        work_task.task_data_len = i;
        work_thread_dispatch_task_auto(&work_task);   
        //usleep(500);
    }
    /*sleep(2);
    printf("随机分发\n");
    for( i = 0; i < 10; i++){
        work_task_t work_task;
        memset(&work_task, 0, sizeof(work_task));
        work_task.handler = work_task_handler_test;
        work_task.task_data_len = i;
        work_thread_dispatch_task_random(&work_task);   
    }*/

    work_thread_join();
    pthread_join(dump_tid, NULL);

    return 0;
}
