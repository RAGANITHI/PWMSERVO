//Company: Adiuvo Diagnostics Private Limited
//Code by: John King, Lead Product Developer
//Date Modified: 29th September 2021
//Version 0
//Revision 2
#include <jni.h>
#include <unistd.h>
#include "power.h"
#include "pca9355.h"
#include "si5356.h"
#include "vl6180x.h"
/////////////////////////////////////////////////////////////////////////////////////////////////
//LED ON / OFF Control Code
//And Servo rotation control Code
/////////////////////////////////////////////////////////////////////////////////////////////////
//int n415nmfilterposition = 700000;
//int n470nmfilterposition = 1150000;
//int nnonmfilterposition = 1550000;
//int n525nmfilterposition = 2000000;
//int n600nmfilterposition = 2450000;
uint8_t n415nmfilterposition = 80;
uint8_t n470nmfilterposition = 110;
uint8_t nnonmfilterposition = 144;
uint8_t n525nmfilterposition = 200;
uint8_t n600nmfilterposition = 245;
extern "C"
JNIEXPORT void JNICALL
Java_com_example_pwmservo_MainActivity_turnon365nmled(JNIEnv *env, jobject thiz) {
    // TODO: implement turnon365nmled()
    si5356_ledon_365nm();
    si5356_ledoff_395nm();
    si5356_ledoff_415nm();
    si5356_ledoff_whinm();
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_pwmservo_MainActivity_turnon395nmled(JNIEnv *env, jobject thiz) {
    // TODO: implement turnon395nmled()
    si5356_ledoff_365nm();
    si5356_ledon_395nm();
    si5356_ledoff_415nm();
    si5356_ledoff_whinm();
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_pwmservo_MainActivity_turnon415nmled(JNIEnv *env, jobject thiz) {
    // TODO: implement turnon415nmled()
    si5356_ledoff_365nm();
    si5356_ledoff_395nm();
    si5356_ledon_415nm();
    si5356_ledoff_whinm();
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_pwmservo_MainActivity_turnonwhinmled(JNIEnv *env, jobject thiz) {
    // TODO: implement turnonwhinmled()
    si5356_ledoff_365nm();
    si5356_ledoff_395nm();
    si5356_ledoff_415nm();
    si5356_ledon_whinm();
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_pwmservo_MainActivity_turnoffallled(JNIEnv *env, jobject thiz) {
    // TODO: implement turnoffallled()
    si5356_ledoff_365nm();
    si5356_ledoff_395nm();
    si5356_ledoff_415nm();
    si5356_ledoff_whinm();
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_pwmservo_MainActivity_set415nmfilter(JNIEnv *env, jobject thiz) {
    // TODO: implement set415nmfilter()
    pca9355_set_duty_cycle(n415nmfilterposition);
    usleep(1000000);
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_pwmservo_MainActivity_set470nmfilter(JNIEnv *env, jobject thiz) {
    // TODO: implement set470nmfilter()
    pca9355_set_duty_cycle(n470nmfilterposition);
    usleep(1000000);
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_pwmservo_MainActivity_setnonmfilter(JNIEnv *env, jobject thiz) {
    // TODO: implement setNOnmfilter()
    pca9355_set_duty_cycle(nnonmfilterposition);
    usleep(1000000);
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_pwmservo_MainActivity_set525nmfilter(JNIEnv *env, jobject thiz) {
    // TODO: implement set525nmfilter()
    pca9355_set_duty_cycle(n525nmfilterposition);
    usleep(1000000);
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_pwmservo_MainActivity_set600nmfilter(JNIEnv *env, jobject thiz) {
    // TODO: implement set600nmfilter()
    pca9355_set_duty_cycle(n600nmfilterposition);
    usleep(1000000);
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_pwmservo_MainActivity_getdepth(JNIEnv *env, jobject thiz) {
    // TODO: implement getdepth()
    return vl6180x_getRange();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_pwmservo_MainActivity_control_1init(JNIEnv *env, jobject thiz) {
    // TODO: implement control_init()
//    power_init();
    vl6180x_init();
    pca9355_init();
    si5356_init();
}
/////////////////////////////////////////////////////////////////////////////////////////////////
//The END
/////////////////////////////////////////////////////////////////////////////////////////////////