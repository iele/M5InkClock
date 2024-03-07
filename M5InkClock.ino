#include <M5Unified.h>
#include <WiFi.h>

#include "main_screen.hpp"
#include "app_screen.hpp"
#include "env_hat.hpp"
#include "utils.hpp"

// WiFi Settings
const char *ssid = "H&Q";
const char *password = "hengheng&queen123";

// NTP Settings
const char *ntpServer = "ntp.aliyun.com";
const char *gmt = "UTC-0";

bool powerOn;

void setup(void)
{
    M5.begin();
    env_begin();

    Serial.println(esp_reset_reason());
    Serial.println(esp_sleep_get_wakeup_cause());
    M5.Display.setEpdMode(epd_fast);

    if (esp_reset_reason() != ESP_RST_DEEPSLEEP)
    {
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.fillScreen(TFT_WHITE);

        configTzTime(gmt, ntpServer);
        M5.Display.setTextSize(1);
        M5.Display.print("Connecting\n");
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
            M5.Display.print(".");
        }
        M5.Display.println("\nCONNECTED");
        M5.Display.println("\nTime Setting ");
        struct tm timeinfo;
        while (!getLocalTime(&timeinfo, 1000))
        {
            M5.Display.print(".");
            delay(500);
        }
        time_t t = time(nullptr) + 1;
        while (t > time(nullptr))
            ;
        M5.Rtc.setDateTime(gmtime(&t));

        M5.Display.println("\nOK");
        // disconnect WiFi as it's no longer needed
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        M5.Display.fillScreen(TFT_WHITE);
    }
}

void loop()
{
    M5.Power.setLed(255);
    M5.update();
    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0)
    {
        appScreen();
    }

    mainScreen();
    M5.Power.setLed(0);

    M5.Display.powerSaveOn();

    pinMode(GPIO_NUM_27, OUTPUT);
    digitalWrite(GPIO_NUM_27, HIGH);
    gpio_hold_en(GPIO_NUM_12);

    M5.Power.deepSleep(60 * 1000 * 1000);
    M5.Display.powerSaveOff();
}
