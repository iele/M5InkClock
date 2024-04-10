#include <M5Unified.h>
#include <WiFi.h>

#include "main_screen.hpp"
#include "app_screen.hpp"
#include "env_hat.hpp"
#include "utils.hpp"

const char *ssid = "test";
const char *password = "12341234";

const char *ntpServer = "ntp.aliyun.com";
const char *gmt = "UTC-0";

void setup(void)
{
    setCpuFrequencyMhz(80);
    M5.begin();
    env_begin();

    M5.Display.setEpdMode(epd_fastest);

    if (esp_reset_reason() != ESP_RST_DEEPSLEEP)
    {
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.fillScreen(TFT_WHITE);

        configTzTime(gmt, ntpServer);
        M5.Display.setTextSize(1);
        M5.Display.setTextFont(&Font0);
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

        for (int i = 0; i < DATA_POINTS; i++)
        {
            temp_hum_data[i].temperature = -0xFF;
            temp_hum_data[i].humidity = -0xFF;
        }

        M5.Display.println("\nWait time sync");
        while ((millis() / 1000) % 61 != 0)
        {
            M5.Display.setTextColor(TFT_BLACK, TFT_WHITE);
            M5.Display.drawNumber(millis() / 1000, 0, M5.Display.height() - 20);
            M5.Display.print(".");
            delay(1000);
        }
        M5.Display.println("\nOK");

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

    M5.Display.powerSaveOn();

    pinMode(GPIO_NUM_27, OUTPUT);
    digitalWrite(GPIO_NUM_27, HIGH);
    gpio_hold_en(GPIO_NUM_12);

    M5.Power.deepSleep(60 * 1000 * 1000);
    M5.Display.powerSaveOff();
}
