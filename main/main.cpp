#include "main.h"
#include "esp_pm.h"
#include "boot_cfg.h"
#include "Button2.h"
#include "Wire.h"
#include "mqttv2.hpp"
#include "nvs_flash.h"
#include "esp_event_cxx.hpp"
#include "BleDeviceFinder.hpp"
BleDeviceFinder *BLEFinder = nullptr;
EventLoop_p_t event_loop = nullptr;
Button2 *button{nullptr};

// Create an instance of the MPU9250_DMP class

void HookEvents()
{
    button->setClickHandler([](Button2 &btn)
                            { oled->SetPage(OLED::Disp_Page_t::MAIN_PAGE); });

    otaServer->SetPreOtaCb([&](void *)
                           {
        if (oled != nullptr)
        {
            oled->SetPage(OLED::Disp_Page_t::OTA_start_PAGE);
        }
        return ESP_OK; });
    otaServer->SetPostOtaCb([&](void *)
                            {
        if (oled != nullptr)
        {
            oled->SetPage(OLED::Disp_Page_t::OTA_END_PAGE);
        }
        return ESP_OK; });
}

extern "C" void app_main(void)
{
    nvs_flash_init();
    bootConfig = std::make_unique<BootConfig_t>();
    bootConfig->Boot();
    esp_log_level_set("wifi-init", ESP_LOG_WARN);
    event_loop = std::make_shared<EventLoop>();
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCLK_PIN, (uint32_t)4000000);
    oled = new OLED(_SCREEN_ADDR);
    oled->ClearDisplay();
    checkForReset(false);
    tools::dumpHeapInfo();
    button = new Button2(BUTTON_PIN2, INPUT, false, true);
    BLEFinder = new BleDeviceFinder([&](const BLEAdvertisedDevice &device) {});
    BLEFinder->AddDeviceMac("dfdsfsdf");
    BLEFinder->StartScanning();
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_PIN, 0);
    HookEvents();
    oled->SetPage(OLED::Disp_Page_t::MAIN_PAGE);
    SetupImu();
    setupGps();
    tools::dumpHeapInfo();
    auto now = GET_NOW_MINUTES;
    if (bootConfig->IsNeedValidation())
    {
        ESP_LOGW("SYSTEM OTA ", "WAiting for 5 seconds before validating OTA");
        std::this_thread::sleep_for(std::chrono::seconds(5));
        bootConfig->SetValidOtaImage();
    }
}