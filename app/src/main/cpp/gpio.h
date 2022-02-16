//
// Created by johnd on 13-10-2021.
//

#ifndef PWMSERVO_GPIO_H
#define PWMSERVO_GPIO_H

#include <fcntl.h>
#include <cstring>

__unused int gpio_export(int pin)
{
    int ret;
    char buf[50];
    int fd = open("/sys/class/gpio/export",O_WRONLY|O_SYNC);
    if(fd<0)
    {
        ret = -1;
        return ret;
    }
    sprintf(buf,"%d",pin);
    ret = write(fd,buf,strlen(buf));
    close(fd);
    return ret;
}

__unused int gpio_unexport(int pin)
{
    int ret;
    char buf[50];
    int fd = open("/sys/class/gpio/unexport",O_WRONLY|O_SYNC);
    if(fd<0)
    {
        ret = -1;
        return ret;
    }
    sprintf(buf, "%d", pin);
    ret = write(fd, buf, strlen(buf));
    close(fd);
    return ret;
}
int gpio_set_dir(char*dir,int pin)
{
    int ret;
    char buf[50];
    sprintf(buf,"/sys/class/gpio/gpio%d/direction",pin);
    int fd = open(buf,O_WRONLY|O_SYNC);
    if(fd < 0)
    {
        ret = -1;
        return ret;
    }
    ret=write(fd, dir, strlen(dir));
    close(fd);
    return ret;
}
int gpiofd=0;
int gpio_ope_val(int pin)
{
    int ret = 0;
    char buf[50];
    sprintf(buf,"/sys/class/gpio/gpio%d/value",pin);
    gpiofd = open(buf, O_RDWR|O_SYNC);
    if(gpiofd<0)
    {
        ret=-1;
        return ret;
    }
    return ret;
}
int gpio_set_val(int value)
{
    int ret;
    static const char s_values_str[]="01";
    ret=write(gpiofd,&s_values_str[(value==0)?0:1],1);
    return ret;
}
void gpio_clo_val()
{
    close(gpiofd);
}
#endif //PWMSERVO_GPIO_H
