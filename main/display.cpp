#include "display.hpp"
#include "esp_attr.h"
#include "esp_log.h"
#include "string.h"
#include "WiFi.h"
#define SDA_PIN 4
#define SCLK_PIN 16

OLED::OLED() : Task(TAG,4096)
{
    eventGroup = xEventGroupCreate();
    wifiIconTimer = std::make_unique< FreeRTOS_v2::Timer>([this](){this-> wifiConnectingIconTimerCb();},"wifi icon", true);
    wifiIconTimer->SetPeriod(1s);

}

OLED::OLED(const int add) : Task("OLED")
{
    eventGroup = xEventGroupCreate();
    setup(add);

}

void OLED::setup(const int add)
{
    u8g2 = new U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R2, 255, SCLK_PIN, SDA_PIN);
    if (add != 0)
        u8g2->setI2CAddress(add);
    u8g2->begin();
    u8g2->enableUTF8Print();
    u8g2->setPowerSave(false);
    powerSave = false;
    ClearDisplay(true);
    Flush();
    StartTask();
}

void OLED::display_text(const oled_display_t& line)
{
    u8g2->setFont(line.font == nullptr ? u8g2_font_ncenB12_tf : line.font);
    u8g2->drawStr(line.x, line.y, line._text.c_str());
}

void OLED::ReturnLastPage()
{
    CurrentPage = lastPage;
    Refresh();
}

void OLED::Flush()
{
    std::unique_lock L(lock);
    u8g2->sendBuffer();
}
void OLED::run(void* arg)
{
    while (1)
    {
        xEventGroupWaitBits(eventGroup, BIT(0), pdTRUE, pdFALSE, portMAX_DELAY);
        ClearDisplay();
        switch (CurrentPage)
        {
        case MAIN_PAGE:
        {
            DrawMainPage();
            break;
        }
        case OTA_start_PAGE:
        {
            DrawOTAPage(0);
            break;
        }
        case OTA_END_PAGE:
        {
            DrawOTAPage(1);
            break;
        }
        case WIFI_PAGE:
        {
            DrawWIFIPage(1);
            break;

        }
        case INFO_PAGE:
        {
            DrawInfoPage();
            break;
        }
        case IDLE:
        {
            break;
        }
        default:
        {
            DrawMainPage();
            break;
        }
        }
        Flush();
    }
}
void OLED::ClearDisplay(bool forced)
{
    std::unique_lock L(lock);
    u8g2->clearBuffer();
}

void OLED::SetPowerSave(bool ps)
{
    if (this->powerSave == ps)
        return;
    this->powerSave = ps;
    u8g2->setPowerSave(ps);
    if (powerSave)
    {
        lastPage = CurrentPage;
        CurrentPage = IDLE;
    }
    else
    {
        CurrentPage = MAIN_PAGE;
    }
    Refresh();
}


bool OLED::GetPowerSave() const
{
    return powerSave;
}

void OLED::SetPage(const OLED::Disp_Page_t& type)
{
    this->lastPage = CurrentPage;
    CurrentPage = type;
    Refresh();
}
void OLED::DrawMainPage()
{
    std::unique_lock L(lock);

    if (MainPageArg.connection == 1)
    {
        u8g2->setFont(u8g2_font_open_iconic_www_2x_t);
        u8g2->drawGlyph(110, 15, 72); // wifi icon
    }
    u8g2->setFont(u8g2_font_open_iconic_weather_2x_t);
    u8g2->drawGlyph(0, 15, 64); // wheather icon
    const std::string str = tools::stringf("%.1f/%.1f", MainPageArg.temp < 40 ? MainPageArg.temp : 0, MainPageArg.humidity);
    const oled_display_t temp_hum = { str, 25, 15, u8g2_font_ncenB10_tr };
    display_text(temp_hum);
    //
    u8g2->setFont(u8g2_font_open_iconic_all_2x_t);
    //u8g2->drawGlyph(0, 40, 154); // up/down icon
    //
    //const oled_display_t second_line = { std::to_string(MainPageArg.value), 20, 40, u8g2_font_ncenB12_tf };
    //display_text(second_line);
}

void OLED::DrawInfoPage()
{
    std::unique_lock L(lock);
    const oled_display_t line1 = { InfoPageArgs.ip, 0, 10, u8g2_font_ncenB08_tf };
    const oled_display_t line2 = { InfoPageArgs.mac, 0, 30, u8g2_font_ncenB08_tf };
    const oled_display_t line3 = { InfoPageArgs.version, 0, 60, u8g2_font_ncenB08_tf };
    display_text(line1);
    display_text(line2);
    display_text(line3);

}
void OLED::DrawMovingPage()
{
    std::unique_lock L(lock);
    std::string str = std::to_string(BlindMovingPageArgs.TargetValue);
    u8g2->setFont(u8g2_font_ncenB12_tf);
    u8g2->drawStr(50, 59, str.c_str());
    u8g2->setFont(u8g2_font_open_iconic_all_2x_t);
    u8g2->drawFrame(2, 2, 125, 61);
    int pos = (BlindMovingPageArgs.value / 2);
    u8g2->drawBox(6, 6, 118, 50 - pos);
    uint16_t _glyph = 160;
    if (BlindMovingPageArgs.value < BlindMovingPageArgs.TargetValue)
        _glyph += 3;
    u8g2->drawGlyph(20, 59, _glyph);
    u8g2->drawGlyph(90, 59, _glyph);
}

void OLED::DrawSetPage()
{
    std::unique_lock L(lock);
    u8g2->setFont(u8g2_font_open_iconic_all_2x_t);
    u8g2->drawGlyph(.30, 20, 154); // up/down icon
    std::string str = "Set :" + std::to_string(SetPageArgs.Targetvalue);
    const oled_display_t first_line = { str, 30, 55, u8g2_font_ncenB12_tf };
    display_text(first_line);
}

void OLED::DrawOTAPage(uint8_t arg)
{
    std::unique_lock L(lock);
    const std::string t2 = arg ? "OTA FINISHED!" : "OTA Started";
    const oled_display_t second_line = { t2, 0, 30, u8g2_font_ncenB10_tf };
    display_text(second_line);
}


void OLED::DrawWIFIPage(uint8_t arg)
{
    std::unique_lock L(lock);
    const std::string t2 = "WIFI CONFIG";
    const oled_display_t second_line = { t2, 0, 15, u8g2_font_ncenB10_tf };
    display_text(second_line);
    const oled_display_t line = { WifiPageArgs.mac, 0, 40, u8g2_font_ncenB10_tf };
    display_text(line);
}

void OLED::Refresh()
{
    xEventGroupSetBits(eventGroup, BIT(0));
}

void OLED::wifiConnectingIconTimerCb()
{
    if (WiFi.status() != WL_CONNECTED)
    {

        MainPageArg.connection = !oled->MainPageArg.connection;
        Refresh();
    }
    else
    {
       MainPageArg.connection = 1;
       Refresh();
    }
}

OLED* oled = nullptr;


void Oled_Sleep()
{
    if (oled->GetPowerSave() == false)
    {
        oled->SetPowerSave(true);
    }
}
void Oled_Wakeup()
{
    oled->SetPage(OLED::Disp_Page_t::MAIN_PAGE);
    if (oled->GetPowerSave() == true)
    {
        oled->SetPowerSave(false);
        oled->SetPage(OLED::Disp_Page_t::MAIN_PAGE);
    }
}
