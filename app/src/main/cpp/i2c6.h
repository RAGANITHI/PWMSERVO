//
// Created by johnd on 13-10-2021.
//

#ifndef PWMSERVO_I2C6_H
#define PWMSERVO_I2C6_H

#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <cstdlib>
#include <errno.h>
#include<android/log.h>

int i2cfd;
static const char *device_name = "/dev/i2c-6";
#define ARRAY_SIZE(a)	(sizeof(a) / sizeof(*a))
int opendevice(void)
{
    i2cfd = open(device_name, O_RDWR);
    return i2cfd;
}
void closedevice(void)
{
    close(i2cfd);
}
int do_rdwr(struct i2c_msg *msgs, int nmsgs)
{
    struct i2c_rdwr_ioctl_data msgset = {
            .msgs = msgs,
            .nmsgs = static_cast<__u32>(nmsgs),
    };
    if (msgs == NULL || nmsgs <= 0)
    {
        return -1;
    }
    if (ioctl(i2cfd, I2C_RDWR, &msgset) < 0)
    {
        __android_log_print(ANDROID_LOG_ERROR,"do_rdwr","ioctl - failure: %s,%d",strerror(errno),i2cfd);
        return -1;
    }
    return 0;
}
int read_i2c_device_16(uint8_t address, uint16_t offset, uint8_t *buf, uint16_t count)
{
    uint8_t offset_data[] =  {static_cast<uint8_t>(offset >> 8), static_cast<uint8_t>(offset & 0xFF)};

    struct i2c_msg msgs[] = {
            {address,0,ARRAY_SIZE(offset_data),static_cast<__u8 *>((void *) offset_data)},
            {address,I2C_M_RD,count,static_cast<__u8 *>((void *) buf)}
    };

    if (do_rdwr(msgs, ARRAY_SIZE(msgs))) {
        return -1;
    }
    return 0;
}
int write_i2c_device_16(uint8_t address, uint16_t offset, const uint8_t *buf, int len)
{
    uint8_t *data = (uint8_t *)malloc((2 + len) * sizeof(*data));

    struct i2c_msg msgs[] = {
            {address,0,static_cast<__u16>((2 + len) * sizeof(*data)),static_cast<__u8 *>((void *) data)}
    };

    if (!data) {
        len = -1;
        goto err_malloc;
    }

    data[0] = offset >> 8;
    data[1] = offset & 0xFF;
    memcpy(data + 2, buf, len);

    if (do_rdwr(msgs, ARRAY_SIZE(msgs))) {
        len = -1;
        goto err_do_rdwr;
    }

    err_do_rdwr:
    free(data);
    err_malloc:
    return len;
}
int read_i2c_device_8(uint8_t address, uint8_t offset, uint8_t* buf)
{
    struct i2c_msg msgs[] = {
            {address,0,1,&offset},
            {address,I2C_M_RD,1,buf}
    };

    if (do_rdwr(msgs, ARRAY_SIZE(msgs))) {
        return -1;
    }
    return 0;
}
int write_i2c_device_8(uint8_t address, uint8_t offset, uint8_t *data)
{
    int retval =2;
    uint8_t *buffer = (uint8_t *)malloc(2*sizeof(*data));
    struct i2c_msg msgs[] = {
            {address,0,2,buffer}
    };
    if(!buffer)
    {
        retval = -1;
        return retval;
    }
    buffer[0] = offset;
    memcpy(buffer+1,data,1);
    if (do_rdwr(msgs, ARRAY_SIZE(msgs))) {
        retval = -1;
    }
    free(buffer);
    return retval;
}
#endif //PWMSERVO_I2C6_H
