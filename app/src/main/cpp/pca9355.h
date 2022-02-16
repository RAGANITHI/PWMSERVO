//
// Created by johnd on 10-11-2021.
//

#ifndef PWMSERVO_PCA9355_H
#define PWMSERVO_PCA9355_H
#include"i2c6.h"
#define PCA9355_ADDRESS 0x62
#define PSC0_REG   0x01
#define PWM0_REG   0x02
#define PSC1_REG   0x03
#define PWM1_REG   0x04
#define LEDS_REG   0x05

__unused uint8_t pca9355_read(uint8_t reg_addr)
{
    opendevice();
    uint8_t buf;
    read_i2c_device_8(PCA9355_ADDRESS, reg_addr, &buf);
    closedevice();
    return buf;
}
__unused void pca9355_write(uint8_t reg_addr, uint8_t buf)
{
    opendevice();
    write_i2c_device_8(PCA9355_ADDRESS, reg_addr, &buf);
    closedevice();
}
void pca9355_init(void)
{
    pca9355_write(PSC0_REG, 0x02);//PWM 0 - 50Hz frequency
    pca9355_write(PWM0_REG, 244);//PWM 0 - 50% duty cycle
    pca9355_write(PSC1_REG, 0x02);//PWM 1 - 50Hz frequency
    pca9355_write(PWM1_REG, 0x7F);//PWM 1 - 50% duty cycle
    pca9355_write(LEDS_REG, 0x02);//LED 0 set to use PWM0 blink rate. All others set to high impedance.
}
void pca9355_set_duty_cycle(uint8_t dutycycle)
{
    pca9355_write(PWM0_REG, dutycycle);
}
#endif //PWMSERVO_PCA9355_H
