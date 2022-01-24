/*
 * File      : main.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2018, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-08-16     armink       first implementation
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <dfs_posix.h>
#include <stdlib.h> //for atoi, the function that change char to int.
#include <finsh.h>
#include "functions.h"
#include <drivers/pm.h>
// #ifndef _MNT_H_
// #define _MNT_H_
//#include "./mnt_init/mnt.c"
// #endif
// the number of subtasks in each task set.
//#ifndef _TICK_TEST_C_
//#define _TICK_TEST_C_
//#include "./tick_test/tick_test.c"
//#endif
// #ifndef _WR_TEST_C_
// #define _WR_TEST_C_
//#include "./wr_test/wr_test.c"
// #endif

ALIGN(RT_ALIGN_SIZE)
static  char    sub_stack[TASK_NUMBER][SUBTASK_NUMBER][SUBTASK_STACK_SIZE]; //所有子线程的栈空间
//static  char    DAG_stack[TASK_NUMBER][TASK_STACK_SIZE];                    //所有DAG任务的栈空间
static  struct  subtask sub_t[TASK_NUMBER][SUBTASK_NUMBER];                 //子任务的结构体数组
static  struct  DAG_task t[TASK_NUMBER];                                    //DAG任务的结构体数组
static  struct  rt_event event[TASK_NUMBER];                                //事件集结构体
//static  struct  rt_thread DAG_thread[TASK_NUMBER];                          //DAG任务的线程控制块
static  struct  rt_thread sub_thread[TASK_NUMBER][SUBTASK_NUMBER];          //子任务的线程控制块
static  struct  rt_event exe_over;                                          //运行结果事件集
static  rt_list_t list_head;
static  struct  rt_thread self_timer;                                       //自己定义的定时器线程控制块
static  char    self_timer_stack[SELF_TIMER_STACK_SIZE];                   //自己定义的定时器线程栈空间

int main(void)
{


    int i;
    int total_WCRT;                 //save the total WCRT of valid task sets
    int valid_task_sets_number;     //the valid task sets number in 1000 task sets of each utilization
    rt_uint32_t e;
    rt_tick_t start;
    rt_err_t err;

    //用于文件读写
    // char file_path_v[64] = "/home/result/valuable_set/0.";
    // char file_name_v[64];
    // char file_path_r[64] = "/home/result/response_time/0.";
    // char file_name_r[64];
    // char data[16];
    // char *fp;

    // int fd, tt, ts, t_count;
    
    
    /*挂载文件系统*/
    mnt_init(); //文件系统挂在初始化


    // rt_kprintf("begin %d delay 10s!\n", rt_tick_get());
    // rt_thread_delay(10000);
    // rt_kprintf("end %d\n", rt_tick_get());


    // rt_kprintf("\n\nbegin %d delay 10s!\n",rt_tick_get());
    // rt_thread_mdelay(10000);
    // rt_kprintf("end %d\n",rt_tick_get());


    // ulog_example();

    // return 0;
/************/
/*************/
    // init execution over flag
    rt_event_init(&exe_over,"exe_flag",RT_IPC_FLAG_PRIO);

    rt_thread_delay(2000);


    for (int u = 24; u <= 24; u = u + 2)
//    int u = 2; //u means the utilization start from. x is 0.2 because it does not support the double 
    {
        rt_kprintf("start utilization 0.%d\n",u);
        total_WCRT = 0;
        valid_task_sets_number = 0;
        int now = 61;
        for (int task_set = now; task_set <= SET_NUMBER; task_set++)
        {
            /*initialise the self timer list head*/
            list_head.prev = &list_head;
            list_head.next = &list_head;

            for(int i = 0; i < TASK_NUMBER; i++)
            {
                t[i].u = u;
                t[i].task_set = task_set;
                t[i].Priority = i + 7;         //the smaller number the higher priority and base priority is 20
                t[i].DAG_order = i;
                t[i].event_base = (int)&event[0];
                t[i].sub_thread_base = (int)&sub_thread[i];
                t[i].sub_task_base = (int)&sub_t[i][0];
                t[i].over_flag = (int)&exe_over;
                /*initial DAG release list*/
                t[i].list.prev = &t[i].list;
                t[i].list.next = &t[i].list;
                /*assign self timer list*/
                t[i].list_head_base = &list_head;
                t[i].self_timer_base = &self_timer;
                for (int j = 0; j < SUBTASK_NUMBER; j++)
                {
                    //t[i].subts[j].sub_stack_base = (int)&sub_stack[i][j][0]; //保存每个子任务栈的基地址
                    ((subtask_p)(t[i].sub_task_base) + j)->sub_stack_base = (int)&sub_stack[i][j][0]; //保存每个子任务栈的基地址
                }
            }

            // 初始化任务集
            task_set_init(t);

            /*initial self timer threads*/
            // for (int i = 0; i < TASK_NUMBER; i++)
            // {
            //     rt_kprintf("%s: T: %d\n",t[i].task_name, t[i].T);
            //     for (int j = 0; j < SUBTASK_NUMBER; j++)
            //     {
            //         rt_kprintf("-%d %d-",j,sub_t[i][j].C);
            //     }
            //     rt_kprintf("\n");
            //     for (int j = 0; j < SUBTASK_NUMBER; j++)
            //     {
            //         rt_kprintf("-%d-",sub_t[i][j].processor);
            //     }
            //     rt_kprintf("\n");
            //     for (int j = 0; j < SUBTASK_NUMBER; j++)
            //     {
            //         rt_kprintf("-%d-",sub_t[i][j].constraint);
            //     }
            //     rt_kprintf("\n");
            // }

            // return 1;


            rt_thread_init(&self_timer,"self_timer",self_timer_scheduler,(void*)t,&self_timer_stack,SELF_TIMER_STACK_SIZE,0,30);
            rt_thread_control(&self_timer, RT_THREAD_CTRL_BIND_CPU, (void *)0);
            // rt_kprintf("self_timer initialise OK!\n");

            rt_kprintf("\n u = 0.%d, %d begin at %d.\n",u,task_set,rt_tick_get());
            exe_over.set = 0;

            start = rt_tick_get();
            for (i = 0; i < TASK_NUMBER; i++)
            {   
                t[i].first_release_tick = start;
                t[i].current_release_tick = start;
                t[i].next_release_tick = start + (rt_tick_t)(t[i].T);
            }
            /*first_release*/
            subtasks_first_release(t);


            // rt_kprintf("\n");
            /***release the DAG tasks***/
            rt_thread_startup(&self_timer);

            /*************************************************************/
            /*********************waiting for timeout*********************/
            /*************************************************************/
            err = rt_event_recv(&exe_over, (1 << 1) | (1 << 2) | (1 << 3), RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, EXECUTION_LIMITATION, &e);
            // 1表示调度失败，不可调度；2表示系统时钟更新错误，突增导致的调度失败，3表示调度成功
            // rt_kprintf("\ne: %d, err: %d, schedulable: %d at %d\n",e,err,t[0].schedulable,rt_tick_get());


            /*detach self_timer*/
            // rt_thread_suspend(&self_timer);
            rt_thread_detach(&self_timer);

            /*取消消息队列限制，清空子任务等待队列*/
            // for(i = 0; i < TASK_NUMBER; i++)
            // {
            //     rt_event_send(&event[i],1023);
            // }

            // rt_thread_delay(400);

            /*detach 所有子任务的线程*/
            

            for(i = 0; i < TASK_NUMBER; i++)
            {    
                for(int j = 0; j < SUBTASK_NUMBER; j++)
                {
                    // if (rt_thread_find(((subtask_p)(t[i].sub_task_base) + j)->sub_name) != RT_NULL)
                    // {
                        rt_thread_detach(&sub_thread[i][j]);  
                    // }
                    
                }
            }

            // rt_thread_delay(100);
            if ((err == -RT_ETIMEOUT && t[0].schedulable == 1) || e == (1 << 3))
            {//this case means that the task set has been executed sucessfully

                /*stop timers and send 1023 to events*/
                for (i = 0; i < TASK_NUMBER; i++)
                {
                    // rt_kprintf("%d-",t[i].WCRT);
                    total_WCRT += t[i].WCRT; 
                }
                // rt_kprintf("\n");
                // for (i = 0; i < TASK_NUMBER; i++)
                // {
                //     rt_kprintf("%d-",t[i].D);
                // }

                valid_task_sets_number ++;
                rt_kprintf("Current total WCRT is %d, valid set is %d/%d\n", total_WCRT, valid_task_sets_number,task_set-now+1);

            }

            if (e == (1 << 1))
            {//this case means that the task set has been executed false
                for (i = 0; i < TASK_NUMBER; i++)
                {
                    rt_kprintf("%d-",t[i].WCRT);
                }
                rt_kprintf("\n");
                for (i = 0; i < TASK_NUMBER; i++)
                {
                    rt_kprintf("%d-",t[i].D);
                }

                rt_kprintf("\nfailed by set\nCurrent total WCRT is %d, valid set is %d/%d\n", total_WCRT, valid_task_sets_number,task_set-now+1);

            }

            if (e == (1 << 2))
            {//it means that this task set can not be scheduled

                for (i = 0; i < TASK_NUMBER; i++)
                {
                    rt_kprintf("%d-",t[i].WCRT);
                }
                rt_kprintf("\n");
                for (i = 0; i < TASK_NUMBER; i++)
                {
                    rt_kprintf("%d-",t[i].D);
                }
                rt_kprintf("\nfailed by system\nCurrent total WCRT is %d, valid set is %d/%d\n", total_WCRT, valid_task_sets_number,task_set-now+1);
         
            }

            rt_thread_delay(2000);
            //detach timers and events!
            for (i = 0; i < TASK_NUMBER; i++)
            {
                rt_event_detach(&event[i]);
                // rt_kprintf("detach %d ok!\n",i);
            }

            rt_thread_delay(100);

        }
        //*************save data to the file
        ///////////可调度数量
        // fp = file_name_v;
        // rt_strncpy(fp,file_path_v,rt_strlen(file_path_v));
        // fp += rt_strlen(file_path_v);

        // rt_sprintf(fp,"%d",u);
        // fp ++;

        // rt_strncpy(fp,".txt",rt_strlen(".txt"));
        // fp += rt_strlen(".txt");

        // *fp = '\0';

        // fp = data;
        // t_count = 0;

        // ts = valid_task_sets_number; //需要被转化的整型
        // while(ts / 10 > 0)
        // {
        //     ts = ts / 10;
        //     t_count ++;
        // }
        // // sn_p = data; //保存转化后的字符串
        // ts = valid_task_sets_number;
        // while(t_count > 0) //比0大，表示还没到个位
        // {
        //     tt = power(10,t_count);
        //     tt = ts / tt;
        //     rt_sprintf(fp,"%d", tt);
        //     fp++;
        //     ts = ts - tt * power(10, t_count);
        //     t_count--;
        // }
        // rt_sprintf(fp,"%d", ts);
        // fp++;
        // *fp = '\0';


        // fd = open(file_name_v, O_RDWR | O_CREAT | O_TRUNC);

        // if (fd >= 0)
        // {

        //     write(fd, data, rt_strlen(data));
        //     close(fd);
        //     rt_kprintf("Write done!\n");
        // }

        // //////////响应时间
        // fp = file_name_r;
        // rt_strncpy(fp,file_path_r,rt_strlen(file_path_r));
        // fp += rt_strlen(file_path_r);

        // rt_sprintf(fp,"%d",u);
        // fp ++;

        // rt_strncpy(fp,".txt",rt_strlen(".txt"));
        // fp += rt_strlen(".txt");

        // *fp = '\0';

        // fp = data;
        // t_count = 0;

        // ts = total_WCRT; //需要被转化的整型
        // while(ts / 10 > 0)
        // {
        //     ts = ts / 10;
        //     t_count ++;
        // }
        // // sn_p = data; //保存转化后的字符串
        // ts = total_WCRT;
        // while(t_count > 0) //比0大，表示还没到个位
        // {
        //     tt = power(10,t_count);
        //     tt = ts / tt;
        //     rt_sprintf(fp,"%d", tt);
        //     fp++;
        //     ts = ts - tt * power(10, t_count);
        //     t_count--;
        // }
        // rt_sprintf(fp,"%d", ts);
        // fp++;
        // *fp = '\0';


        // fd = open(file_name_r, O_RDWR | O_CREAT | O_TRUNC);

        // if (fd >= 0)
        // {

        //     write(fd, data, rt_strlen(data));
        //     close(fd);
        //     rt_kprintf("Write done!\n");
        // }
        //*************save data to the file

        rt_kprintf("\n\n*************************************\n");
        rt_kprintf("The total WCRT when u equals to %d is: %d\n", u, total_WCRT);
        rt_kprintf("The corresponding valid task sets number is: %d\n", valid_task_sets_number);
        rt_kprintf("\n*************************************\n\n");

        


        //还是要往文件里面写
        

    }


    return 0;
}

