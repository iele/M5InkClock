#include <M5Unified.h>
#include <efontEnableCn.h>
#include <efontFontData.h>
#include "time.h"

#include "env_hat.hpp"
#include "utils.hpp"

#include "poem.h"

const char *months[12] = {"一月", "二月", "三月", "四月", "五月", "六月", "七月", "八月", "九月", "十月",
                          "十一月", "十二月"};

const char *days[32] = {"", "一日", "二日", "三日", "四日", "五日", "六日", "七日", "八日", "九日", "十日",
                        "十一日", "十二日", "十三日", "十四日", "十五日", "十六日", "十七日", "十八日", "十九日", "二十日",
                        "二十一", "二十二", "二十三", "二十四", "二十五", "二十六", "二十七", "二十八", "二十九", "三十日",
                        "三十一"};

const char *week[7] = {"日", "一", "二", "三", "四", "五", "六"};

void mainScreen()
{
    M5.Display.fillScreen(TFT_BLACK);
    M5.Display.fillScreen(TFT_WHITE);

    auto timeinfo = M5.Rtc.getDateTime().get_tm();
    auto tm = mktime(&timeinfo);
    auto ltm = (tm + 60 * 60 * 8);
    timeinfo = *gmtime(&ltm);

    M5.Display.startWrite();
    M5.Display.fillScreen(TFT_WHITE);

    // Power ----------------------------------------------------------
    int32_t level = M5.Power.getBatteryLevel();
    M5.Display.startWrite();
    char level_str[4];
    sprintf(level_str, "%d%%", level);
    M5.Display.fillRect(0, 0, M5.Display.width(), 8, TFT_WHITE);
    M5.Display.drawRect(0, 0, M5.Display.width() - 30, 8, TFT_BLACK);
    M5.Display.fillRect(0, 0, ((float)level / 100 * (M5.Display.width() - 30)), 8, TFT_BLACK);
    displayText(level_str, &Font0, M5.Display.width(), 0, ALIGN_RIGHT, 1);
    M5.Display.endWrite();

    // Date ----------------------------------------------------------
    M5.Display.startWrite();
    char date_str[30];
    sprintf(date_str, "%s%s %s", months[timeinfo.tm_mon], days[timeinfo.tm_mday], week[timeinfo.tm_wday]);
    displayText(date_str, &efontCN_24, M5.Display.width() / 2, 25, ALIGN_CENTER, 1);
    M5.Display.endWrite();

    bool temp_avaliable = sht3x.update();
    bool press_avaliable = qmp.update();
    // Hour ----------------------------------------------------------
    M5.Display.startWrite();
    char hour_str[6];
    sprintf(hour_str, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    displayText(hour_str, &Font4,
                M5.Display.width() / 2 - ((temp_avaliable && press_avaliable) ? 23 : 0), 65,
                ALIGN_CENTER, 2);
    M5.Display.endWrite();

    if (temp_avaliable && press_avaliable)
    {
        if ((tm / 60) % 2)
        {
            // Temp ----------------------------------------------------------
            M5.Display.startWrite();
            char temp_str[5];
            sprintf(temp_str, "%2.1fC", sht3x.cTemp);
            displayText(temp_str, &Font2, M5.Display.width() / 2 + 67, 69, ALIGN_CENTER, 1);
            M5.Display.endWrite();
            // Hum ----------------------------------------------------------
            M5.Display.startWrite();
            char hum_str[5];
            sprintf(hum_str, "%0.1f%%", sht3x.humidity);
            displayText(hum_str, &Font2, M5.Display.width() / 2 + 67, 89, ALIGN_CENTER, 1);
            M5.Display.endWrite();
        }
        else
        {
            // Temp ----------------------------------------------------------
            M5.Display.startWrite();
            char press_str[10];
            sprintf(press_str, "%.0fkPa", qmp.pressure / 1000);
            displayText(press_str, &Font2, M5.Display.width() / 2 + 67, 69, ALIGN_CENTER, 1);
            M5.Display.endWrite();
            // Hum ----------------------------------------------------------
            M5.Display.startWrite();
            char alt_str[10];
            sprintf(alt_str, "%0.0fM", qmp.altitude);
            displayText(alt_str, &Font2, M5.Display.width() / 2 + 67, 89, ALIGN_CENTER, 1);
            M5.Display.endWrite();
        }
    }

    M5.Display.startWrite();
    displayText(poem[(tm / 60) % 188][0], &efontCN_16, M5.Display.width() / 2, 125, ALIGN_CENTER, 1);
    displayText(poem[(tm / 60) % 188][1], &efontCN_16, M5.Display.width() / 2, 155, ALIGN_CENTER, 1);
    M5.Display.endWrite();

    M5.Display.endWrite();
    M5.Display.display();
    M5.Display.waitDisplay();
}