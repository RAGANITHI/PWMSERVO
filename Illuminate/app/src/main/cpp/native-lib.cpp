//Company: Adiuvo Diagnostics Private Limited
//Code by: John King, Lead Product Developer
//Date Modified: 24th November 2020
//Version 0
//Revision 1
#include <jni.h>
#include <string>
#include <time.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <android/bitmap.h>

#include <android/log.h>
#define  LOG_TAG    "Native"
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

using namespace std;
/////////////////////////////////////////////////////////////////////////////////////////////////
//I2C Bus Functions Read Write Functions
/////////////////////////////////////////////////////////////////////////////////////////////////
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
        return -1;
    if (ioctl(i2cfd, I2C_RDWR, &msgset) < 0)
        return -1;
    return 0;
}
int read_i2c_device(uint8_t address, uint16_t offset, uint8_t *buf, uint16_t count)
{
    uint8_t offset_data[] =  {static_cast<uint8_t>(offset >> 8), static_cast<uint8_t>(offset & 0xFF)};

    struct i2c_msg msgs[] = {
            [0] = {
                    .addr = address,
                    .flags = 0,
                    .buf = static_cast<__u8 *>((void *) offset_data),
                    .len = ARRAY_SIZE(offset_data),
            },
            [1] = {
                    .addr = address,
                    .flags = I2C_M_RD,
                    .buf = static_cast<__u8 *>((void *) buf),
                    .len = count,
            },
    };

    __android_log_print(ANDROID_LOG_ERROR,"Native","Reading %d bytes from slave-addr:0x%x offset:0x%x\n",count,address,offset);

    if (do_rdwr(msgs, ARRAY_SIZE(msgs))) {
        return -1;
    }
    return 0;
}
int write_i2c_device(uint8_t address, uint16_t offset, const uint8_t *buf, int len)
{
    uint8_t *data = (uint8_t *)malloc((2 + len) * sizeof(*data));

    struct i2c_msg msgs[] = {
            [0] = {
                    .addr = address,
                    .flags = 0,
                    .buf = static_cast<__u8 *>((void *) data),
                    .len = static_cast<__u16>((2 + len) * sizeof(*data)),
            },
    };

    if (!data) {
        len = -1;
        goto err_malloc;
    }

    __android_log_print(ANDROID_LOG_ERROR,"Native","Writing %d bytes to slave-addr:0x%x offset:0x%x\n",len,address,offset);

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
/////////////////////////////////////////////////////////////////////////////////////////////////
//VL6180X Functions
/////////////////////////////////////////////////////////////////////////////////////////////////
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
    read_i2c_device(VL6180X_ADDRESS,reg_addr,&buf,1);
    closedevice();
    return buf;
}
int vl6180x_write(uint16_t reg_addr,uint8_t buf)
{
    opendevice();
    write_i2c_device(VL6180X_ADDRESS,reg_addr,&buf,1);
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
/////////////////////////////////////////////////////////////////////////////////////////////////
//PCA9685 Functions
/////////////////////////////////////////////////////////////////////////////////////////////////
// REGISTER ADDRESSES
#define PCA9685_ADDRESS  0x40
#define FREQUENCY_OSCILLATOR 25000000
#define PCA9685_PRESCALE_MIN 0x03
#define PCA9685_PRESCALE_MAX 0xFF
#define PCA9685_MODE1 0x00
#define MODE1_RESTART 0x80
#define PCA9685_PRESCALE 0xFE
int pca9685i2chandle;
uint8_t pca9685_read(uint8_t reg_addr)
{
    pca9685i2chandle = open(device_name, O_RDWR);
    if (pca9685i2chandle < 0) {
        LOGE("The i2c bus couldn't open!");
    }
    if (ioctl(pca9685i2chandle, I2C_SLAVE, PCA9685_ADDRESS) < 0) {
        LOGE("Device not found");
    }
    uint8_t buf[1] = {reg_addr};
    if (write(pca9685i2chandle, buf, 1) != 1) {
        LOGE("fail read 1");
    }
    if (read(pca9685i2chandle, buf, 1) != 1) {
        LOGE("fail read 2");
    }
    close(pca9685i2chandle);
    return buf[0];
}
int pca9685_write(uint8_t reg_addr,uint8_t val)
{
    pca9685i2chandle = open(device_name, O_RDWR);
    if (pca9685i2chandle < 0) {
        LOGE("The i2c bus couldn't open!");
        return -1;
    }
    if (ioctl(pca9685i2chandle, I2C_SLAVE, PCA9685_ADDRESS) < 0) {
        LOGE("Device not found");
        close(pca9685i2chandle);
        return -1;
    }
    uint8_t packet[2] = {reg_addr,val};
    if (write(pca9685i2chandle, packet, 2) != 2) {
        LOGE("fail write");
        close(pca9685i2chandle);
        return -1;
    }
    close(pca9685i2chandle);
    return 0;
}
void pca9685_init(jfloat freq) {
    if (freq < 1)
        freq = 1;
    if (freq > 3500)
        freq = 3500;
    float prescaleval = (((float) FREQUENCY_OSCILLATOR / (freq * 4096.0)) - 1);
    if (prescaleval < PCA9685_PRESCALE_MIN)
        prescaleval = PCA9685_PRESCALE_MIN;
    if (prescaleval > PCA9685_PRESCALE_MAX)
        prescaleval = PCA9685_PRESCALE_MAX;
    uint8_t prescale = (uint8_t)prescaleval;

    pca9685_write(PCA9685_MODE1, 0x10);// go to sleep
    pca9685_write(PCA9685_PRESCALE, prescale); // set the prescaler
    pca9685_write(PCA9685_MODE1, 0x00);
    usleep(500);
    while(pca9685_read(PCA9685_MODE1) & MODE1_RESTART){
        pca9685_write(PCA9685_MODE1, MODE1_RESTART);//Restart
    }
}
void pca9685_setpwm(int channel, int pulselength) {
    uint8_t pulseonl = 0;
    uint8_t pulseonh = 0 >> 8;
    uint8_t pulseofl = pulselength;
    uint8_t pulseofh = pulselength >> 8;
    pca9685_write((uint8_t)(0x06+(4*channel)+0), pulseonl);
    pca9685_write((uint8_t)(0x06+(4*channel)+1), pulseonh);
    pca9685_write((uint8_t)(0x06+(4*channel)+2), pulseofl);
    pca9685_write((uint8_t)(0x06+(4*channel)+3), pulseofh);
}
/////////////////////////////////////////////////////////////////////////////////////////////////
//LED ON / OFF Control Code
//And Servo rotation control Code
/////////////////////////////////////////////////////////////////////////////////////////////////
int n365nmledchannel = 0;
int n395nmledchannel = 1;
int n415nmledchannel = 2;
int nwhinmledchannel = 3;
int nservomotchannel = 4;
int n415nmfilterposition = 150;
int n470nmfilterposition = 250;
int nnonmfilterposition = 350;
int n525nmfilterposition = 450;
int n600nmfilterposition = 550;
extern "C"
JNIEXPORT void JNICALL
Java_com_example_illuminate_MainActivity_turnon365nmled(JNIEnv *env, jobject thiz) {
    // TODO: implement turnon365nmled()
    pca9685_setpwm(n365nmledchannel,4095);
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_illuminate_MainActivity_turnon395nmled(JNIEnv *env, jobject thiz) {
    // TODO: implement turnon395nmled()
    pca9685_setpwm(n395nmledchannel,4095);
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_illuminate_MainActivity_turnon415nmled(JNIEnv *env, jobject thiz) {
    // TODO: implement turnon415nmled()
    pca9685_setpwm(n415nmledchannel,4095);
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_illuminate_MainActivity_turnonwhinmled(JNIEnv *env, jobject thiz) {
    // TODO: implement turnonwhinmled()
    pca9685_setpwm(nwhinmledchannel,4095);
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_illuminate_MainActivity_turnoffallled(JNIEnv *env, jobject thiz) {
    // TODO: implement turnoffallled()
    pca9685_setpwm(n365nmledchannel,0);
    pca9685_setpwm(n395nmledchannel,0);
    pca9685_setpwm(n415nmledchannel,0);
    pca9685_setpwm(nwhinmledchannel,0);
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_illuminate_MainActivity_set415nmfilter(JNIEnv *env, jobject thiz) {
    // TODO: implement set415nmfilter()
    pca9685_setpwm(nservomotchannel,n415nmfilterposition);
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_illuminate_MainActivity_set470nmfilter(JNIEnv *env, jobject thiz) {
    // TODO: implement set470nmfilter()
    pca9685_setpwm(nservomotchannel,n470nmfilterposition);
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_illuminate_MainActivity_setnonmfilter(JNIEnv *env, jobject thiz) {
    // TODO: implement set470nmfilter()
    pca9685_setpwm(nservomotchannel,nnonmfilterposition);
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_illuminate_MainActivity_set525nmfilter(JNIEnv *env, jobject thiz) {
    // TODO: implement set525nmfilter()
    pca9685_setpwm(nservomotchannel,n525nmfilterposition);
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_illuminate_MainActivity_set600nmfilter(JNIEnv *env, jobject thiz) {
    // TODO: implement set600nmfilter()
    pca9685_setpwm(nservomotchannel,n600nmfilterposition);
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_illuminate_MainActivity_getdepth(JNIEnv *env, jobject thiz) {
    // TODO: implement getdepth()
    return vl6180x_getRange();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_illuminate_MainActivity_control_1init(JNIEnv *env, jobject thiz, jfloat pwmfreq) {
    // TODO: implement control_init()
    vl6180x_init();
    pca9685_init(pwmfreq);
}
/////////////////////////////////////////////////////////////////////////////////////////////////
//The END
/////////////////////////////////////////////////////////////////////////////////////////////////