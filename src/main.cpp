#include <M5Unified.h>
#include <WiFi.h>

#include "main_screen.hpp"
#include "app_screen.hpp"
#include "env_hat.hpp"
#include "utils.hpp"

void setup(void)
{
    pinMode(GPIO_NUM_12, OUTPUT);
    digitalWrite(GPIO_NUM_12, HIGH);
    setCpuFrequencyMhz(80);

    auto cfg = M5.config();
    M5.begin(cfg);

    M5.Display.powerSaveOff();
    M5.Display.setEpdMode(epd_text);
    M5.Display.fillScreen(TFT_WHITE);
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
    M5.Display.sleep();
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_27, 0);
    esp_sleep_enable_timer_wakeup((61-second)*1000*1000);
    esp_deep_sleep_start();
 }
