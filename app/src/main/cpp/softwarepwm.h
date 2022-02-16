//
// Created by johnd on 13-10-2021.
//
#ifndef PWMSERVO_SOFTWAREPWM_H
#define PWMSERVO_SOFTWAREPWM_H
#include <pthread.h>
#include <cstdio>
#include"gpio.h"
pthread_t setservotask;
pthread_mutex_t servotasklock;
int servopin = 41;
int ontime = 900000;
int period = 20000000;
int threadrunning=0;
void* softwarePWM_pulsegenerating_thread(__unused void*arg)
{
    threadrunning=1;
    pthread_t thId = pthread_self();
    pthread_attr_t thAttr;
    int policy = 0;
    pthread_attr_init(&thAttr);
    pthread_attr_getschedpolicy(&thAttr, &policy);
    int max_prio_for_policy = sched_get_priority_max(policy);
    pthread_setschedprio(thId, max_prio_for_policy);
    pthread_attr_destroy(&thAttr);
    int remainder = period - ontime;
    pthread_mutex_init(&servotasklock,NULL);
    pthread_mutex_lock(&servotasklock);
    timespec starttime,now;
    long double timetaken;
    clock_gettime(CLOCK_REALTIME,&starttime);
    gpio_set_val(1);
    do {
        clock_gettime(CLOCK_REALTIME,&now);
        timetaken = (((now.tv_sec*1e-9)+now.tv_nsec)-((starttime.tv_sec*1e-9)+starttime.tv_nsec));
    }while(timetaken<ontime&&timetaken>0);
    clock_gettime(CLOCK_REALTIME,&starttime);
    gpio_set_val(0);
    do {
        clock_gettime(CLOCK_REALTIME,&now);
        timetaken = (((now.tv_sec*1e-9)+now.tv_nsec)-((starttime.tv_sec*1e-9)+starttime.tv_nsec));
    }while(timetaken<remainder&&timetaken>0);
    pthread_mutex_unlock(&servotasklock);
    pthread_mutex_destroy(&servotasklock);
    threadrunning=0;
    return nullptr;
}
void softwarePWM_run(int ntime)
{
    ontime = ntime;
    char buf[50];
    sprintf(buf,"out");
    gpio_set_dir(buf,servopin);
    gpio_ope_val(servopin);
    gpio_set_val(0);
    for(int i=0;i<25;i++)
    {
        pthread_create(&setservotask,NULL,&softwarePWM_pulsegenerating_thread,NULL);
        pthread_join(setservotask,NULL);
        while(threadrunning);
    }
    gpio_clo_val();
}
#endif //PWMSERVO_SOFTWAREPWM_H
