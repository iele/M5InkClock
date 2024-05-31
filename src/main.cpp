#include <M5Unified.h>
#include <WiFi.h>
#include <ESPPerfectTime.h>

#include "main_screen.hpp"
#include "app_screen.hpp"
#include "env_hat.hpp"
#include "utils.hpp"

const char *ssid = "test";
const char *password = "12341234";

const char *ntpServer1 = "ntp.ntsc.ac.cn";
const char *ntpServer2 = "pool.ntp.org";
const char *ntpServer3 = "ntp.aliyun.com";

const char *gmt = "UTC-0";

void setup(void)
{
    setCpuFrequencyMhz(80);
    M5.begin();

    M5.Display.setEpdMode(epd_fastest);

    if (esp_reset_reason() != ESP_RST_DEEPSLEEP)
    {
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.fillScreen(TFT_WHITE);

        M5.Display.setTextSize(1);
        M5.Display.setFont(&Font0);
        M5.Display.println("Prepare your hotspot:");
        M5.Display.printf("SSID: %s\n", ssid);
        M5.Display.printf("PASSWORD: %s\n\n", password);
        M5.Display.print("Connecting\n");
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
            M5.Display.print(".");
        }
        M5.Display.println("\nCONNECTED");

        static bool sync_success = false;
        auto callback = []()
        {
            struct tm *timeinfo;
            suseconds_t usec;
            timeinfo = pftime::localtime(nullptr, &usec);
            M5.Rtc.setDateTime(timeinfo);
            M5.Display.println("\nOK");
            auto tm = M5.Rtc.getDateTime().get_tm();
            M5.Display.printf("Current Time: %2d:%2d:%2d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
            // disconnect WiFi as it's no longer needed
            WiFi.disconnect(true);
            WiFi.mode(WIFI_OFF);
            sync_success = true;
        };
        pftime::setSyncSuccessCallback(callback);
        pftime::configTzTime(gmt, ntpServer1, ntpServer2, ntpServer3);
        M5.Display.println("\nTime Setting ");
        while(!sync_success){
            delay(1000);
            Serial.print(".");
            M5.Display.print(".");
        }

        for (int i = 0; i < DATA_POINTS; i++)
        {
            temp_hum_data[i].temperature = -0xFF;
            temp_hum_data[i].humidity = -0xFF;
        }

        M5.Display.fillScreen(TFT_WHITE);
    }
}

void loop()
{
    M5.update();
    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0)
    {
        setCpuFrequencyMhz(240);
        M5.Power.setLed(255);
        appScreen();

        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.fillScreen(TFT_WHITE);
        M5.Power.setLed(0);
    }
    setCpuFrequencyMhz(80);
    mainScreen();

    pinMode(GPIO_NUM_27, OUTPUT);
    digitalWrite(GPIO_NUM_27, HIGH);
    gpio_hold_en(GPIO_NUM_12);

    auto second = M5.Rtc.getDateTime().get_tm().tm_sec;

    M5.Display.powerSaveOn();
    M5.Power.deepSleep((61 - second) * 1000 * 1000);
}
