#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#pragma once
#include <vector>
#include <string>
#include "esp_attr.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <U8g2lib.h>
#include "FreeRTOS.hpp"
#include "system.hpp"
#include "task_tools.h"

class OLED : private Task
{
public:
    static constexpr char TAG[] = "OLED";
    EventGroupHandle_t eventGroup = nullptr;
    enum Disp_Page_t
    {
        MAIN_PAGE = 0,
        SET_PAGE = 1,
        Blind_Moving_PAGE = 2,
        WIFI_PAGE = 3,
        OTA_start_PAGE = 4,
        OTA_END_PAGE = 5,
        INFO_PAGE = 6,
        IDLE,
    };
    struct oled_display_t
    {
        const std::string& _text;
        const int x;
        const int y;
        const uint8_t* font = u8g2_font_ncenB12_tf;
    };
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C *u8g2 = nullptr;
    Disp_Page_t CurrentPage, lastPage;
    using vec_disp = std::vector<oled_display_t>;
    ~OLED() = default;
    OLED();
    OLED(const int add);
    void setup(const int add);
    void ClearDisplay(bool forced = false);
    void SetPowerSave(bool ps);
    bool GetPowerSave() const;
    void SetPage(const Disp_Page_t& type);
    void ReturnLastPage();

    struct
    {
        std::string upperText;
        std::string LowerText;
        float temp;
        float humidity;
        uint8_t value;
        uint8_t connection;
    } MainPageArg;
    struct
    {
        std::string mac;
    } WifiPageArgs;
    struct
    {
        std::string upperText;
        std::string LowerText;
        uint8_t value;
        uint8_t Targetvalue;
        uint8_t connection;
    } SetPageArgs;
    struct
    {
        std::string upperText;
        std::string LowerText;
        uint8_t value;
    } OtaPageArgs;
    struct
    {
        std::string LowerText;
        uint8_t value;
        uint8_t TargetValue;
        uint8_t direction;
    } BlindMovingPageArgs;
    struct
    {
        std::string ip;
        std::string mac;
        std::string version;
    } InfoPageArgs;
    void Refresh();

private:
    void Flush();
    void run(void* arg);
    void DrawMainPage();
    void DrawWifiPage();
    void DrawMovingPage();
    void DrawSetPage();
    void DrawInfoPage();
    void wifiConnectingIconTimerCb();
    std::unique_ptr<FreeRTOS_v2::Timer> wifiIconTimer = nullptr;
    void DrawWIFIPage(uint8_t arg);
    void DrawOTAPage(uint8_t arg);
    void display_text(const oled_display_t& line);
    bool showing_message = false;
    std::timed_mutex lock ;
    bool powerSave = false;
};

extern OLED* oled;

void Oled_Wakeup(); 
void Oled_Sleep();

#endif // __DISPLAY_H__