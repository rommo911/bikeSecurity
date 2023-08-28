#include "main.h"
WebServer ConfigurationServer;
WebConfig confClass(true);

static const String params = "["
                             "{'name':'ssid','label':'SSID',"
                             "'type':" OPTION_INPUTTEXT ","
                             "'default':'ssid-here'},"

                             "{'name':'pwd','label':'PASSSWORD',"
                             "'type':" OPTION_INPUTPASSWORD ","
                             "'default':''},"

                             "{'name':'mqtt_server','label':'MQTTT server',"
                             "'type':" OPTION_INPUTTEXT ","
                             "'default':'192.168.1.62'},"

                             "]";

bool StartupConfigPortal()
{

    tools::dumpHeapInfo();
    if (oled != nullptr)
    {
        oled->WifiPageArgs.mac = WiFi.macAddress().c_str();
        oled->SetPage(OLED::Disp_Page_t::WIFI_PAGE);
        oled->Refresh();
    }
    WiFi.enableSTA(false);
    WiFi.softAP(WiFi.macAddress().c_str(), nullptr, 10, 0, 1);
    bool configuring = true;
    confClass.setDescription(params, &ConfigurationServer);
    confClass.registerOnSave([&]()
                             { configuring = false; });
    ConfigurationServer.begin(80);
    ConfigurationServer.enableDelay(true);
    otaServer = std::make_unique<OTA_Server>(event_loop);
    otaServer->StopService();

    while (configuring)
    {
        ConfigurationServer.handleClient();
        delay(10);
    }
    ConfigurationServer.stop();
    WiFi.enableAP(false);
    return true;
}

bool checkForReset(bool _force)
{

    bool configured = true;
    do
    {
        auto nvs = OPEN_NVS("default");
        std::string ssid, pass, device;
        configured = nvs->getS("deviceName", device) == ESP_OK ? true : false;
        nvs->getS("ssid", ssid);
        nvs->getS("pwd", pass);
        nvs.reset();
        configured &= (ssid.length() > 3 && pass.length() > 7);
    } while (0);

    if (_force || (!configured))
    {
        if (StartupConfigPortal())
        {
            delay(3000);
            std::abort(); // reboot
        }
    }
    else
    {
        pinMode(BUTTON_PIN2, INPUT_PULLUP);
        int counter = 0;
        while (digitalRead(BUTTON_PIN2) == false && counter < 10)
        {
            std::this_thread::sleep_for(200ms);
            counter++;
        }
        if (counter >= 8 || _force)
        {
            if (StartupConfigPortal())
            {
                delay(3000);
                std::abort(); // reboot
            }
        }
    }

    return false;
}
