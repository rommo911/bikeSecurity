#ifndef MAIN_H
#define MAIN_H
#pragma once
#include "system.hpp"
#include "sdkconfig.h"
#include "buzzer.h"
#include "debug_tools.h"
#include "ota_server.h"
#include "string.h"
#include <string>
#include "nvs_tools.h"
//
#include "display.hpp"
#include "WebConfig.h"
#include "SparkFunMPU9250-DMP.h"
#include "TinyGPSPlus.h"

extern volatile bool imuWoke;    // ISR Woke-on-Motion Flag
extern MPU9250_DMP imu;
extern EventLoop_p_t event_loop;
extern TinyGPSPlus gps;

#define DEFAULT_CLIENT_SSID "SFR_3DD0"
#define DEFAULT_AP_PASS "11112222"

#define BUTTON_PIN2 GPIO_NUM_13 //TMS

#define SDA_PIN 4
#define SCLK_PIN 16
#define BUZZ_PIN GPIO_NUM_27
#define LED_PIN GPIO_NUM_21
void setupGps();
void PrintGPS();
bool SetupImu();
void feed();

void UpdateMainPageInfo();
esp_err_t sendOtaStarted(void* arg);
esp_err_t sendOtaFinished(void* arg);
//WM 
bool StartupConfigPortal();
bool checkForReset(bool _force = false);

//BME
static constexpr char *ntpServer = {"pool.ntp.org"};
static const long gmtOffset_sec = 3600;
static const int daylightOffset_sec = 3600;

//*OLED
#define _SCREEN_ADDR 0x78 //
//
#endif