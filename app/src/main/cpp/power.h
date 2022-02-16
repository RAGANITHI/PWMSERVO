//
// Created by johnd on 22-10-2021.
//

#ifndef PWMSERVO_POWER_H
#define PWMSERVO_POWER_H

#include <cstdio>
#include "gpio.h"

void power_init()
{
    char buf[50];
    sprintf(buf,"out");
    gpio_set_dir(buf,72);
    gpio_ope_val(72);
    gpio_set_val(1);
}
void power_deinit()
{
    char buf[50];
    sprintf(buf,"out");
    gpio_set_dir(buf,77);
    gpio_ope_val(77);
    gpio_set_val(0);
}
#endif //PWMSERVO_POWER_H
