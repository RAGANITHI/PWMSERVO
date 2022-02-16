//
// Created by johnd on 13-10-2021.
//

#ifndef PWMSERVO_VL6180X_H
#define PWMSERVO_VL6180X_H
/////////////////////////////////////////////////////////////////////////////////////////////////
//VL6180X Functions
/////////////////////////////////////////////////////////////////////////////////////////////////
#include "i2c6.h"

#define VL6180X_ADDRESS  0x29
#define VL6180X_REG_IDENTIFICATION_MODEL_ID 0x00
#define SYSRANGE__INTERMEASUREMENT_PERIOD 0x001b
#define VL6180X_REG_RESULT_RANGE_STATUS 0x04d
#define VL6180X_REG_SYSRANGE_START 0x018
#define VL6180X_REG_RESULT_INTERRUPT_STATUS_GPIO 0x04f
#define VL6180X_REG_RESULT_RANGE_VAL 0x062
#define VL6180X_REG_SYSTEM_INTERRUPT_CLEAR 0x015
uint8_t vl6180x_read(uint16_t reg_addr)
{
    opendevice();
    uint8_t buf;
    read_i2c_device_16(VL6180X_ADDRESS, reg_addr, &buf, 1);
    closedevice();
    return buf;
}
int vl6180x_write(uint16_t reg_addr,uint8_t buf)
{
    opendevice();
    write_i2c_device_16(VL6180X_ADDRESS, reg_addr, &buf, 1);
    closedevice();
    return 0;
}
int vl6180x_getRange() {
    // wait for device to be ready for range measurement
    while (!(vl6180x_read(VL6180X_REG_RESULT_RANGE_STATUS) & 0x01));
    // Start a range measurement
    vl6180x_write(VL6180X_REG_SYSRANGE_START, 0x01);
    // Poll until bit 2 is set
    while (!(vl6180x_read(VL6180X_REG_RESULT_INTERRUPT_STATUS_GPIO) & 0x04));
    // read range in mm
    uint8_t range = vl6180x_read(VL6180X_REG_RESULT_RANGE_VAL);
    // clear interrupt
    vl6180x_write(VL6180X_REG_SYSTEM_INTERRUPT_CLEAR, 0x07);
    return range;
}
int vl6180x_init() {
    if(vl6180x_read(VL6180X_REG_IDENTIFICATION_MODEL_ID)==0xB4)
    {
        if(vl6180x_write(0x0207, 0x01)<0)return -1;
        if(vl6180x_write(0x0208, 0x01)<0)return -1;
        if(vl6180x_write(0x0096, 0x00)<0)return -1;
        if(vl6180x_write(0x0097, 0xfd)<0)return -1;
        if(vl6180x_write(0x00e3, 0x00)<0)return -1;
        if(vl6180x_write(0x00e4, 0x04)<0)return -1;
        if(vl6180x_write(0x00e5, 0x02)<0)return -1;
        if(vl6180x_write(0x00e6, 0x01)<0)return -1;
        if(vl6180x_write(0x00e7, 0x03)<0)return -1;
        if(vl6180x_write(0x00f5, 0x02)<0)return -1;
        if(vl6180x_write(0x00d9, 0x05)<0)return -1;
        if(vl6180x_write(0x00db, 0xce)<0)return -1;
        if(vl6180x_write(0x00dc, 0x03)<0)return -1;
        if(vl6180x_write(0x00dd, 0xf8)<0)return -1;
        if(vl6180x_write(0x009f, 0x00)<0)return -1;
        if(vl6180x_write(0x00a3, 0x3c)<0)return -1;
        if(vl6180x_write(0x00b7, 0x00)<0)return -1;
        if(vl6180x_write(0x00bb, 0x3c)<0)return -1;
        if(vl6180x_write(0x00b2, 0x09)<0)return -1;
        if(vl6180x_write(0x00ca, 0x09)<0)return -1;
        if(vl6180x_write(0x0198, 0x01)<0)return -1;
        if(vl6180x_write(0x01b0, 0x17)<0)return -1;
        if(vl6180x_write(0x01ad, 0x00)<0)return -1;
        if(vl6180x_write(0x00ff, 0x05)<0)return -1;
        if(vl6180x_write(0x0100, 0x05)<0)return -1;
        if(vl6180x_write(0x0199, 0x05)<0)return -1;
        if(vl6180x_write(0x01a6, 0x1b)<0)return -1;
        if(vl6180x_write(0x01ac, 0x3e)<0)return -1;
        if(vl6180x_write(0x01a7, 0x1f)<0)return -1;
        if(vl6180x_write(0x0030, 0x00)<0)return -1;

        // Recommended : Public registers - See data sheet for more detail
        if(vl6180x_write(0x0011, 0x10)<0)return -1; // Enables polling for 'New Sample ready'
        // when measurement completes
        if(vl6180x_write(0x010a, 0x30)<0)return -1; // Set the averaging sample period
        // (compromise between lower noise and
        // increased execution time)
        if(vl6180x_write(0x003f, 0x46)<0)return -1; // Sets the light and dark gain (upper
        // nibble). Dark gain should not be
        // changed.
        if(vl6180x_write(0x0031, 0xFF)<0)return -1; // sets the # of range measurements after
        // which auto calibration of system is
        // performed
        if(vl6180x_write(0x0040, 0x63)<0)return -1; // Set ALS integration time to 100ms
        if(vl6180x_write(0x002e, 0x01)<0)return -1; // perform a single temperature calibration
        // of the ranging sensor

        // Optional: Public registers - See data sheet for more detail
        if(vl6180x_write(SYSRANGE__INTERMEASUREMENT_PERIOD,
                         0x09)<0)return -1;         // Set default ranging inter-measurement
        // period to 100ms
        if(vl6180x_write(0x003e, 0x31)<0)return -1; // Set default ALS inter-measurement period
        // to 500ms
        if(vl6180x_write(0x0014, 0x24)<0)return -1; // Configures interrupt on 'New Sample
        // Ready threshold event'
        return 0;
    } else
    {
        return -1;
    }
}
#endif //PWMSERVO_VL6180X_H
