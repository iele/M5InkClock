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

    M5.Display.setEpdMode(epd_fast);
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

void loop()
{
    M5.update();
    if (M5.BtnPWR.isPressed())
    {
        powerOn = !powerOn;
    }
    if (powerOn)
    {
        appScreen();
        powerOn = false;
    } 
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.fillScreen(TFT_WHITE);

    mainScreen();
}
